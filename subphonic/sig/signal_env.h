#ifndef SIGNAL_ENV_H
#define SIGNAL_ENV_H

#include "signal.h"

#include <vector>

/******/
/*ADHSR*/
/******/

/*There is 2 adsr:
 * ADHSRin : uses gate to decide how long evelope: i.e sustain while gate on
 * ADHSRgen: "fixed" time envelope, i.e after gate triggers it will do it's cycle
 */ 

//envelope will release from the level that was when gate zeroed. i.e if just finished attack
//release will be from 'in'(1.0)

namespace spl{

class AHDSRin : public Value
{
  public:
    //soft cut default
    AHDSRin();
   
    //val==NULL => in=1.0
    void setInput(Value** val);
   
    //default: 1 
    //e.g : set to 44100, this means that a input of 1.0 gives 1sec at 44100hz
    void setTimeMult(smp_t m);
   
    //A,H,D,R all in number of samples(use setTimeMult() to get wanted format), sustain is level
    //all values except sustain are multiplyed by setTimeMult(default)
    //Convert::secToLen() should be used for control data to specify it in
    //seconds and also to make it samplerate independent
   
    //NULL = > 0
    void setAttack(Value** n_smp);
    void setHold(Value** n_smp);
    void setDecay(Value** n_smp);
    void setSustain(Value** lvl);
    void setRelease(Value** n_smp);
         
    smp_t nextValue();
    void reset();
   
    //rising edge
    void setGate(Value** gate);
   
    //level=0 will never trigger
    void setGateLevel(smp_t lev);
   
    //current gate value
    const smp_t* gateOn();
   
    //will the envelope restart from 0(hardcut=true) 
    //or from where it is(new start level for attack phase)(hardcut=false, DEFAULT)
    void setHardCut(bool v);
    bool getHardCut();
   
    //get the raw envelope signal
    //this is the value of the envelope so out is this times insignal
    const smp_t* envValPtr();
    smp_t envVal();
 
  private:
    smp_t c_mult;
   
    bool retrig;
   
    int rel_index;
    smp_t rel_level;
   
    bool hard_cut;
   
    smp_t val;
    smp_t gateon;
    smp_t gate_level; //when it triggers
    Value** gate;
    Value** in;
   
    Value** att;
    Value** hold;
    Value** dec;
    Value** sus;
    Value** rel;
   
    smp_t eval; //raw envelope
};


class AHDSRgen : public Value
{
  public:
    AHDSRgen();
   
    //val==NULL => in=1.0
    void setInput(Value** val);
   
    //def: 1
    void setTimeMult(smp_t m);
   
    //NULL => 0
    void setAttack(Value** n_smp);
    void setHold(Value** n_smp);
    void setDecay(Value** n_smp);
    void setSustainTime(Value** n_smp);
    void setSustain(Value** lvl);
    void setRelease(Value** n_smp);
         
    smp_t nextValue();
    void reset();
   
    void setGate(Value** gate);
   
    void setGateLevel(smp_t lev);
   
    const smp_t* gateOn();
   
    void setHardCut(bool v);
    bool getHardCut();
      
   
    const smp_t* envValPtr();
    smp_t envVal();
   
    const bool* inCyclePtr();
   
  private:
    smp_t c_mult;
   
    bool run;
    bool retrig;
   
    bool hard_cut;
   
    smp_t val;
    smp_t gateon;
    smp_t gate_level; //when it triggers
    Value** gate;
    Value** in;
   
    Value** att;
    Value** hold;
    Value** dec;
    Value** sus_l;
    Value** sus_t;
    Value** rel;
   
    smp_t eval; //raw envelope
};

//signal amplitute envelope follower
//envelope is sharp cut; one possibility is to soften it by slew-limit
//FIXME: attack/realease times?

class EnvFollow : public Value
{
  public:
    //peak returns the greatest value for each up_per
    //rms returns the squareroot of the mean of the squares over a up_per
    enum env_mode {PEAK, RMS};
   
    EnvFollow(env_mode mode, smp_t up_per);
   
    void setMode(env_mode mode);
   
    void setInput(Value** in);
   
    //period clamped to [1, inf]
    void setPeriod(Value** in);
   
    //unused if setPeriod()
    void setUpdatePeriod(smp_t n);
    smp_t getUpdatePeriod();
   
    smp_t nextValue();
   
    //pointer to what nextValue() returns
    const smp_t* getEnvelopeVal();
   
    //pointer to input
    const smp_t* getInputVal();
   
  private:
    smp_t next;
   
    env_mode mode;
    int up_per;
   
    Value** in;
    Value** per;
   
    smp_t rmsval;
    smp_t peakval;
    smp_t retval;
};


//FIXME: time step instead of index++ makes period changes MUCH better

//multiplies input with the interpolated control value(-SMP_MAX, SMP_MAX)
//it will complete the width of the control points(last point's x) in period samples
//so at 44100hz a period of 22050 gives 2hz etc
//WARNING: input data is expected to have only positive x values!
class CPInterpolator : public Value
{
  public:
    enum MODE {LINEAR, BEZIER} ;
   
    CPInterpolator();
   
    void setGate(Value** g);
   
    //after a trig it finishes cycle before new trig possible
    void setGateFinishCycle(bool val);
    bool getGateFinishCycle();
   
    //sustains the value last output before cycle done
    void setHoldEndValue(bool val);
    bool getHoldEndValue();
   
    //if gate off: endless mode
    bool getGateTrigOn();
    void setGateTrigOn(bool v);
   
    smp_t* gateValPtr();
   
    //when index hits x-pos in data > 'val' sustain is on until gate offs
    void setSustainIndex(int val);

    smp_t* getDataIndexPtr();
    //void preCalcTable(bool v);
   
    //default: linear
    void setMode(MODE m);
   
    //def:0
    void setPeriod(Value** p);
   
    //data must be sorted in increasing x
    //last point's x is used to determine width of envelope
    //WARNING: data.y is interpreted as smp_t while data.x as an int(truncated)
    void setData(const std::vector<SigCoord>& data);
   
    //def: 1.0
    void setInput(Value** v);
    smp_t nextValue();
   
  private:
    enum CYCLE {GATE_ON, GATE_OFF, SUSTAIN, SLEEP, ENDLESS};
    CYCLE incycle;
    bool first_sust;
   
    smp_t gate_th;
    bool gate_finish_cycle;
    bool use_gate;
    Value** gate;
    smp_t gval;
    int sustain_v;
   
   
    std::vector<SigCoord> data;
   
    MODE mode;
    Value** period;
    Value** in;
   
    int data_i; 
    smp_t data_i_val;
   
    unsigned int w;
    unsigned int cs,ce;
   
    bool retrig;
   
    smp_t val;
    bool hold_end_val;
    bool have_end_val;
};

}
#endif
