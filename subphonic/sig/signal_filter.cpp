#include "signal_filter.h"

/*Ax/By*/

namespace spl{

IIRFilter::IIRFilter(int num_x, int num_y)
{
    this->num_x=num_x;
    this->num_y=num_y;
   
    DASSERT(this->num_x>0);
    DASSERT(this->num_y>0);
   
    Xcoeff = new smp_t[num_x];
    Ycoeff = new smp_t[num_y];
   
    inbuff = new RingBuffer<smp_t>(num_x); 
    outbuff = new RingBuffer<smp_t>(num_y);
   
    resetState();
   
    in=NULL;
}

IIRFilter::~IIRFilter()
{
    delete[] Xcoeff;
    delete[] Ycoeff;
   
    delete outbuff;
    delete inbuff;
}

int IIRFilter::numX()
{
    return num_x;
}

int IIRFilter::numY()
{
    return num_y;
}


void IIRFilter::resetState()
{
    inbuff->clear();
    outbuff->clear();
   
    for(int i=0;i<num_x;i++)
    {
        inbuff->add(0);
    }
   
    //outbuff needs one less than inbuff because 
    //inbuff also stores the current value which out does not
    for(int i=0;i<num_y-1;i++)
    {
        outbuff->add(0);
    }
}

void IIRFilter::setCoeff(smp_t* XC, smp_t* YC)
{
    //pre divide by YC[0] so that output can be written as the sum 2)
    DASSERT( !feq(YC[0], (smp_t)0.0) );
   
    for(int i=0;i<num_x;i++)
    {
        Xcoeff[i]=XC[i]/YC[0];
        DASSERT(!isnan(Xcoeff[i]));
	
        //cout << "x" << i << " " << Xcoeff[i] << endl;
    }
   
    for(int i=0;i<num_y;i++)
    {
        Ycoeff[i]=YC[i]/YC[0];
        DASSERT(!isnan(Ycoeff[i]));
	
        //cout << "y" << i << " " << Ycoeff[i] << endl;
    }
}


void IIRFilter::setInput(Value** val)
{
    in=val;
}

Value** IIRFilter::getInput()
{
    return in;
}

smp_t IIRFilter::nextValue()
{
    smp_t val=0;
    if(in!=NULL)val = (*in)->nextValue();
   
    static bool warned=false;
    if(!isfinite(val))
    {
        if(!warned)DERROR("input is not finite! ( one warning only )");
        warned=true;
        val=0;
    }
   
    smp_t sum=0;
   
   
    inbuff->get();//head is most recent tail is oldest, discard it
    inbuff->add(val); //add new
   
    //cout << "in : " << val;
    for(int i=0;i<num_x;i++)
    {
        sum += inbuff->peek(num_x-1-i)*Xcoeff[i];
    }
   
    for(int i=1;i<num_y;i++)
    {
        sum -= outbuff->peek(num_y-1-i)*Ycoeff[i];
    }
   
    //FIXMEFIXME
    //keep to small numbers from escaping
    if(feq(sum, (smp_t)0.0))sum=0;
   
    //cant do this if num_y==1 then there are not elms in outbuff
    if(num_y>1)
    {
        outbuff->get();
        outbuff->add(sum);
    }
   
      
    //FIXME:DEBUG
    /*static bool warned=false;
      if(fabs(sum)>=1)
      {
      if(!warned)DERROR("filter sum >1");
      warned=true;
      reset();
      //sum=0;
      }*/
   
    //cout << "  out : " << sum << endl;
   
    if(!isfinite(sum))
    {
        if(!warned)DERROR("output is not finite! ( one warning only )");
        warned=true;
        sum=0;
    }
      
    return sum;
}


/*************/
/*BASICFILTER*/
/*************/

BasicFilter::BasicFilter(int nx, int ny, smp_t samplerate) : IIRFilter(nx, ny)
{
    cutoff=0;
    q=0;
      
    old_cut=cutoff;
    old_q=q;
   
    srate=samplerate;
   
    cut_in=NULL;
    q_in=NULL;
   
    setUseSafe(false);
    cutSafe(10, samplerate);
    qSafe(0.1,10);
   

}

void BasicFilter::setSampleRate(smp_t rate)
{
    srate=rate;
   
    //force recalc
    old_cut=-1;
    old_q=-1;
}

smp_t BasicFilter::getSampleRate()
{
    return srate;
}


smp_t BasicFilter::nextValue()
{
    smp_t want_cut=0;
    if(cut_in!=NULL)
    {
        want_cut = fabs((*cut_in)->nextValue());
    }
   
    smp_t want_q=0;
    if(q_in!=NULL)
    {
        want_q = fabs((*q_in)->nextValue());
    }
   
    bool recalc=false;
   
    if(want_cut != old_cut)
    {
        if(use_safe)
        {
            if(want_cut>cs_hi)want_cut=cs_hi;
            if(want_cut<cs_lo)want_cut=cs_lo;
        }
	
        old_cut = want_cut;
        cutoff=old_cut;
        recalc=true;
    }
   
    if(want_q != old_q)
    {
        if(use_safe)
        {
            if(want_q>qs_hi)want_q=qs_hi;
            if(want_q<qs_lo)want_q=qs_lo;
        }
	
        //change Q
        old_q = want_q;
        q=old_q;
        recalc=true;
    }
   
	
    if(recalc)
    {
        //uses 'cutoff' & 'q'
        calcCoeff();
    }
   
    return IIRFilter::nextValue();
}

void BasicFilter::reset(int index)
{
    resetState();
}


void BasicFilter::setUseSafe(bool v)
{
    use_safe=v;
}

bool BasicFilter::getUseSafe()
{
    return use_safe;
}

void BasicFilter::cutSafe(smp_t lo_hz, smp_t hi_hz)
{
    cs_lo=lo_hz;
    cs_hi=hi_hz;
}

void BasicFilter::qSafe(smp_t lo, smp_t hi)
{
    qs_lo=lo;
    qs_hi=hi;
}


Value** BasicFilter::getCutoff()
{
    return cut_in;
}

void BasicFilter::setCutoff(Value** cut)
{
    this->cut_in=cut;
}


Value** BasicFilter::getQ()
{
    return q_in;
}

void BasicFilter::setQ(Value** q)
{
    this->q_in=q;
}

/**/
smp_t Filter_Identity::nextValue()
{
    smp_t val=0;
    if(in!=NULL)val= (*in)->nextValue();
    return val;
}
/**/


/*********/
/*FILTERS*/
/*********/

/**************/
/*LOWPASS 6-18*/
/**************/

void Filter_BW_LP_6::calcCoeff()
{
    smp_t k=tan(M_PI*cutoff/srate);
   
    X[0]=k/(1+k);
    X[1]=X[0];
    Y[0]=1.0;
    Y[1]=-(1-k)/(1+k);
   
    setCoeff(X, Y);
}

void Filter_BW_LP_12::calcCoeff()
{
    smp_t w=2*M_PI*cutoff/srate;
   
    smp_t k=tan(w/2);
    smp_t k2=k*k;
   
    X[0] =   k2;
    X[1] = 2*k2;
    X[2] =   k2;
   
    Y[0] = 1 + M_SQRT2*k + k2;
    Y[1] =-2 + 2*k2;
    Y[2] = 1 - M_SQRT2*k + k2;
   
    setCoeff(X, Y);
}

void Filter_BW_LP_18::calcCoeff()
{
    smp_t w=2*M_PI*cutoff/srate;
   
    smp_t k=tan(w/2);
    smp_t k2=k*k;
    smp_t k3=k2*k;
   
    X[0]=  k3;
    X[1]=3*k3;
    X[2]=3*k3;
    X[3]=  k3;
   
    Y[0]=  k3 + 2*k2 + 2*k + 1;
    Y[1]=3*k3 + 2*k2 - 2*k - 3;
    Y[2]=3*k3 - 2*k2 - 2*k + 3;
    Y[3]=  k3 - 2*k2 + 2*k - 1;
   
    setCoeff(X, Y);
}


/**********/
/*HIGHPASS*/
/**********/

void Filter_BW_HP_6::calcCoeff()
{
    smp_t k=tan(M_PI*cutoff/srate);
    X[0]=1/(1+k);
    X[1]=-X[0];
    Y[0]=1.0;
    Y[1]=((k-1)/(1+k));
   
    setCoeff(X, Y);
}

void Filter_BW_HP_12::calcCoeff()
{
    smp_t k=tan(M_PI*cutoff/srate);
    smp_t k2=k*k;
   
    X[0] = 1;
    X[1] =-2;
    X[2] = 1;
    Y[0] = k2 + M_SQRT2*k + 1;
    Y[1] = 2*k2 - 2;
    Y[2] = k2 - M_SQRT2*k + 1;
   
    setCoeff(X, Y);
}

void Filter_BW_HP_18::calcCoeff()
{
    smp_t w=2*M_PI*cutoff/srate;
   
    smp_t k=tan(w/2);
    smp_t k2=k*k;
    smp_t k3=k2*k;
   
    X[0]=  1;
    X[1]= -3;
    X[2]=  3;
    X[3]= -1;
   
    Y[0]=  1 + 2*k + 2*k2 +   k3;
    Y[1]= -3 - 2*k + 2*k2 + 3*k3;
    Y[2]=  3 - 2*k - 2*k2 + 3*k3;
    Y[3]= -1 + 2*k - 2*k2 +   k3;
   
    setCoeff(X, Y);
}



/**********/
/*BANDPASS*/
/**********/

void Filter_BW_BP_6::calcCoeff()
{
    smp_t wc=2*M_PI*cutoff/srate;
    smp_t B = wc/q;
   
    smp_t wu = B/2.0 + sqrt(B*B/4.0+wc*wc);
    smp_t wl = wu-B;
   
   
    smp_t w0 = sqrt(wu*wl);
    smp_t B0= wu-wl;
   
    smp_t k=tan(w0/2);
    smp_t k2=k*k;
   
    smp_t b=tan(B0/2);
   
   
    X[0] = b;
    X[1] = 0;
    X[2] = -b;
    Y[0] = 1+k2+b;
    Y[1] = -2+2*k2;
    Y[2] = 1+k2-b;
   
    setCoeff(X, Y);
}

void Filter_BW_BP_12::calcCoeff()
{
    smp_t wc=2*M_PI*cutoff/srate;
    smp_t B = wc/q;
   
    smp_t wu = B/2.0 + sqrt(B*B/4.0+wc*wc);
    smp_t wl = wu-B;
   
   
    smp_t w0 = sqrt(wu*wl);
    smp_t B0= wu-wl;
   
    smp_t k=tan(w0/2);
    smp_t k2=k*k;
    smp_t k4=k2*k2;
   
    smp_t b=tan(B0/2);
    smp_t b2=b*b;
    smp_t sqb = sqrt(2.0)*b;
   
   
    X[0] = b2;
    X[1] = 0;
    X[2] = -2*b2;
    X[3] = 0;
    X[4] = b2;
   
    Y[0] = 1+sqb+2*k2+b2+sqb*k2+k4;
    Y[1] = -4-2*sqb+2*sqb*k2+4*k4;
    Y[2] = 6-4*k2-2*b2+6*k4;
    Y[3] = 2*sqb-4-2*sqb*k2+4*k4;
    Y[4] = 1-sqb+2*k2+b2+k4-sqb*k2;
   
    setCoeff(X, Y);
}

}