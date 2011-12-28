#ifndef SIGNAL_MIX_H
#define SIGNAL_MIX_H

#include "signal.h"

#include <algorithm>

#include <list>

namespace spl{

class MixerList : public Value
{
  public:
    typedef std::list<Value**> val_list;
    typedef std::list<Value**>::iterator val_list_it;
   
    MixerList();
   
    void addInput(Value** sig);
    void remInput(Value** sig);
    void remAllInput();
   
    uint getNumIn();
   
    void setGain(smp_t d);
      
    //clamp output
    void disableClamp();
    void setClamp(smp_t min, smp_t max);
    void setMin(smp_t v);
    void setMax(smp_t v);
    smp_t getMin();
    smp_t getMax();
   
   
    smp_t nextValue();
 
  protected:
    bool clamp_min;
    bool clamp_max;
    smp_t min;
    smp_t max;
   
    val_list inputs;
    smp_t depth;
};

//mixer array is good if caller wants to add/del inputs by index
class MixerArray : public Value
{
  public:
    MixerArray(int n);
   
    void addInput(Value** sig, int i);
    void remInput(int i);   
    void remAllInput();
            
    smp_t nextValue();
 
  private:
    Value*** inputs;
    int num;
};

//default mixer class used
class Mixer : public MixerList
{
};


//GroundMixer drains it's inputs and returns 0
class GroundMixer : public MixerList
{
  public:
    GroundMixer();
   
    //if stereo is enabled it will only drain everey second sample
    //default: true
    void setStereo(bool v);
    bool getStereo();
   
    smp_t nextValue();
   
  private:
    bool stereo;
    bool flip;
};


//singleton of GroundMixer
class SingleGround : public GroundMixer
{
  public:
    static SingleGround& instance()
    {
        return ground;
    }
   
  private:
    static SingleGround ground;
   
    SingleGround()
    {
    }
    virtual ~SingleGround()
    {
    }
   
    SingleGround(SingleGround& s);
    SingleGround& operator=(SingleGround& s);
    
};


class SmpPointAdd : public Value
{
  public:
    SmpPointAdd(smp_t* v0, smp_t* v1)
    {
        val0 = v0;
        val1 = v1;
    }
    smp_t nextValue()
    {
        return (*val0)+(*val1);
    }
   
  private:
    smp_t* val0;
    smp_t* val1;
};

}

#endif
