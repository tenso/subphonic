#ifndef SIGNAL_SAMPLE_H
# define SIGNAL_SAMPLE_H

#include "signal.h"

namespace spl{

//raw sample data i.e: no channel book-keeping, must be done manualy

//FIXME: maybe stereo flag?
//FIXME: play() does not work if loop=false and speed=negative
//MABY: spline interpolation 
//
//takes over sample data, will delete it later, so better send in a compy and throw the 
//pointer away
class SampleValue : public Value
{
    enum STATE {S_RUN,S_IDLE};
   
  public:
    //how samples are interpolated when reading non-integer values
    enum INTERPOLATION_MODE{NONE, LINEAR};
   
    //how a retrig is handled if already playing:
    //HARD: just restart sample
    //SOFT: interpolate between start index and old read
    enum GATE_MODE {HARD_GATE, SOFT_GATE};
   
    SampleValue(Sample* s);
    virtual ~SampleValue(); //deletes 's'
   
   
    //default: LINEAR
    void setInterpolationMode(INTERPOLATION_MODE m);
   
    //default: HARD
    void setGateRetrigMode(GATE_MODE mode);
   
    //def: 0.0005
    //must be > 0.000000001
    void setGateInterpolateStep(smp_t speed);
   
    //in (samples)
    //when this is !=NULL this sets index to 0-len (speed/trig disabled)
    void setPosIn(Value** in);
   
    //plays sample, all theses are secondary after real trigger etc...
    //FIXME: only single shot(no loop)
    void play();
    void pause();
   
    //FIXME: 'i' ignored
    //restart at start index+i, and stop playback
    void reset(int i=0);
   
    //setLoop(true) : sample will loop as long as trigger is on: for a single play(no loop) send an impulse
    //        false : play then stop
   
    //default=true
    //requires a retrig to take effect
    void setLoop(bool v);
    bool getLoop();
   
    //(re)start sample from start index
    void setTrigger(Value** t);
   
    //stops playback(until next trigger restarts it, or play())
    void setStopIn(Value** in);
   
    //FIXME: toggles
    //setPlayPauseIn();
   
   
    //speed is 'in' * 's' (if in connected otherwise just 's'
    void setSpeedIn(Value** in);
   
    //default : 1.0
    void setSpeed(smp_t s);
   
    //IF convertion wanted: set this then run make() for getLen()
    //same as a setSpeed(from/to) offset
    void setResampleRate(int from, int to);
   
    //FIXME: should probably be smp_t's
    //default: 0, getLength()
    //clamped to 0, getLength(), start<=stop
    void setInterval(int start, int stop);
   
    //def:NULL : unused ==> setInterval() used
    void setStartIndex(Value** in);
    void setStopIndex(Value** in);
   
    //triggers sample to start playing if gate>'l'
    //default:0
    void setTrigLevel(smp_t l);
    smp_t getTrigLevel();
   
    const smp_t* onTrigger();
    Sample* getSample();
   
    uint getLength() const; //in samples(including stereo: number of "sound samples" at stereo:/2)
   
    bool playing();
   
    smp_t nextValue();
   
    const bool* getIsPlayingPtr();
   
  private:
   
    void readSample(smp_t* r_val, smp_t r_index);
    void stepIndex(smp_t* ind, smp_t speed, bool change_state);
   
    smp_t g_lev;
   
    smp_t ind;
    smp_t speed;
    smp_t re_speed;
   
    Sample* smp;
    Value** trig_in;
    Value** speed_in;
    Value** pos_in;
    Value** stop_in;
   
    STATE incycle;
    INTERPOLATION_MODE mode;
   
    int p_len;
    int start_i;
    int stop_i;
      
    smp_t tr;
   
    bool loop;
   
    bool trig_backwards;
    bool retrig;
   
    smp_t val;
   
    GATE_MODE gate_mode;
   
    bool soft_gate_run;
    smp_t soft_gate_index;
    smp_t soft_gate_inter;
    smp_t soft_gate_inter_step;
    smp_t soft_val;
   
    bool isplaying;
    smp_t inspeed;
    bool man_trig;
   
    Value** start_i_in;
    Value** stop_i_in;
   
    int d_start_i;
    int d_stop_i;
};

}

#endif
