#include "signal_osc.h"

#include "../math/mathutil.h"

namespace spl{

using namespace spl;

/******/
/*BASE*/
/******/

SignalGenerator::SignalGenerator(SigGenValue* gen)
{
    index=0;
    i_time=0;
   
    shape=NULL;
    amp=NULL;
    period=NULL;
   
    sync_in=NULL;
   
    sync_out = new OneShot();
   
    do_sync=false;
   
    valgen=gen;
   
    resync=true;
   
    setSuppressedPeriods(0);
    sup_count=0;
   
    p=0;
    a=0;
    val=0;
}

SignalGenerator::~SignalGenerator()
{
    delete valgen;
   
    delete sync_out;
}

/*Value** SignalGenerator::getAmp()
  {
  return amp;
  }
  * 
  Value** SignalGenerator::getPeriod()
  {
  return period;
  }
  * 
  Value** SignalGenerator::getShape()
  {
  return shape;
  }
*/

smp_t SignalGenerator::nextValue()
{
    if(sync_in!=NULL)
    {
        smp_t sval = (*sync_in)->nextValue();
        if(fabs(sval)>GATE_ZERO_LVL)
        {
            if(resync)
            {
                reset(0, sval); //see 1)
                resync=false;
                sup_count=sy_sup; //resets re_sup in reset()
                //cout << "synced: " << index << endl;
            }
        }
        else resync=true;
    }
   
    a=1.0;
    if(amp!=NULL)a = (*amp)->nextValue();
   
    p=2205;
    if(period!=NULL)p = (int)(*period)->nextValue();
   
   
    smp_t s=0.5;
    if(shape!=NULL)s = fabs((*shape)->nextValue()/SMP_MAX);
   
    /*debug
    //cout << p << endl;
    a=0.5;
    p=11025;
    s=0;*/
   
    val=0;
    if(p!=0)
    {
        if(sup_count==0)val = valgen->genValue(a, p, s, index, i_time);
        else val = 0;
	
        DASSERT(!isnan(val));
        if(isnan(val))val=0;
	
        if(p<0)index--;
        else index++;
	
        index %= p;
        if(index==0)
        {
            if(sup_count>0)
            {
                sup_count--;
		  
                //FIXME: maybe reset i_time here for better precission
                //if(sup_count==0)i_tim
            }
	     
	     
            do_sync=true;
        }
	
	
        smp_t inc = M_2PI/(smp_t)p;
        i_time+=inc;
    } //what to do when p==0 ?
   
   
    if(sync_out!=NULL && do_sync)
    {
        //period done send pulse
        //1) NEXT value for "master" vill be sample_index=0 (just went zero)
	
        do_sync=false;
	
        //cout << "sync out" << endl;
	
        sync_out->set(i_time);
    }
   
    return val;
   
}


void SignalGenerator::setAmp(Value** a)
{
    amp = a;
}

void SignalGenerator::setPeriod(Value** p)
{
    period = p;
}

void SignalGenerator::reset(int i)
{
    reset(i, i*M_2PI/(smp_t)p);
}

void SignalGenerator::reset(int i, smp_t i_t)
{
    index=i;
   
    if(p!=0)
    {
        index%=p;
        i_time=i_t*sign(p);
    }
    else 
    {
        //FIXME:
        i_time=0;
        //index=0;
    }
   
   
    sup_count=re_sup;
}

//this only applys to square/triangle, and for pulse it sets the duty cycle for now
//shape must be between 0 SSHORT_MAX where SSHORT_MAX/2 is standard shape
void SignalGenerator::setShape(Value** v)
{
    shape=v;
}

void SignalGenerator::setSyncIn(Value** in)
{
    sync_in=in;
}

/*Value** SignalGenerator::getSyncIn()
  {
  return sync_in;
  }
*/

Value** SignalGenerator::getSyncOut()
{
    return (Value**)&sync_out;
}

void SignalGenerator::setSuppressedPeriods(uint supp)
{
    setSuppressedPeriodsSync(supp);
    setSuppressedPeriodsReset(supp);
}

void SignalGenerator::setSuppressedPeriodsSync(uint supp)
{
    sy_sup=supp;
}

void SignalGenerator::setSuppressedPeriodsReset(uint supp)
{
    re_sup=supp;
}




/******/
/*OSCS*/
/******/

smp_t SquareValue::genValue(smp_t a, int p, smp_t s, int index, smp_t i_time)
{
    smp_t val;
   
    val = a*squ(i_time, s);
   
    return val;
}

smp_t TriangleValue::genValue(smp_t a, int p, smp_t s, int index, smp_t i_time)
{
    smp_t val;

    val = a*tri(i_time, s);
   
    return val;
}

smp_t SawtoothValue::genValue(smp_t a, int p, smp_t s, int index, smp_t i_time)
{
    smp_t val;

    smp_t ts=s;
    if(ts<0.5)ts=0.5;
    val = a*tri(i_time, 2*(ts-0.5));
   
    return val;
}


smp_t SineValue::genValue(smp_t a, int p, smp_t s, int index, smp_t i_time)
{
    smp_t val = (smp_t)(a*sin(i_time));
   
    return val;
}

smp_t PulseValue::genValue(smp_t a, int p, smp_t s, int index, smp_t i_time)
{
    smp_t val;
   
    if(index<=s*p)
    {
        val = a;
    }
    else val=0;
   
    return val;
}

smp_t WNoiseValue::genValue(smp_t a, int p, smp_t s, int index, smp_t i_time)
{
    return (smp_t)(-a+2*a*(rand()/(smp_t)RAND_MAX));
}




/**********/
/*ComboGen*/
/**********/

ComboGen::ComboGen()
{
    a_i_time=0;
    i_time=a_i_time;

    a=1.0;
    p=2205;
    s=0.5;
   
    shape=NULL;
    amp=NULL;
    period=NULL;
    phase=NULL;
   
    sync_in=NULL;
   
    sync_out = new OneShot();
   
    do_sync=false;
   
    gen[SQUARE] = new SGST_Square(this, 0);
    gen[TRI]    = new SGST_Triangle(this, 1);
    gen[SAW]    = new SGST_Sawtooth(this, 2);
    gen[PULSE]  = new SGST_Pulse(this, 3);
    gen[SINE]   = new SGST_Sine(this, 4);
    gen[NOISE]  = new SGST_WNoise(this, 5);
   
    re_sync=true;
}

ComboGen::~ComboGen()
{
    for(uint i=0;i<NUM_OSC;i++)
    {
        delete gen[i];
    }
   
    delete sync_out;
}

Value** ComboGen::getGen(TYPE t)
{
    DASSERT(t>=0 && t<NUM_OSC);
   
    return (Value**)&gen[t];
}


/*Value** ComboGen::getAmp()
  {
  return amp;
  }
  * 
  Value** ComboGen::getPeriod()
  {
  return period;
  }
  * 
  Value** ComboGen::getShape()
  {
  return shape;
  }*/


smp_t ComboGen::nextValue()
{
    if(sync_in!=NULL)
    {
        smp_t sval = (*sync_in)->nextValue();
        if(fabs(sval)>GATE_ZERO_LVL /*FIXMENOW: problem?*/)
        {
            if(re_sync)
            {
                //cout << "syncing" << endl;
                reset(0, sval-1); //see 1)
                re_sync=false;
            }
        }
        else re_sync=true;
	
        //sup_count=sy_sup;
    }
   
    a=1.0;
    if(amp!=NULL)a = (*amp)->nextValue();
   
    p=2205;
    if(period!=NULL)p = (*period)->nextValue();
   
    s=0.5;
    if(shape!=NULL)s = fabs((*shape)->nextValue()/SMP_MAX);
   
    ph=0;
    if(phase!=NULL)ph=(*phase)->nextValue()*M_2PI; //all genereators have 2pi period

    if(!feq(p, (smp_t)0.0)) //p==0 is not allowed: stop generation
    {
        //cout << p << endl; 
        //a_* is used so that calculation is done on right index
        i_time=a_i_time; //gens will use i_time

        for(uint i=0;i<NUM_OSC;i++)
        {
            recalc[i]=true;
        }
	
        //FIXME: removing multiples of M_2PI makes FM handled bad?
        smp_t inc = M_2PI/p; //same as M_2PI*freq
        a_i_time+=inc;
	
        while(a_i_time>=M_2PI)
        {
            a_i_time-=M_2PI;
            do_sync=true;
        }
    }
   
   
    if(sync_out!=NULL && do_sync)
    {
        //period done send pulse
        //1) NEXT value for "master" vill be a_i_time
	
        do_sync=false;
		
        sync_out->set(a_i_time+1/*+1 to get pulse subtracted at sync*/);
        //cout << "sync " << a_i_time << endl;
    }
   
    return 0;
   
}

void ComboGen::setAmp(Value** a)
{
    amp = a;
}

void ComboGen::setPeriod(Value** p)
{
    period = p;
}

void ComboGen::setPhase(Value** v)
{
    phase=v;
}

void ComboGen::reset(int i)
{
    reset(i, i*M_2PI/p);
}


void ComboGen::reset(int i, smp_t i_t)
{
    //cout << "sync" << endl;
   
    if(!feq(p, (smp_t)0.0))
    {
        i_time=i_t*sign(p);
        while(i_time>M_2PI)
        {
            i_time-=M_2PI;
        }
    }
    else 
    {
        i_time=0;
    }
   
    a_i_time=i_time;
    //sup_count=re_sup;
}

void ComboGen::setShape(Value** v)
{
    shape=v;
}

void ComboGen::setSyncIn(Value** in)
{
    sync_in=in;
}

/*Value** ComboGen::getSyncIn()
  {
  return sync_in;
  }
*/

Value** ComboGen::getSyncOut()
{
    return (Value**)&sync_out;
}



/*SINGLEGEN ST*/

ComboGen::SingleGenST::SingleGenST(ComboGen* master, uint i)
  
    : a(master->a),  p(master->p),s(master->s),ph(master->ph),
      recalc(master->recalc[i]), i_time(master->i_time)
  
{
    val=0;
}

ComboGen::SingleGenST::~SingleGenST()
{
}

smp_t ComboGen::SingleGenST::nextValue()
{
    if(recalc)
    {
        reCalc();
        DASSERT(isfinite(val));
        if(!isfinite(val))val=0;
    }
   
    return val;
}

void ComboGen::SGST_Square::reCalc()
{
    val = a*squ(i_time+ph, s);
   
    recalc=false;
}

void ComboGen::SGST_Sine::reCalc()
{
    //FIXME:high freq not good...
    val = (smp_t)(a*sin(i_time+ph));
   
    //cout << i_time << endl;
    recalc=false;
}

//high to low
/* shp: \/    conventional is more calc*/
void ComboGen::SGST_Triangle::reCalc()
{
    val = a*tri(i_time+ph, s);
   
    recalc=false;
}

//def low to high
void ComboGen::SGST_Sawtooth::reCalc()
{
    smp_t ts=s;
    if(ts<0.5)ts=0.5;
    val = a*tri(i_time+ph, 2*(ts-0.5));
   
    recalc=false;
}

void ComboGen::SGST_Pulse::reCalc()
{
    //THIS IS NO GOOD:
    //val = pul(i_time, maxOf(0.08,s));
   
    //index based better?
    int index=(int)(i_time*p/M_2PI);
    if(index<=s*p)val= a;
    else val=0;
   
    recalc=false;
}

void ComboGen::SGST_WNoise::reCalc()
{
    val = (smp_t)(-a+2*a*(rand()/(smp_t)RAND_MAX));
   
    recalc=false;
}

}