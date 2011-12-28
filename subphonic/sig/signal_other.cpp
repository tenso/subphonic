#include "signal_other.h"

namespace spl{

GateExecCmd::GateExecCmd()
{
    in=NULL;
    cmd_q=NULL;
    retrig=true;
}


smp_t GateExecCmd::nextValue()
{
    smp_t inval=0;
    if(in!=NULL)inval = (*in)->nextValue();
   
    if(inval>GATE_ZERO_LVL)
    {
        if(retrig)
        {
            retrig=false;
            if(cmd_q->size() < maxsize)
                cmd_q->push(cmd);
            else
            {
                ERROR("cmd queue has reached max size");
            }
        }
    }
    else
    {
        retrig=true;
    }
    return 0.0;
}

void GateExecCmd::setInput(Value** v)
{
    in=v;
}


void GateExecCmd::setCmdStr(const string& cmd)
{
    this->cmd=cmd;
}

void GateExecCmd::setCmdQueue(std::queue<string>* q, uint maxsize)
{
    cmd_q=q;
    this->maxsize=maxsize;
}


/************/
/*VALUESCOPE*/
/************/

ValueScope::ValueScope(int len)
{
    l =len;
    buffer = new RingBuffer<smp_t>(len);
    boost = 1.0f;
}

void ValueScope::addInput(Value** in)
{
    this->in=in;
}

void ValueScope::setBoost(float val)
{
    boost = val;
}

smp_t ValueScope::nextValue()
{
    smp_t val = (*in)->nextValue();
    smp_t bval = (smp_t)(val*boost);
    if(buffer->inBuffer()<l-1)buffer->add(bval);
    else
    {
        buffer->get();
        buffer->add(bval);
    }
   
    return val;
}

RingBuffer<smp_t>* ValueScope::getBuffer()
{
    return buffer;
}

void ValueScope::clear()
{
    while(buffer->inBuffer()>0)buffer->get();
}



/***********/
/*PEAKMETER*/
/***********/

PeakMeter::PeakMeter()
{
    in=NULL;
   
    peak=0;
    hold=11025;
    falloff=0.5/11025.0;
    doabs=true;
    holdtick=0;
}


void PeakMeter::setPeakHold(uint samples)
{
    hold=samples;
}

void PeakMeter::setFalloffRate(smp_t persample)
{
    falloff=persample;
}


const smp_t* PeakMeter::peakPtr()
{
    return &peak;
}


void PeakMeter::setInput(Value** in)
{
    this->in=in;
}

smp_t PeakMeter::nextValue()
{
    smp_t inval=0;
    if(in!=NULL)inval = (*in)->nextValue();
    smp_t retval=inval;
   
    if(doabs)inval=fabs(inval);
   
    if(inval>peak)
    {
        holdtick=hold;
	
        peak=inval;
        if(peak>1.0)peak=1.0;
    }
   
    if(holdtick>0)
    {
        holdtick--;
        //no falloff
    }
    else
    {
        if(peak>0)
        {
            peak-=falloff;
            if(peak<0)peak=0;
        }
    }
   
   
    return retval;
}


void PeakMeter::reset(int index)
{
    peak=0;
    holdtick=0;   
}

/*************/
/*OstreamEcho*/
/*************/

OstreamEcho::OstreamEcho(ostream& os, const string& postfix) : out(os)
{
    post.assign(postfix);
    in=NULL;
}

void OstreamEcho::setInput(Value** in)
{
    this->in=in;
}

smp_t OstreamEcho::nextValue()
{
    if(in==NULL)return 0;
   
    smp_t val = (*in)->nextValue();
    out << (smp_t)val << post;
    return val;
}

/************/
/*PRINTVALUE*/
/************/

PrintValue::PrintValue(ostream& stream) : os(stream)
{
    input=NULL;
    setPeriod(44100);
}

void PrintValue::setInput(Value** in)
{
    this->input=in;
}

void PrintValue::setPeriod(uint p)
{
    this->p=p;
}

smp_t PrintValue::nextValue()
{
    smp_t in=0;
    if(input!=NULL)in = (*input)->nextValue();
   
    if(index%p==0)
    {
        os << in << endl;
    }
   
    index++;
   
    return in;
}

/*******/
/*ONOFF*/
/*******/

OnOff::OnOff()
{
    onoff=NULL;
    in=NULL;
}

   

void OnOff::setOnOff(Value** v)
{
    onoff=v;
}

void OnOff::setInput(Value** v)
{
    in=v;
}

   
smp_t OnOff::nextValue()
{
    smp_t oo = 0;
    if(onoff!=NULL)oo = (*onoff)->nextValue();
   
    if(oo>=0.5 && in!=NULL)
    {
        return (*in)->nextValue();
    }
    return 0.0;
}

}