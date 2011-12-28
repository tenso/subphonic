#ifndef SIGNAL_DELAY_H
#define SIGNAL_DELAY_H

#include "signal.h"

//fractional, linear interpolated delaytime line

//CHECKMEIN: the delay grow/shrink corssfades between 'old' and 'new' read pointers
//maybe it is better to step through the middle ptrs so to say

//does not discard delay line i.e keeps it at max(set_delays) (only read pointer changes)
//v.s Delay : discards

//must be >= 1
namespace spl{
const uint INITIAL_DELAY_SZ = 44100*10;


//NOTICE: because this is a fractional delay line it needs to store one more sample then the integer delay time
//i.e if integer delay time is 0, delay line must be able to give back [0, 0.99999...] in delay. Thus delay line is = 1 sample long
//in this case. However it can still return samples that are delay with say 0,1,2 samples BUT: there will be an initial line fill
//even if delay time is 0. This means(dealy=0):first sample skipped(initial fill) but then samples are returned with delay 0 etc

class Delay_MK2 : public Value
{
    enum CROSS_TYPE{NONE, BACKWARD, FORWARD, GROW };
   
  public:
   
    enum F_INTER_MODE{FRAC_INTERPOLATE_NONE, FRAC_INTERPOLATE_LINEAR};
   
    //if max_delay_smp != -1, delay times are truncated to max_delay_smp
    Delay_MK2(int max_delay_smp=-1/*inf*/);
   
    //in samples
    void setDelay(Value** d); //seconds if setTimeMult(samplerate)
   
    void setInput(Value** in);
   
    //peek in samples(always) ahead in delay line i.e in==0 gives normal op, in==delay_len gives a delay of 0
    //clipped to [0 delaylength] ; WARNING: no warnings given 
    void setPeekInput(Value** in);
   
    smp_t nextValue();
   
    //default: LINEAR
    void setFracInterpolateMode(F_INTER_MODE m);
   
    //void setCrossTime(smp_t d);
   
    //def: 1.0
    //set to: sample_rate(44100) for => in:1.0 out: 1.0 sec
    void setTimeMult(smp_t m);
   
  private:
   
    smp_t getValue();
   
    F_INTER_MODE mode;
   
    bool warned;
    smp_t t_mult;
    uint max_delay_smp;
   
    uint cross_d;
    smp_t cross_d_frac;
    uint curr_d;
    smp_t curr_d_frac;
    uint peek_i;
    smp_t cross;
    smp_t cross_step;
    CROSS_TYPE do_cross;
   
    uint delay_len;
    uint curr_read;
    uint to_add;
   
    Value** delay;
    Value** input;
    RingBuffer<smp_t>* buff;
   
    bool first_done;
   
    bool inf_delay_size;
   
    uint d_chg_sleep_val;
    uint d_chg_sleep;
   
    Value** peek;
};




/*MK1*/


//FIXME:want dynamic max delay
//also: fractional delay time
static const smp_t DEF_MAXDELAY=10; //10s

//does a linear crossfade between changing delay times
class Delay : public Value
{
    enum CROSS_TYPE{NONE, BACKWARD, FORWARD, GROW };
   
  public:
   
    enum F_INTER_MODE{FRAC_INTERPOLATE_NONE, FRAC_INTERPOLATE_LINEAR};
   
    Delay(uint max_delay_smp);
   
    void setDelay(Value** d); //seconds
   
    void setInput(Value** in);
      
    smp_t nextValue();
   
    //default: LINEAR
    void setFracInterpolateMode(F_INTER_MODE m);
   
    void setTimeMult(smp_t m);
   
    //void setCrossTime(smp_t d);
   
  private:
   
    smp_t getValue();
   
    F_INTER_MODE mode;
   
    bool warned;
    smp_t t_mult;
    int max_delay_smp;
   
    int cross_d;
    smp_t cross_d_frac;
    int curr_d;
    smp_t curr_d_frac;
    int peek_i;
    float cross;
    float cross_step;
    CROSS_TYPE do_cross;
   
    Value** delay;
    Value** input;
    RingBuffer<smp_t>* buff;
   
    bool first_done;
};

}

#endif
