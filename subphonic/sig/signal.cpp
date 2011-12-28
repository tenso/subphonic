#include "signal.h"

using namespace std;

namespace spl{

/*******/
/*Value*/
/*******/

Value::Value()
{
    index=0;
}


Value::~Value()
{
}

void Value::make(Sample* smp, smp_t scale/*, bool clip*/)
{
    DASSERT(smp!=NULL);
   
    make(smp->data, smp->len, scale/*, clip*/);
}

void Value::reset(int index)
{
    this->index=index;
}


/*************/
/*LinearSweep*/
/*************/

LinearSweep::LinearSweep(smp_t start, smp_t end, uint n_smp)
{
    setLoop(false);
    setHold(END);
    setRange(start,end,n_smp);
   
    val=0;
}


void LinearSweep::setRange(smp_t start, smp_t end, uint n_smp)
{
    DASSERT(n_smp>0);
    if(n_smp==0)n_smp=1;
   
    this->start=start;
    this->end=end;
    this->n_smp=n_smp;
   
    if(n_smp>1)
        val_inc = (end-start)/(n_smp-1);
}


void LinearSweep::setLoop(bool val)
{
    loop=val;
}

bool LinearSweep::getLoop()
{
    return loop;
}


void LinearSweep::setHold(HOLD h)
{
    hold=h;
}

LinearSweep::HOLD LinearSweep::getHold()
{
    return hold;
}


smp_t LinearSweep::nextValue()
{
    if(index<0)
    {
        DERROR("index range");
        return 0;
    }
   
    if(n_smp==1)
    {
        if(hold==END)return end;
        if(hold==START)return start;
        return 0;
    }
   
    if((unsigned)index>=n_smp-1)
    {
        if(loop)
        {
            val=end;
            index=0;
            return val;
        }
	
        else
        {
            if(hold==END)return end;
            if(hold==START)return start;
            return 0;
        }
    }
   
    val=start+val_inc*index;
   
    index++;
   
    return val;
}


/**************/
/*VALUEDOPPLER*/
/**************/

ValueDoppler::ValueDoppler(uint num_slaves, Value** input)
{
    DASSERT(num_slaves>0);
    if(num_slaves<=0)num_slaves=1;
   
    num=num_slaves;
   
    real=input;
   
    slaves = new DopplerSlave*[num];
   
    for(uint i=0;i<num;i++)
    {
        slaves[i] = new DopplerSlave(this);
    }
   
    val=0;
    first=true;
}

ValueDoppler::~ValueDoppler()
{
    for(uint i=0;i<num;i++)
    {
        delete slaves[i];
    }
   
    delete[] slaves;
}

void ValueDoppler::setInput(Value** r)
{
    real=r;
}

void ValueDoppler::getValue()
{
    /*first=false MUST be set before nextValue() otherwise it is possible to get endless recursion.
     *EX: feedback with mixer:
     * 
     *1)something drains doppler for value
     *2)doppler drains mixer
     *3)mixer in turn wants value from doppler(feedback) but because bool isnot flipped the first=true
     *  clause in nextValue enters and it all begins again...
     *
     *THIS way it will halt second turn around and just return val, first=false clause in nextValue
     * */
   
    //here
    first=false; 
   
    //and here
    for(uint i=0;i<num;i++)
    {
        slaves[i]->first=false;
    }
   
    //now its safe to get value(any recursive attempts to drain will just return 'val')
   
    //val = 0; is NOT good here: slave might drain in feedback gets to master which have val=0 after this i.e
    //master will always have 0 in FB if it is the fb so to say...this way it is x[n-1] on master(not ideal but only way?)
    if(real==NULL)val=0;
    else val = (*real)->nextValue();
   
    static bool warned=false;
    if(!isfinite(val))
    {
        if(!warned)DERROR("output is not finite! ( one warning only )");
        warned=true;
        val=0;
    }
   
    for(uint i=0;i<num;i++)
    {
        slaves[i]->setValue(val);
    }
   
}

smp_t ValueDoppler::nextValue()
{
    if(first)
    {
        getValue();
    }
   
    first=true;
   
    return val;
}

Value** ValueDoppler::getSlave(uint num)
{
    DASSERT(num < this->num);
   
    return (Value**)&slaves[num];
}


ValueDoppler::DopplerSlave::DopplerSlave(ValueDoppler* master)
{
    this->master=master;
   
    first=true;
   
    val=0;
}

void ValueDoppler::DopplerSlave::setValue(smp_t v)
{
    val=v;
}

smp_t ValueDoppler::DopplerSlave::nextValue()
{
    if(first)
    {
        master->getValue(); //also sets this->val
    }
   
    first=true;
   
    return val;
}



/**************/
/*VALUEFORWARD*/
/**************/

ValueForward::ValueForward()
{
    forward=NULL;
}

smp_t ValueForward::nextValue()
{
    DASSERT(forward!=NULL);
   
    return (*forward)->nextValue();
}

void ValueForward::setInput(Value** in)
{
    forward=in;
}

Value** ValueForward::getInput()
{
    return forward;
}



/*******/
/*CONST*/
/*******/

Const::Const(smp_t v)
{
    val = v;
}

smp_t Const::nextValue()
{
    return val;
}

void Const::setValue(smp_t v)
{
    val=v;
}
smp_t Const::getValue()
{
    return val;
}



/**********/
/*SMPPOINT*/
/**********/

SmpPoint::SmpPoint(const smp_t* v)
{
    val = v;
}

smp_t SmpPoint::nextValue()
{
    if(val==NULL)return 0;
   
    return *val;
}

void SmpPoint::setSmp(const smp_t* v)
{
    val=v;
}

/*
  void SmpPoint::set(smp_t v)
  {
  *val=v;
  }*/




/**********/
/*Converts*/
/**********/

ConvertHz2Per::ConvertHz2Per(const Convert& conv, Value** v) : c(conv)
{
    setInput(v);
}

void ConvertHz2Per::setInput(Value** v)
{
    val = v;
}

smp_t ConvertHz2Per::nextValue()
{
    if(val==NULL)return 0;
   
    return c.hzToPeriod((*val)->nextValue());
}

/********/

ConvertSec2Len::ConvertSec2Len(const Convert& conv, Value** v) : c(conv)
{
    setInput(v);
}

void ConvertSec2Len::setInput(Value** v)
{
    val = v;
}

smp_t ConvertSec2Len::nextValue()
{
    if(val==NULL)return 0;
   
    return (smp_t)c.secToLen((*val)->nextValue());
}

/********/

ConvertUnifFreq2Per::ConvertUnifFreq2Per(const Convert& conv) : c(conv)
{
}

void ConvertUnifFreq2Per::setInput(Value** v)
{
    val = v;
}

smp_t ConvertUnifFreq2Per::nextValue()
{
    if(val==NULL)return 0;
   
    return c.hzToPeriod((*val)->nextValue()*c.halfSampleRate());
}




/********/
/*Stereo*/
/********/

Stereo::Stereo(int chans)
{
    n=chans;
    curr=0;
    mix=false;
   
    this->chans[0]=NULL;
    this->chans[1]=NULL;
}

void Stereo::setNChan(int chans)
{
    n=chans;
}


smp_t Stereo::nextValue()
{
    smp_t ch0=0;
    smp_t ch1=0;
   
    if(mix)
    {
        if(curr==0)
        {
            //only read every second sample
            if(chans[0] != NULL)ch0 = (*chans[0])->nextValue();
            ret = ch0;
        }
    }
    else
    {
        if(curr==0)
        {
            if(chans[0] != NULL)ch0 = (*chans[0])->nextValue();
            ret = ch0;
        }
        else 
        {
            if(chans[1] != NULL)ch1 = (*chans[1])->nextValue();
            ret = ch1;
        }
    }
   
    curr++;
    curr%=n;
   
    return ret;
}

void Stereo::addInput(Value** in, int chan)
{
    chans[chan]=in;
}

void Stereo::distFirstCh(bool v)
{
    mix=v;
}



/******/
/*Gate*/
/******/

Gate::Gate(smp_t level)
{
    lvl=level;
    setOn(false);
    retrig_c=0;
   
    index=0;
    setOneShot(false);
    setShotLen(1);
}


void Gate::setOnLevel(smp_t lvl)
{
    this->lvl=lvl;
}

void Gate::setOn(bool v)
{
    retrig_c=0;
   
    on=v;
   
    if(on)gate = lvl;
    else gate = 0;
   
    if(on && shot_mode)
    {
        if(shot_len==0) //off directly
        {
            gate=0;
            on=false;
            index=-1;
        }
        else index=shot_len;
    }
   
}

bool Gate::getOn()
{
    return on;
}


//set gate to off but retrig in 'smpls' 
void Gate::offRetrigIn(unsigned int smpls)
{
    if(smpls==0)
    {
        setOn(true);
        return;
    }
   
    setOn(false);
    retrig_c=smpls;
}

smp_t Gate::nextValue()
{
    if(retrig_c)
    {
        DASSERT(!on);
	
        retrig_c--;
        if(retrig_c==0)setOn(true);
        return 0;
    }
   
    if(shot_mode)
    {
        if(index>=0)
        {
            if(index==0)setOn(false);
            index--;
        }
    }
   
    return gate;
   
}

const smp_t* Gate::gatePtr()
{
    return &gate;
}

void Gate::setOneShot(bool on)
{
    if(on==shot_mode)return;
   
    if(!shot_mode)
    {
        index=shot_len;
    }
    else index=0;
   
    shot_mode=on;
   
}

bool Gate::getOneShot()
{
    return shot_mode;
}


void Gate::setShotLen(uint len)
{
    shot_len=len;
}





/*********/
/*ONESHOT*/
/*********/

OneShot::OneShot(smp_t lvl)
{
    setShotLen(1);
    index=0;
    ison=false;
    setAlwaysOn(false);
    this->lvl=lvl;
}

smp_t OneShot::nextValue()
{
    if(always)
    {
        if(ison)
        {
            return lvl;
        }
	
        return 0;
    }
   
    if(index>0)
    {
        ison=false;
        index--;
        return lvl;
    }
    return 0;
}

void OneShot::setLevel(smp_t l)
{
    lvl=l;
}

void OneShot::set(smp_t lvl)
{
    this->lvl=lvl;
    ison=true;
    index=ticks;
}


void OneShot::set()
{
    ison=true;
    index=ticks;
}

void OneShot::unset()
{
    ison=false;
    index=0;
}

void OneShot::setShotLen(uint len)
{
    ticks=len;
}

void OneShot::setAlwaysOn(bool v)
{
    always=v;
   
    if(always)
    {
        if(index!=0)
        {
            index=0;
            ison=true;
        }
    }
}


/*******/
/*CLOCK*/
/*******/

Linear::Linear(smp_t d)
{
    reset=NULL;
    retrig=true;
    run=NULL;
    tick=0;
   
    setD(d);
    setRetrigMode(NORMAL);
}

void Linear::setD(smp_t d)
{
    if(d==0)
    {
        ERROR("d==0, reseting to 1");
        d=1.0;
    }
   
    this->d=d;
}


void Linear::setResetInput(Value** in)
{
    this->reset=in;
}

void Linear::setRunInput(Value** in)
{
    this->run=in;
}


smp_t Linear::nextValue()
{
    smp_t rval=0;
    if(reset != NULL)rval = (*reset)->nextValue();
   
    smp_t runval=0;
    if(run != NULL)runval = (*run)->nextValue();
   
   
    if(m==NORMAL)
    {
	
        if(retrig && rval>GATE_ZERO_LVL)
        {
            tick = 0;
            retrig=false;
        }
        else
        {
            retrig=true;
            if(runval>GATE_ZERO_LVL)tick++;
        }
    }
    else
    {
        if(rval>GATE_ZERO_LVL)
        {
            tick = 0;
        }
        else
        {
            if(runval>GATE_ZERO_LVL)tick++;
        }
    }
   
    return tick/d;
}

void Linear::setRetrigMode(MODE m)
{
    this->m=m;
}

}