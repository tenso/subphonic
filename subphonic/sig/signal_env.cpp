#include "signal_env.h"

using namespace std;

namespace spl{

/********/
/*AHDSRin*/
/********/

AHDSRin::AHDSRin()
{
    retrig=true;
   
    gate=NULL;
    in=NULL;
    gate_level=GATE_ZERO_LVL;
   
    att=NULL;
    hold=NULL;
    dec=NULL;
    sus=NULL;
    rel=NULL;
   
    val=0;
    eval=0;
    rel_index=0;
   
    hard_cut=false;
   
    c_mult = 1.0;
   
    gateon=0;
}

void AHDSRin::setTimeMult(smp_t m)
{
    c_mult=m;
}


void AHDSRin::setAttack(Value** t)
{
    att=t;
}

void AHDSRin::setHold(Value** t)
{
    hold=t;
}

void AHDSRin::setDecay(Value** t)
{
    dec=t;
}
void AHDSRin::setSustain(Value** l)
{
    sus=l;
}
void AHDSRin::setRelease(Value** t)
{
    rel=t;
}

void AHDSRin::setInput(Value** val)
{
    in = val;
}

smp_t AHDSRin::nextValue()
{
    //might remove gate and still want output...
    smp_t gval=0;
    if(gate!=NULL)gval=(*gate)->nextValue();
   
    smp_t inval=1.0;
    if(in!=NULL)inval = (*in)->nextValue();
   
    int   at = 0;
    if(att!=NULL)at = (int)(c_mult*(*att)->nextValue());
   
    int   ht = 0;
    if(hold!=NULL)ht = (int)(c_mult*(*hold)->nextValue());
   
    int   dt = 0;
    if(dec!=NULL)dt = (int)(c_mult*(*dec)->nextValue());
      
    smp_t sl = 0;
    if(sus!=NULL)sl = (*sus)->nextValue();

    int   rt = 0;
    if(rel!=NULL)rt = (int)(c_mult*(*rel)->nextValue());
   
   
    gateon=gval;
   
    /*D   static int first_rel=0;
      static Uint32 t=0;*/
   
    //trigger on abs()
    if(gval > gate_level) //greater than so that 0 menas trig on anything greater than 0
    {
	
        /*D	if(!first_rel)
          {
          first_rel=1;
          }*/
	
        if(retrig)
        {
            //continue trig from this level
            if(rel_index!=0 && !hard_cut)
            {
                index=(int)(at*eval); //FIXME: PROBABLY roundoff errrors
            }
            else //hardcut(total reset)
            {
                index=0;
            }
	     
            retrig=false;
        }
	
    }
    else if(!retrig)
    {
        retrig=true;
	
        /* D if(first_rel==1)
           {
           t=SDL_GetTicks();
           first_rel=2;
           }*/
	
        rel_index=index;
        rel_level=eval;
    }
   
    smp_t sld=sl/(smp_t)SMP_MAX;
   
    smp_t aslope = 0;
    if(at!=0)aslope = 1.0/(smp_t)at;
   
    smp_t dslope = 0;
    if(dt!=0)dslope = (1.0-sld)/(smp_t)(dt);
   
    //problems with index flipping over?
   
    if(gateon>0) //attack, hold, decay, sustain
    {
        if(at==0)
        {
            if(dt!=0)
            {
                eval = 1.0 - dslope*ramp(index-ht) + dslope*ramp(index-ht-dt);
            }
            else
            {
                eval = 1.0 + (-1.0+sld)*step(index-ht);
            }
        }
        else
        {
            if(dt!=0)
            {
                eval = aslope*ramp(index) - aslope*ramp(index-at)
                    - dslope*ramp(index-at-ht) + dslope*ramp(index-at-ht-dt);
            }
            else
            {
                eval = aslope*ramp(index) - aslope*ramp(index-at) + (-1.0+sld)*step(index-at-ht);
            }
        }
	
	
        val = (smp_t)(inval*eval);
	
    }
    else if(rel_index>0) //release
    {
        smp_t rslope = 0;
        if(rt!=0)rslope = rel_level/(smp_t)rt;
	
        eval = rel_level-rslope*ramp(index-rel_index);
	
        //done with release
        if((signed int)index-rel_index>=rt)
        {
            eval=0;
            rel_index=0;
            rel_level=0;
        }
	
        val = (smp_t)(inval*eval);
	
	
    }
   
    index++;
   
    /*D   if(first_rel==2 && val==0)
      {
      Uint32 t2=SDL_GetTicks();
      first_rel=3;
      cout << (t2-t)/1000.0 << endl;
      first_rel=0;
      }*/
   
    return val;
}

const smp_t* AHDSRin::envValPtr()
{
    return &eval;
}

smp_t AHDSRin::envVal()
{
    return eval;
}


void AHDSRin::reset()
{
    eval=0;
    val=0;
    index=0;
    retrig=true;
    rel_index=0;
}

void AHDSRin::setGate(Value** gate)
{
    this->gate=gate;
}

//level=0 will never trigger
void AHDSRin::setGateLevel(smp_t lev)
{
    gate_level=lev;
}

const smp_t* AHDSRin::gateOn()
{
    return &gateon;
}

//will the envelope restart from 0(hardcut=true) or from where it is(hardcut=false, DEFAULT)
void AHDSRin::setHardCut(bool v)
{
    hard_cut=v;
}
bool AHDSRin::getHardCut()
{
    return hard_cut;
}





/**********/
/*AHDSRgen*/
/**********/

AHDSRgen::AHDSRgen()
{
    run=false;
    retrig=true;
   
    gate=NULL;
    in=NULL;
    gate_level=GATE_ZERO_LVL;
   
    att=NULL;
    hold=NULL;
    dec=NULL;
    sus_l=NULL;
    sus_t=NULL;
    rel=NULL;
   
    val=0;
    eval=0;
   
    gateon=0;
   
    hard_cut=false;
   
    c_mult=1.0;
}

void AHDSRgen::setTimeMult(smp_t m)
{
    c_mult=m;
}


void AHDSRgen::setAttack(Value** t)
{
    att=t;
}

void AHDSRgen::setHold(Value** t)
{
    hold=t;
}

void AHDSRgen::setDecay(Value** t)
{
    dec=t;
}

void AHDSRgen::setSustainTime(Value** n_smp)
{
    sus_t=n_smp;
}

void AHDSRgen::setSustain(Value** l)
{
    sus_l=l;
}
void AHDSRgen::setRelease(Value** t)
{
    rel=t;
}

void AHDSRgen::setInput(Value** val)
{
    in = val;
}

smp_t AHDSRgen::nextValue()
{
    //must have these
    if(att==NULL || dec==NULL || sus_t==NULL || rel==NULL || sus_l==NULL)return 0;
   
    //might remove gate and still want output...
    smp_t gval=0;
    if(gate!=NULL)gval=(*gate)->nextValue();
   
    smp_t inval=1.0;
    if(in!=NULL)inval = (*in)->nextValue();
   
    int   at = 0;
    if(att!=NULL)at = (int)(c_mult*(*att)->nextValue());
   
    int   ht = 0;
    if(hold!=NULL)ht = (int)(c_mult*(*hold)->nextValue());
   
    int   dt = 0;
    if(dec!=NULL)dt = (int)(c_mult*(*dec)->nextValue());
      
    smp_t sl = 0;
    if(sus_l!=NULL)sl = (*sus_l)->nextValue();
      
    int   st = 0;
    if(sus_t!=NULL)st = (int)(c_mult*(*sus_t)->nextValue());
   
    int   rt = 0;
    if(rel!=NULL)rt = (int)(c_mult*(*rel)->nextValue());

    gateon=gval;
   
    if(gval > gate_level)
    {
        if(retrig)
        {
	     
            //cout << "start" << endl;
            if(hard_cut)
            {
                run=true;
                index=0;
            }
            else
            {
                run=true;
                index=(int)(at*eval); //make current env level to new star pos for attack
            }
            retrig=false;
        }
    }
    else //if(!retrig)
    {
        //cout << "gate off" << endl;
        retrig=true;
    }
   
   
    smp_t sld=sl/(smp_t)SMP_MAX;
   
    smp_t aslope = 0;
    if(at!=0)aslope = 1.0/(smp_t)at;
   
    smp_t dslope = 0;
    if(dt!=0)dslope = (1.0-sld)/(smp_t)(dt);
   
    smp_t rslope = 0;
    if(rt!=0)rslope = sld/(smp_t)rt;
   
   
    if(run) //attack, hold, decay, sustain, realse
    {
        if(at==0)
        {
            if(dt!=0)
            {
                eval = 1.0 - dslope*ramp(index-ht) + dslope*ramp(index-ht-dt)
                    - rslope * ramp(index-ht-dt-st);
            }
            else
            {
                eval = 1.0 + (-1.0+sld)*step(index-ht)
                    - rslope * ramp(index-ht-st);
            }
        }
        else
        {
            if(dt!=0)
            {
                eval = aslope*ramp(index) - aslope*ramp(index-at)
                    - dslope*ramp(index-at-ht) + dslope*ramp(index-at-ht-dt)
                    - rslope * ramp(index-at-ht-dt-st);
            }
            else
            {
                eval = aslope*ramp(index) - aslope*ramp(index-at) + (-1.0+sld)*step(index-at-ht)
                    -rslope * ramp(index-at-ht-st);
            }
        }
			
        /*
          eval = aslope * ramp(index) - aslope*ramp(index-at)
          - dslope * ramp(index-at-ht)
          + dslope * ramp(index-at-ht-dt)
          - rslope * ramp(index-at-ht-dt-st);
        */
	
        val = (smp_t)(inval*eval);
	
        index++;
	
        if(index >= at+ht+dt+st+rt)
        {
            index=0;
            eval=0;
            run=false;
        }
	
    }
    else
    {
        eval=0;
        val=0;
    }
   
    return val;
}

const smp_t* AHDSRgen::envValPtr()
{
    return &eval;
}

smp_t AHDSRgen::envVal()
{
    return eval;
}


void AHDSRgen::reset()
{
    eval=0;
    val=0;
    index=0;
    retrig=true;
    run=false;

}

void AHDSRgen::setGate(Value** gate)
{
    this->gate=gate;
}

//level=0 will never trigger
void AHDSRgen::setGateLevel(smp_t lev)
{
    gate_level=lev;
}

const smp_t* AHDSRgen::gateOn()
{
    return &gateon;
}

//will the envelope restart from 0(hardcut=true) or from where it is(hardcut=false, DEFAULT)
void AHDSRgen::setHardCut(bool v)
{
    hard_cut=v;
}
bool AHDSRgen::getHardCut()
{
    return hard_cut;
}

const bool* AHDSRgen::inCyclePtr()
{
    return &run;
}


/**********/
/*ENVELOPE*/
/**********/

EnvFollow::EnvFollow(env_mode mode, smp_t up_per)
{
    this->up_per=(int)up_per;
    this->mode=mode;
    retval=0;
    peakval=0;
    rmsval=0;
    next=0;
}

void EnvFollow::setUpdatePeriod(smp_t n)
{
    up_per=(int)n;
}
smp_t EnvFollow::getUpdatePeriod()
{
    return (smp_t)up_per;
}

void EnvFollow::setMode(env_mode mode)
{
    this->mode=mode;
}


void EnvFollow::setInput(Value** in)
{
    this->in=in;
}

void EnvFollow::setPeriod(Value** in)
{
    this->per=in;
}

smp_t EnvFollow::nextValue()
{
    int per_v=up_per;
    if(per!=NULL)per_v = (int)(*per)->nextValue();
    if(per_v==0)
    {
        per_v=1;
    }
   
    next = 0;
    if(in!=NULL)next = (*in)->nextValue();
   
    if(mode==PEAK)
    {
        smp_t a = fabs(next);
        if(a>peakval)peakval=a;
        if(index % per_v ==0)
        {
            retval=peakval;
            peakval=0;
        }
    }
    else if(mode==RMS)
    {
        smp_t a = next*next; 
        rmsval+=a;
        if(index % per_v ==0)
        {
            retval=(smp_t)( sqrt(rmsval/(smp_t)per_v) );
            rmsval=0;
        }
    }
   
    index++;
    return retval;
}

const smp_t* EnvFollow::getEnvelopeVal()
{
    return &retval;
}

const smp_t* EnvFollow::getInputVal()
{
    return &next;
}


/****************/
/*CPINTERPOLATOR*/
/****************/

CPInterpolator::CPInterpolator()
{
    w=0;
   
    mode=LINEAR;
    in=NULL;
    period=NULL;
    cs=0;
    ce=1;
    data_i=0;
    data_i_val=data_i;
    index=0;
   
    gval=0;
    gate=NULL;
    sustain_v=0;
    gate_finish_cycle=true;
   
    gate_th=0;
   
    use_gate=false;
    incycle=ENDLESS;
    first_sust=true;
   
    retrig=true;
   
    hold_end_val=false;
    have_end_val=false;
}

void CPInterpolator::setGate(Value** g)
{
    gate=g;
}

smp_t* CPInterpolator::gateValPtr()
{
    return &gval;
}


bool CPInterpolator::getGateTrigOn()
{
    return use_gate;
}

void CPInterpolator::setGateTrigOn(bool v)
{
    use_gate=v;
   
    if(!use_gate)incycle=ENDLESS;
    else
    {
        if(index==0)incycle=SLEEP; //otherwise it must run complete
    }
   
}


void CPInterpolator::setSustainIndex(int val)
{
    sustain_v=val;
}


void CPInterpolator::setMode(MODE m)
{
    mode=m;
}

void CPInterpolator::setPeriod(Value** p)
{
    period=p;
}

void CPInterpolator::setData(const vector<SigCoord>& data)
{
    DASSERT(data.front().x==0);
   
    this->data=data;
    w=(int)data.back().x;
   
    //find correct cs,ce 
    while(data_i<(int)data[cs].x)
    {
        if(cs==0)
        {
            ERROR("something wrong");
            cs=0;
            ce=1;
            break;
        }
	
        cs--;
        ce--;
	

    }
}

void CPInterpolator::setInput(Value** v)
{
    in=v;
}

void CPInterpolator::setGateFinishCycle(bool val)
{
    gate_finish_cycle=val;
}

bool CPInterpolator::getGateFinishCycle()
{
    return gate_finish_cycle;
}

void CPInterpolator::setHoldEndValue(bool val)
{
    hold_end_val=val;
    have_end_val=false;
}

bool CPInterpolator::getHoldEndValue()
{
    return hold_end_val;
}

smp_t CPInterpolator::nextValue()
{
    bool reset_index=false;
   
    if(data.size()==0)
    {
        ERROR("data null");
        return 0;
    }
   
    int per=0;
    if(period != NULL)per = abs((int)((*period)->nextValue()));
   
    smp_t inval=1.0;
    if(in != NULL)inval = (*in)->nextValue();
   
   
    if(use_gate) //use gate
    {
        if(gate!=NULL)
        {
            gval = (*gate)->nextValue();
	     
            if(gval>gate_th) //gate on
            {
                if(retrig)
                {
                    if(gate_finish_cycle)
                    {
                        //only time retrig is permitted here is in sleep
                        if(incycle==SLEEP)
                        {
                            incycle=GATE_ON;
                            reset_index=true;
                            first_sust=true;
                        }
                    }
                    else
                    {
                        //retrig anywhere
                        incycle=GATE_ON;
                        reset_index=true;
                        first_sust=true;
                    }
		       
                    retrig=false;
                }
            }
            else //gate off
            {
                retrig=true;
		  
                //dont disturb sleeping
                if(incycle != SLEEP)incycle=GATE_OFF;
		  
                first_sust=false;
            }
        }
        else //use_gate BUT gate NULL =>
        {
            if(incycle!=SLEEP)incycle=GATE_OFF;
            retrig=true;
        }
    }
    else //dont use gate
    {
        retrig=true;
        //incycle=ENDLESS;
    }
   
    if(index>per || reset_index)
    {
        index=0;

        if(!reset_index) //cycle done
        {
            if(use_gate)
            {
                incycle=SLEEP;
                have_end_val=true;
            }
        }
        else 
        {
            reset_index=false;
        }
	
        cs=0;
        ce=1;
        if(per!=0)index %= per; //do this here is period just changed since last time, as not to overshoot
    }
   
    if(hold_end_val && incycle==SLEEP && have_end_val)
    {
        return val;
    }
   
    //FIXME: changeing period makes data_i jump back, but without cs,ce
    if(per==0)data_i=0;
    else data_i = (int)(w*(index/(float)per));
   
    if(data_i>(int)w)
    {
        DERROR("index bound");
        data_i=w;
    }
    if(data_i<0)
    {
        DERROR("index bound");
        data_i=0;
    }
   
    if(data_i>(int)data[ce].x)
    {
        //next pair
        cs++;
        ce++;
	
        //data done(should never happen
        if(ce>data.size())
        {
            DERROR("happened");
            index=0;
        }
    }
   
    if(first_sust && incycle==GATE_ON && data_i>=sustain_v)
    {
        first_sust=false; //just one sustain per cycle(only applicable in fisish cycle mode)
        incycle=SUSTAIN;
    }
   
    //linear interpolated 'depth' between cp's
    float dx = data[ce].x-data[cs].x;
    float dy = data[ce].y-data[cs].y;
   
    int x = data_i-(int)data[cs].x;
    float delta = x/dx;
    float d = (data[cs].y+dy*delta)/SMP_MAX;
   
    val = (smp_t)(inval*d);
   
    //keep index fixed in SUSTAIN(at sutain_v) and SLEEP(at 0)
    if(incycle!=SUSTAIN && incycle!=SLEEP)
    {
        index++;
    }
      
    data_i_val=data_i; // this is to be able to get data index in a smp_t
   
    return val;
}

smp_t* CPInterpolator::getDataIndexPtr()
{
    return &data_i_val;
}

}