#include "signal_effect.h"

#include "../util/debug.h"

namespace spl{

BitCrush::BitCrush()
{
    setNBits(32);

    input=NULL;
    in_nbits=NULL;
}

BitCrush::~BitCrush()
{
}

void BitCrush::setInput(Value** in)
{
    input=in;
}

void BitCrush::setNBits(uint n)
{
    if(n==0)
    {
        ERROR("min bits=1");
        n=1;
    }
   
    nbits=n;
    maxval=1<<(n-1);//-1 due to sign, actualy now -128, 128, not -128, 127 (no matter)
}

void BitCrush::setNBitsIN(Value** in)
{
    in_nbits=in;
}


smp_t BitCrush::nextValue()
{
    smp_t out=0;
    smp_t in=0;
    if(input!=NULL)in = (*input)->nextValue();
   
    if(in_nbits!=NULL)
    {
        uint n=(uint)(*in_nbits)->nextValue();
        if(n==0)n=1;
        else if(n>32)n=32;
        uint max=1<<(n-1);
	
        long v=(long)(round(in*max));
        out = v/(smp_t)max;
    }
    else
    {
        long v=(long)(round(in*maxval));
        out = v/(smp_t)maxval;
    }
   
    return out;
}




ValueBuffer::ValueBuffer() : buffer(44100,0)
{
    only_sampled=false;
    sampled=0;
    sampling=false;
    regate_ok=true;
   
    input=NULL;
    v_gate=NULL;
    read=NULL;
   
    len=buffer.size();//44100;
    //buffer.resize(len);
}

ValueBuffer::~ValueBuffer()
{
    //buffer.clear();
}


void ValueBuffer::setInput(Value** in)
{
    input=in;
}


void ValueBuffer::setLen(uint samples)
{
    if(buffer.size()<samples)
    {
        buffer.resize(samples*2);
    }
    len=samples;
}

void ValueBuffer::setOnlyReadSampled(bool val)
{
    only_sampled=val;
}


void ValueBuffer::setGate(Value** in)
{
    v_gate=in;
}

void ValueBuffer::setRead(Value** in)
{
    read=in;
}

smp_t ValueBuffer::nextValue()
{
    uint index=0;
    if(read!=NULL)index = (uint)(*read)->nextValue();
    if(index>=len)
    {
        ERROR("sample index > buffer len");
        if(len==0)index=0;
        else index=len-1;
    }
   
    smp_t in=0;
    if(input!=NULL)in = (*input)->nextValue();
   
    smp_t gate=0;
    if(v_gate!=NULL)gate = (*v_gate)->nextValue();
   
    if(gate>=GATE_ZERO_LVL)
    {
        if(regate_ok)
        {
            regate_ok=false;
	     
            sampling=true;
            sampled=0;
        }
    }
    else
    {
        regate_ok=true;
    }
   
    if(sampled==len)sampling=false;
   
    if(sampling)
    {
        buffer[sampled]=in;
        sampled++;
	
        if(only_sampled)
        {
            if(index>=sampled)index=sampled-1;
        }
	
    }
   
    return buffer[index];
}





ClipOverdrive::ClipOverdrive()
{
    in=NULL;
}

void ClipOverdrive::setInput(Value** in)
{
    this->in=in;
}


smp_t ClipOverdrive::nextValue()
{
    smp_t val = 0;
    if(in!=NULL)val = (*in)->nextValue();
   
    smp_t sgn = (val>0) ? 1 : -1;
   
    smp_t aval = sgn*val;
   
    smp_t th = 1.0/3.0;
   
    if(aval < th)return 2.0*val;
   
    if(aval < 2*th)
    {
        smp_t s = (2.0-3.0*aval);
	
        return sgn*(3.0-s*s)/3.0;
    }
   
    return sgn;
   
}



/**********/
/*SLEWRATE*/
/**********/

SlewLimitRate::SlewLimitRate()
{
    mode=LINEAR;
   
    r_in=NULL;
    f_in=NULL;
   
    val_in=NULL;
   
    c_val=0;
    c_target=0;
   
    rise=false;
}

SlewLimitRate::~SlewLimitRate()
{
   
}

void SlewLimitRate::setMode(MODE m)
{
    mode=m;
}


void SlewLimitRate::setRiseRate(Value** v)
{
    r_in=v;
}

void SlewLimitRate::setFallRate(Value** v)
{
    f_in=v;
}


void SlewLimitRate::setInput(Value** v)
{
    val_in=v;
}


smp_t SlewLimitRate::nextValue()
{
    smp_t c_target = 0;
    if(val_in!=NULL)c_target = (*val_in)->nextValue();
   
    //in samples
    smp_t r=1;
    smp_t f=1;
   
    if(r_in!=NULL)r = fabs((*r_in)->nextValue());
    if(f_in!=NULL)f = fabs((*f_in)->nextValue());
   
    //nothing to do if c_target==current
    if(c_target == c_val)return c_val;
   
    if(c_target > c_val)rise=true;
    else rise=false;
   
    if(mode==LINEAR)
    {
        if(rise)c_val += r;
        else c_val -= f;
    }
    else
    {
        DERROR("error");
    }
   
   
    if(rise)
    {
        if(c_val > c_target)c_val = c_target;
    }
    else 
    {
        if(c_val < c_target)c_val = c_target;
    }
   
   
    return c_val;
}




/**********/
/*SLEWTIME*/
/**********/

SlewLimitTime::SlewLimitTime()
{
    mode=LINEAR;
   
    rt_in=NULL;
    ft_in=NULL;
   
    val_in=NULL;
   
    c_val=0;
    c_target=0;
    c_time=0;
    c_dv=0;
    c_dv_dt=0;
   
    time_m=1.0;
   
    rise=false; //no matter
}

SlewLimitTime::~SlewLimitTime()
{
   
}

void SlewLimitTime::setMode(MODE m)
{
    mode=m;
}


void SlewLimitTime::setRiseTime(Value** v)
{
    rt_in=v;
}

void SlewLimitTime::setFallTime(Value** v)
{
    ft_in=v;
}


void SlewLimitTime::setInput(Value** v)
{
    val_in=v;
}

void SlewLimitTime::setTimeMult(smp_t m)
{
    time_m=m;
}


smp_t SlewLimitTime::nextValue()
{
    smp_t in = 0;
    if(val_in!=NULL)in = (*val_in)->nextValue();
   
    //in samples
    smp_t rt=1;
    smp_t ft=1;
   
    if(rt_in!=NULL)rt = fabs(time_m * (*rt_in)->nextValue());
    if(ft_in!=NULL)ft = fabs(time_m * (*ft_in)->nextValue());
   
    //nothing to do if in==current
    if(in == c_val)return c_val;
   
    if(in > c_val)rise=true;
    else rise=false;
   
    if(rise)
    {
        if(rt==0)
        {
            c_val=in;
            c_target=c_val;
	     
            return c_val;
        }
    }
    else
    {
        if(ft==0)
        {
            c_val=in;
            c_target=c_val;
	     
            return c_val;
        }
    }
   
    if(in != c_target)
    {
        //restart at evey level change
        c_target=in;
        c_dv = c_target-c_val;
        c_start=c_val;
	
        if(mode==LINEAR)
        {
            //calc everytime instead: for changing times
            /*if(rise)c_dv_dt = c_dv/rt;
              else c_dv_dt = c_dv/ft;*/
        }
        else if(mode==SINE)
        {
            c_time=1; //otherwise no change this iter
        }
        else
        {
            DERROR("unsupported mode");
        }
    }
   
    if(mode==LINEAR)
    {
        if(rise)c_dv_dt = c_dv/rt;
        else c_dv_dt = c_dv/ft;
	
        c_val += c_dv_dt;
    }
    else if(mode==SINE)
    {
        if(rise)c_val = c_start + c_dv*sin(M_PI_2*c_time/rt);
        else c_val = c_start + c_dv*sin(M_PI_2*c_time/ft);
	
        if(rise)
        {
            if(c_time<rt)c_time++;
        }
        else 
        {
            if(c_time<ft)c_time++;
        }
    }
    else
    {
        DERROR("error");
    }
   
   
    if(rise)
    {
        if(c_val > c_target)c_val = c_target;
    }
    else 
    {
        if(c_val < c_target)c_val = c_target;
    }
   
   
    return c_val;
}





/******/
/*GAIN*/
/******/

Gain::Gain(smp_t gain, Value** depth)
{
    in=NULL;
    d=gain;
    this->depth=depth;
}

void Gain::setGainIn(Value** v)
{
    depth=v;
}

void Gain::setGain(smp_t d)
{
    this->d=d;
}

smp_t Gain::getGain()
{
    return d;
}


void Gain::setInput(Value** v)
{
    in = v;
}

smp_t Gain::nextValue()
{
    //DASSERT(in!=NULL); //still might want to drain depth
   
    smp_t val = 0;
    if(in!=NULL)val = (*in)->nextValue();
   
    smp_t dv =1.0;
    if(depth!=NULL)dv = (*depth)->nextValue();
   
    return d*dv*val;
}



/******/
/*RECT*/
/******/

Rectify::Rectify()
{
    val = NULL;
    half=false;
}

void Rectify::setInput(Value** v)
{
    val = v;
}

void Rectify::setHalf(bool h)
{
    half=h;
}

bool Rectify::getHalf()
{
    return half;
}

/*bool* Rectify::halfPtr()
  {
  return &half;
  }*/


smp_t Rectify::nextValue()
{
    smp_t v = 0;
    if(val!=NULL)v = (*val)->nextValue();
   
    if(half)
    {
        if(v>0)return v;
        return 0;
    }
   
    return fabs(v);
}






/*****************/
/*SAMPLE AND HOLD*/
/*****************/

SampleAndHold::SampleAndHold()
{
    retrig=true;
   
    sample=0;
    index=0;
   
    in=NULL;
    clock_in=NULL;
    per=NULL;
    c_len_in=NULL;
   
    clock_out = new OneShot();
   
    setClockLen(1);
}

SampleAndHold::~SampleAndHold()
{
    delete clock_out;
}


void SampleAndHold::setPeriod(Value** in)
{
    per=in;
}

void SampleAndHold::setInput(Value** in)
{
    this->in=in;
}

void SampleAndHold::setClockIn(Value** in)
{
    clock_in=in;
}

Value** SampleAndHold::getClockOut()
{
    return (Value**)&clock_out;
}

void SampleAndHold::setClockLen(smp_t v)
{
    c_len = (uint)v;
    //clock_out->setShotLen((uint)v);
}

void SampleAndHold::setClockLenIn(Value** in)
{
    c_len_in = in;
   
}

smp_t SampleAndHold::nextValue()
{
    smp_t val=0;
   
    if(in!=NULL)val=(*in)->nextValue();
   
    uint ulen = c_len;
    if(c_len_in != NULL)ulen = (uint)(*c_len_in)->nextValue(); 
    clock_out->setShotLen(ulen);
   
    if(clock_in==NULL)
    {
        uint p=0;
        if(per!=NULL)p = (uint)fabs((*per)->nextValue());
        if(p==0)return sample;
	
        if(index==0)
        {
            sample = val;
            clock_out->set();
        }
	
        index++;
        index%=p;
    }
    else
    {
        smp_t cv = (*clock_in)->nextValue();
        if(retrig && cv > GATE_ZERO_LVL)
        {
            sample = val;
            clock_out->set();
	     
            retrig=false;
        }
        else if(cv<=0)retrig=true;
    }
   
   
   
    return sample;
}



/***********/
/*CrossFade*/
/***********/

Crossfade::Crossfade()
{
    fval = 0;
    in[0] = NULL;
    in[1] = NULL;
   
    out[0] = new Const(0);
    out[1] = new Const(0);
   
   
    setMix(true);
}

Crossfade::~Crossfade()
{
    delete out[0];
    delete out[1];
}


void Crossfade::setInput(Value** in, uint chan/*0,1*/)
{
    DASSERT(chan<=1);
    if(chan>1)chan=1;
   
    this->in[chan]=in;
}


//if mix is on, chan0 is only one used; chan1 = zero
Value** Crossfade::getOutput(uint chan)
{
    DASSERT(chan<=1);
    if(chan>1)chan=1;
   
    return (Value**)&out[chan];
}


void Crossfade::setMix(bool mix_to_ch0)
{
    mix=mix_to_ch0;
}


//all other values that 0-1 set as 0
void Crossfade::setFade(Value** ctrl)
{
    fade=ctrl;
}


//unused if setFade(Value**)
void Crossfade::setFade(smp_t v)
{
    fval=v;
}


smp_t Crossfade::nextValue()
{
    smp_t in0=0;
    smp_t in1=0;
   
    if(in[0] != NULL)in0 = (*in[0])->nextValue();
    if(in[1] != NULL)in1 = (*in[1])->nextValue();
   
    smp_t f=fval;
    if(fade != NULL) f = (*fade)->nextValue();
   
    if(f>1)f=1;
    if(f<0)f=0;
   
    if(mix)
    {
        out[0]->set(in0*(1-f)+in1*f);
    }
    else
    {
        out[0]->set(in0*(1-f));
        out[1]->set(in1*f);
    }
   
    return 0;
}




/********/
/*St_fft*/
/********/

St_fft::St_fft(uint n, uint hop_size, Window<smp_t>* window)
{
    DASSERT(n==window->getLen());
   
    this->n=n;
    this->hop_size=hop_size;
   
    //must make room for
    int ib_sz = n;
    int ob_sz = n;///hop_size;
   
    //everytime in buffer reaches 'n' take a FFT frame and the jump hop_size and wait for next
    in_buff = new ring(ib_sz);
   
    out_ready=0; //samples ready to be returned
    oadd_index=0;
   
    //every time a FFT frame is taken: IFFT it and overlap-add it to this
    //at oadd_index, oadd_index+=hop_size
    //out_ready+=hop_size
   
    //when reading: oadd_index--
    //out_ready--
   
    out_buff = new ring(ob_sz);
   
    //some copying, maybe fix if slow
    fft_in = new smp_t[n];
    fft_out = new Complex[n];
   
    ifft_out = new smp_t[n];
   
   
    for(uint i=0;i<n;i++)
    {
        out_buff->add(0);
    }
   
    win=window;
   
    setShiftSpectrum(false);
    setDebug(false);
   
    process=NULL;
   
    in=NULL;
   
    fft = new FFTfixed<smp_t>();
    fft->setup(n);
    VERBOSE2(cout << "fft used: " << fft->getBytesUsed()/(1024*1024.0) << "mb" << endl;);
}

St_fft::~St_fft()
{
    delete in_buff;
    delete out_buff;
   
    delete[] fft_in;
    delete[] fft_out;
    delete[] ifft_out;
   
    delete win;
   
    delete fft;
}


smp_t St_fft::nextValue()
{
    smp_t inval = 0;
    if(in!=NULL)inval=(*in)->nextValue();
   
    in_buff->add(inval);
   
    if(in_buff->inBuffer() >= n)
    {
        for(uint i=0;i<n;i++)
        {
            fft_in[i] = in_buff->peek(i)*win->get(i);
	     
            //FIXMEFIXME: fft(mult) gets REALLY slow when numbers get to 1e-305
            //WHY?
            if(feq(fft_in[i], (smp_t)0.0))fft_in[i]=0; 
        }
	
        //::fft(fft_in, fft_out, n);
        fft->fft(fft_in, fft_out);
	
        if(shift)circularHalfShift(fft_out, n);
	
        //PROCESS HERE
        if(process!=NULL)
        {
            bool ok = (*process)(fft_out, n, shift);
            DASSERT(ok);
        }
	
        if(shift)circularHalfShift(fft_out, n);
	
        //::ifft(fft_out, ifft_out, n);
        fft->ifft(fft_out, ifft_out);
	
        DASSERT(oadd_index==0);
	
        for(uint i=0;i<n;i++)
        {
            out_buff->peek(i+oadd_index) += ifft_out[i]*win->get(i);
        }
	
	
	
	
        /*DEBUG*/
        if(do_debug)
        {
            //FFT ERROR
            smp_t err = fft_in[0]-ifft_out[0];
            err = fabs(err);
	     
            uint max=0;
            smp_t ml=err;
	     
            for(int i=1;i<n;i++)
            {
                err = fft_in[i]-ifft_out[i];
                err = fabs(err);
		  
                if(err>ml)
                {
                    max=i;
                    ml=err;
                }
            }
	     
            if(ml>max_fft_err)
            {
                max_fft_err =  ml;
		  
            }
	     
            //cout << ml << endl;
	     
            //ADD: RESYNTH ERROR
        }
        /*END DEBUG*/
	
	
	
	
        in_buff->jump(hop_size);
	
        oadd_index += hop_size;
        out_ready  += hop_size;
	
    }
   
   
    smp_t ret=0;
   
    if(out_ready>0)
    {
        ret = out_buff->get();
        out_buff->add(0); //beacuse 'overlap_add' needs values
	
        oadd_index--;
        out_ready--;
	
        DASSERT(out_buff->inBuffer()==n);
    }
   
    return ret;
}

void St_fft::setProcess(Process* p)
{
    process=p;
}


template<class T>
void St_fft::circularHalfShift(T* in_out, uint n)
{
    uint hn = n/2;
    T* tmp = new T[n];
    memcpy(tmp, in_out, sizeof(T)*n);
   
    memcpy(in_out, tmp+hn, sizeof(T)*hn);
    memcpy(in_out+hn, tmp, sizeof(T)*hn);
   
    delete[] tmp;
}


void St_fft::setInput(Value** v)
{
    in=v;
}

void St_fft::setShiftSpectrum(bool v)
{
    shift=v;
}

bool St_fft::getShiftSpectrum()
{
    return shift;
}


void St_fft::setDebug(bool v)
{
    do_debug=v;
}

void St_fft::resetDebug()
{
    max_fft_err=-1;
}


smp_t St_fft::d_getMaxAbsFFTError()
{
    return max_fft_err;
}

//smp_t St_fft::d_getMaxResynthError()


/*res power of 2, number of elements in a table entry*/
/*
  WaveShaper::WaveShaper(int res, int ser_len)
  {
  ncoeff=ser_len;
  resolution=res;
  * 
  coeff = new smp_t[ncoeff];
  for(int i=0;i<ncoeff;i++)
  {
  coeff[i]=0.0;
  }
  * 
  coeff[0]=1.0;
  nelm = (int)(2*SMP_MAX/resolution);
  table = new smp_t[nelm];
  get_mode=GET_SHARP;
  calc_mode=SHAPE_POLYNOM;
  }
  * 
  void WaveShaper::setGetMode(int mode)
  {
  get_mode=mode;
  }
  * 
  * 
  void WaveShaper::zeroTable()
  {
  for(int x=0;x<nelm;x++)
  {
  table[x]=0;
  }
  }
  * 
  void WaveShaper::addInput(Value** v)
  {
  in=v;
  }
  * 
  void WaveShaper::calcTable()
  {
  //speed up?
  //zeroTable();
  if(calc_mode==SHAPE_POLYNOM)
  {
  smp_t tmp[nelm];
  memset(tmp,0x0,sizeof(smp_t)*nelm);
  * 
  //calc for x: -1 to 1
  smp_t x=-1;
  smp_t slope=2/(smp_t)nelm;
  * 
  smp_t max=1.0;
  for(int i=0;i<nelm;i++)
  { 
  smp_t ux=x;
  for(int c=0;c<ncoeff;c++)
  {
  tmp[i]+=ux*coeff[c]; //x^1*c[0]+x^2*c[1]+...+x^n*c[n-1]
  ux*=x;
  }
  if(fabs(tmp[i])>max)max=fabs(tmp[i]);
  x+=slope;
  }
  for(int i=0;i<nelm;i++)
  {
  table[i]=(smp_t)((SMP_MAX-1)*tmp[i]/max); //normalize
  }
  return;
  }
  else if(calc_mode==SHAPE_SINE)
  {
  smp_t tmp[nelm];
  memset(tmp,0x0,sizeof(smp_t)*nelm);
  * 
  * 
  smp_t max=1.0;
  smp_t period = 2*M_PI/nelm;
  * 
  for(int i=0;i<nelm;i++)
  { 
  smp_t ux=1;
  for(int c=0;c<ncoeff;c++)
  {
  tmp[i]-=sin(period*ux*i)*coeff[c]; //x^1*c[0]+x^2*c[1]+...+x^n*c[n-1]
  ux+=1;
  }
  if(fabs(tmp[i])>max)max=fabs(tmp[i]);
  }
  for(int i=0;i<nelm;i++)
  {
  table[i]=(smp_t)((SMP_MAX-1)*tmp[i]/max);
  }
  return;
  }
  }
  * 
  smp_t WaveShaper::lookup(smp_t v)
  {
  //offset values form -SMP_MAX | SMP_MAX to 0 | 2*SMP_MAX
  int read = (int)((SMP_MAX+v)/resolution);
  return table[read];
  }
  * 
  smp_t WaveShaper::lookup_interpolate(smp_t v)
  {
  smp_t bv = (SMP_MAX+v)/(smp_t)resolution;
  * 
  int rp = (int)bv;
  * 
  smp_t fp = bv-read;
  smp_t interpolated = (smp_t)(fp*(table[read+1]-table[read]));
  * 
  return table[read]+interpolated;
  }
  * 
  smp_t WaveShaper::nextValue()
  {
  if(get_mode==GET_SHARP)return lookup((*in)->nextValue());
  if(get_mode==GET_SMOOTH)return lookup_interpolate((*in)->nextValue());
  return 0; //actually error
  }
  * 
  void WaveShaper::setCoeff(int n, smp_t v)
  {
  //assert(n<ncoeff,"error");
  coeff[n]=v;
  }
  * 
  * 
  Compressor::Compressor()
  {
  attack = 0;
  decay=0;
  ratio=0;
  threshold=0;
  over=false;
  under=false;
  off_i=0;
  on_i=0;
  index=0;
  c_ratio=1.0;
  }
  * 
  void Compressor::setAttack(int a)
  {
  attack = a;
  }
  * 
  void Compressor::setDecay(int d)
  {
  decay = d;
  }
  * 
  void Compressor::setRatio(smp_t r)
  {
  ratio = r;
  }
  * 
  void Compressor::setThreshold(smp_t t)
  {
  threshold=t;
  }
  * 
  void Compressor::setInput(Value** in)
  {
  input = in;
  }
  * 
  void Compressor::reset()
  {
  over=false;
  under=false;
  off_i=0;
  on_i=0;
  index=0;
  c_ratio=1.0;
  (*input)->reset();
  }
  * 
  smp_t Compressor::nextValue()
  {
  smp_t val = (*input)->nextValue();
  smp_t aval = fabs(val);
  * 
  if(!over && aval>=threshold)
  {
  on_i=index;
  over=true;
  under=false;
  a_slope = (c_ratio-ratio)/attack;
  }
  * 
  if(over && aval<threshold)
  {
  off_i=index;
  over=false;
  under=true;
  d_slope = (1.0-c_ratio)/(smp_t)decay;
  }
  * 
  if(under)
  {
  if(index-off_i<decay)
  {
  c_ratio += d_slope;
  }
  else 
  {
  c_ratio=1.0;
  under=false;
  }
  * 
  }
  * 
  if(over)
  {
  if(index-on_i<attack)c_ratio -= a_slope;
  else c_ratio = ratio;
  }
  * 
  index++;
  return (smp_t)(val*c_ratio);
  }*/

}