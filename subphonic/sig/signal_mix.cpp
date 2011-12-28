#include "signal_mix.h"

namespace spl{

using namespace std;

/*******/
/*Mixer*/
/*******/

MixerList::MixerList()
{
    depth=1.0;
   
    max=0;
    min=0;
   
    clamp_min=false;
    clamp_max=false;
}

void MixerList::addInput(Value** sig)
{
    inputs.push_back(sig);
}

void MixerList::remInput(Value** sig)
{
    val_list_it it = find(inputs.begin(),inputs.end(), sig);
   
    if(it==inputs.end())
    {
        DERROR("Value** not found");
        return;
    }
   
    inputs.erase(it);
}

void MixerList::remAllInput()
{
    inputs.clear();
}

void MixerList::setGain(smp_t d)
{
    depth = d;
}

smp_t MixerList::nextValue()
{
    smp_t val = 0;
   
    for(val_list_it it = inputs.begin();it!=inputs.end();it++)
    {
        Value** sig = *it;
        DASSERT(sig!=NULL);
	
        val += (*sig)->nextValue();
    }
   
    val*=depth;
   
    if(clamp_max)
    {
        if(val>max)val=max;
    }
    if(clamp_min)
    {
        if(val<min)val=min;
    }
   
    static bool warned=false;
    if(!isfinite(val))
    {
        if(!warned)DERROR("output is not finite! ( one warning only )");
        warned=true;
        val=0;
    }
   
    return val;
}

void MixerList::setClamp(smp_t min, smp_t max)
{
    clamp_max=true;
    clamp_min=true;
   
    this->min=min;
    this->max=max;
}

smp_t MixerList::getMin()
{
    return min;
}

smp_t MixerList::getMax()
{
    return max;
}

void MixerList::disableClamp()
{
    clamp_max=false;
    clamp_min=false;
}


void MixerList::setMin(smp_t v)
{
    clamp_min=true;
    min=v;
}

void MixerList::setMax(smp_t v)
{
    clamp_max=true;
    max=v;
}

uint MixerList::getNumIn()
{
    return inputs.size();
}


/************/
/*MIXERARRAY*/
/************/

MixerArray::MixerArray(int n)
{
    num=n;
    inputs = new Value**[num];
    remAllInput();
}

void MixerArray::addInput(Value** sig, int i)
{
    //assert i<num
    inputs[i] = sig;
}

void MixerArray::remInput(int i)
{
    inputs[i]=NULL;
}

void MixerArray::remAllInput()
{
    for(int i=0;i<num;i++)remInput(i);
}

smp_t MixerArray::nextValue()
{
    smp_t val = 0;
   
    for(int i=0;i<num;i++)
    {
        if(inputs[i]==NULL)continue;
        val += (*inputs[i])->nextValue();
    }
   
    static bool warned=false;
    if(!isfinite(val))
    {
        if(!warned)DERROR("output is not finite! ( one warning only )");
        warned=true;
        val=0;
    }
   
    return val;
}



/*************/
/*GROUNDMIXER*/
/*************/

GroundMixer::GroundMixer()
{
    stereo=false;
    flip=true;
}


smp_t GroundMixer::nextValue()
{
    if(stereo)
    {
	
        if(flip)
        {
            for(val_list_it it = inputs.begin();it!=inputs.end();it++)
            {
                Value** sig = *it;
                (*sig)->nextValue();
            }
            flip=!flip;
        }
        else flip=!flip;
    }
    else
    {
        for(val_list_it it = inputs.begin();it!=inputs.end();it++)
        {
            Value** sig = *it;
            (*sig)->nextValue();
        }
    }
   
    return 0;
}

void GroundMixer::setStereo(bool v)
{
    stereo=v;
}

bool GroundMixer::getStereo()
{
    return stereo;
}

/**************/
/*SINGLEGROUND*/
/**************/

SingleGround SingleGround::ground;

}