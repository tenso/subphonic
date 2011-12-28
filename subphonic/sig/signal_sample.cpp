#include "signal_sample.h"

/*************/
/*SAMPLEVALUE*/
/*************/

namespace spl{

SampleValue::SampleValue(Sample* s)
{
    smp = s;
   
    speed=1.0;
    ind=0.0;
   
    pos_in=NULL;
    trig_in=NULL;
    speed_in=NULL;
    stop_in=NULL;
   
    setInterval(0,smp->getLen()-1);
   
    incycle = S_IDLE;
    mode = LINEAR;
   
    tr=0;
   
    g_lev=GATE_ZERO_LVL;
   
    re_speed=1.0; //resample speed
   
    loop=true;
   
    trig_backwards=false;
    retrig=true;
   
    val=0;
   
    soft_gate_run=false;
    soft_gate_index=0;
    soft_gate_inter=0;
    soft_val=0;
    setGateInterpolateStep(0.0005);
    setGateRetrigMode(HARD_GATE);
   
    inspeed=speed*re_speed;;
    isplaying=false;
    man_trig=false;
   
    start_i_in=NULL;
    stop_i_in=NULL;
}

SampleValue::~SampleValue()
{
    delete smp;
}

void SampleValue::setStartIndex(Value** in)
{
    start_i_in=in;
}

void SampleValue::setStopIndex(Value** in)
{
    stop_i_in=in;
}


void SampleValue::setInterval(int start, int stop)
{
    int t;
    if(start>stop)
    {
        t=start;
        start=stop;
        stop=t;
    }
   
    d_start_i=start;
    d_stop_i=stop;
   
    if(d_start_i<0)d_start_i=0;
    if(d_start_i>=getLength())d_start_i=getLength()-1;
   
    if(d_stop_i<0)d_stop_i=0;
    if(d_stop_i>=getLength())d_stop_i=getLength()-1;
   
    //p_len = d_stop_i-d_start_i;
}

void SampleValue::setLoop(bool v)
{
    loop=v;
}

bool SampleValue::getLoop()
{
    return loop;
}


smp_t SampleValue::nextValue()
{
    start_i=d_start_i;
    stop_i=d_stop_i;
   
    if(start_i_in!=NULL)
    {
        start_i=(int)(*start_i_in)->nextValue();
	
        if(start_i<0)start_i=0;
        if(start_i>=getLength())start_i=getLength()-1;
    }
    if(stop_i_in!=NULL)
    {
        stop_i=(int)(*stop_i_in)->nextValue();
	
        if(stop_i<0)stop_i=0;
        if(stop_i>=getLength())stop_i=getLength()-1;
    }
    if(start_i>stop_i)swap(&start_i, &stop_i);

    p_len = stop_i-start_i;
   
   
    //do theses first so that they are always drained
    //TRIG
    tr=0;
    if(trig_in!=NULL)
    {
        tr = (*trig_in)->nextValue();
    }
   
    smp_t stop=0;
    if(stop_in!=NULL)
    {
        stop=(*stop_in)->nextValue();
    }
   
    if(stop>g_lev)
    {
        incycle=S_IDLE;
        isplaying=false;
    }
   
   
    //SPEED
    inspeed=speed*re_speed;
    if(speed_in!=NULL)
    {
        inspeed *= (*speed_in)->nextValue();
    }
   
    //POS IN overrides everything else
    if(pos_in!=NULL)
    {
        smp_t v = ((*pos_in)->nextValue());
        if(v<0)v=0;
        if(v>=smp->getLen())v=smp->getLen()-1;
	
        int tindex = (uint)(v);
        smp_t findex = v-tindex;
	
        if(mode==NONE)
        {
            return (*smp)[tindex]*smp->maxValueInv();
        }
        else if(mode==LINEAR)
        {
            val=0;
	     
            if(tindex+1 > stop_i) //frac index out of bounds; what to do:
            {
                //wrap around and intepolate with start
                val = (*smp)[tindex]*(1.0-findex)+(*smp)[0]*findex;
            }
            else val = (*smp)[tindex]*(1.0-findex)+(*smp)[tindex+1]*findex;
	     
            //normalize and return
            val *= smp->maxValueInv();
        }
        return val;
    }
   
   
    if(tr>g_lev)
    {
        man_trig=false;
	
        if(retrig)
        {
            if(incycle==S_RUN && gate_mode==SOFT_GATE)
            {
                soft_gate_run=true;
                soft_gate_index=ind;
                soft_gate_inter=0;
                soft_val=0;
            }
	     
            ind=start_i;
	     
            if(inspeed<0)
            {
                trig_backwards=true;
            }
            else 
            {
                trig_backwards=false;
            }
	     
            incycle=S_RUN;
            isplaying=true;
	     
            retrig=false;
        }//end retig
        else
        {
            //trig_backwards=false;
        }
    } //end tr>g_lev
    else 
    {
        retrig=true;
        //trig_backwards=false;
    }
   
    if(incycle == S_IDLE)return 0;
   
   
    if(soft_gate_run)
    {
        readSample(&soft_val, soft_gate_index);
	
        stepIndex(&soft_gate_index, inspeed, false);
	
	
        soft_gate_inter+=soft_gate_inter_step;
        if(soft_gate_inter>1.0)
        {
            soft_gate_inter=1.0;
            soft_gate_run=false;
        }
    }
   
    readSample(&val, ind);
   
    if(soft_gate_run)
    {
        val = val*soft_gate_inter + soft_val*(1.0-soft_gate_inter);
    }
   
    stepIndex(&ind, inspeed, true/*change state, pri index*/);
   
    return val;
}



void SampleValue::setGateRetrigMode(GATE_MODE mode)
{
    gate_mode=mode;
}

void SampleValue::setGateInterpolateStep(smp_t step)
{
    soft_gate_inter_step = step;
    if(soft_gate_inter_step<0.000000001)
    {
        ERROR("soft_gate_inter_step to small");
        soft_gate_inter_step=0.000000001;
    }
   
}


void SampleValue::readSample(smp_t* r_val, smp_t r_index)
{
    int tindex=(int)r_index;
    smp_t findex = r_index-tindex;
   
    //WARNING: Sample data better be signed 16bit int!
    if(mode==NONE)
    {
        *r_val = (*smp)[tindex];
    }
    else if(mode==LINEAR)
    {
        if(tindex == stop_i) //frac index out of bounds; 
        {
            if(incycle==S_IDLE)
            {
                //stopping, just get last
                *r_val = (*smp)[tindex];
            }
            else
            {
                //wrap around and intepolate with start
                *r_val = (*smp)[tindex]*(1.0-findex)+(*smp)[start_i]*findex;
            }
        }
        else if(tindex == start_i && findex<0) 
        {
            if(incycle==S_IDLE)
            {
                //stopping, just get last
                *r_val = (*smp)[tindex];
            }
            else
            {
                findex=fabs(findex);
                //wrap around and intepolate with end
                *r_val = (*smp)[tindex]*(1.0-findex)+(*smp)[stop_i]*findex;
            }
        }
        else *r_val = (*smp)[tindex]*(1.0-findex)+(*smp)[tindex+1]*findex;
    }
   
    //normalize
    *r_val*=smp->maxValueInv();
}



void SampleValue::stepIndex(smp_t* r_index, smp_t speed, bool change_state)
{
    //HANDLE INDEX
    *r_index+=speed;
    int tindex=(int)(*r_index);
    smp_t findex = *r_index-tindex;
   
    if(p_len==0)
    {
        *r_index=start_i;
    }
    else
    {
        if(tindex < start_i) //only possibility speed<0
        {
            if(trig_backwards)
            {
                int i_off = (tindex-start_i+1)%p_len; //+1 beacuse: index==-1 --> stop_i
                *r_index = stop_i + i_off + findex;
		  
                tindex=(int)(*r_index);
                DASSERT(tindex >= start_i && tindex<=stop_i);
		  
                trig_backwards=false;
            }
            else if(!loop)
            {
                if(change_state)
                {
                    incycle=S_IDLE;
                    isplaying=false;
                }
		  
                *r_index=start_i;
            }
            else
            {
                if(!man_trig && tr<=g_lev)
                {
                    //loop is on BUT trigger if off: dont play;
		       
                    if(change_state)
                    {
                        incycle=S_IDLE;
                        isplaying=false;
                    }
		       
                    *r_index=start_i;
                }
                else
                {
                    int i_off = (tindex-start_i+1)%p_len; //+1 beacuse: index==-1 --> stop_i
                    *r_index = stop_i + i_off + findex;
		       
                    tindex=(int)*r_index;
                    DASSERT(tindex >= start_i && tindex<=stop_i);
                }
            }
        }
        else if(tindex > stop_i)
        {
            if(!loop)
            {
                if(change_state)
                {
                    incycle=S_IDLE;
                    isplaying=false;
                }
		  
                *r_index=start_i;
            }
            else
            {
                if(!man_trig && tr<=g_lev)
                {
                    //loop is on BUT trigger if off: dont play;
                    //but if man_trig: loop
                    if(change_state)
                    {
                        incycle=S_IDLE;
                        isplaying=false;
                    }
		       
                    *r_index=start_i;
                }
                else
                {
                    int i_off = (tindex-stop_i-1)%p_len;
                    *r_index = start_i + i_off + findex;
		       
                    tindex=(int)*r_index;
                    DASSERT(tindex >= start_i && tindex<=stop_i);
                }
            }
        }
    }
}



void SampleValue::setInterpolationMode(INTERPOLATION_MODE m)
{
    mode=m;
}

void SampleValue::setTrigger(Value** t)
{
    trig_in=t;
}

void SampleValue::setStopIn(Value** in)
{
    stop_in=in;
}

void SampleValue::setPosIn(Value** in)
{
    pos_in=in;
}


void SampleValue::setSpeedIn(Value** in)
{
    speed_in=in;
}

void SampleValue::setSpeed(smp_t s)
{
    speed=s;
}

void SampleValue::setResampleRate(int from, int to)
{
    re_speed = (smp_t)from/(smp_t)to;
}


const smp_t* SampleValue::onTrigger()
{
    return &tr;
}

Sample* SampleValue::getSample()
{
    return smp;
}

uint SampleValue::getLength() const
{
    return smp->getLen();
}

void SampleValue::play()
{
    if(incycle==S_IDLE)
    {
        if(inspeed<0) //WARNING: uses "old"(max one sample) inspeed
        {
            trig_backwards=true;
        }
    }
   
    incycle=S_RUN;
    isplaying=true;
   
    man_trig=true;
}

void SampleValue::pause()
{
    incycle=S_IDLE;
    isplaying=false;
}

void SampleValue::reset(int i)
{
    incycle=S_IDLE;
    isplaying=false;
    retrig=true;
    ind=start_i;
    man_trig=false;
    trig_backwards=false;
    soft_gate_run=false;
}

void SampleValue::setTrigLevel(smp_t l)
{
    g_lev=l;
}

smp_t SampleValue::getTrigLevel()
{
    return g_lev;
}

bool SampleValue::playing()
{
    return incycle!=S_IDLE;
}

const bool* SampleValue::getIsPlayingPtr()
{
    return &isplaying;
}

}