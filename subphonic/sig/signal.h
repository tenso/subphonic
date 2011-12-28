#ifndef SIGNAL_H
#define SIGNAL_H

#include "sample.h"
#include "../util/types.h"
#include "../util/debug.h"
#include "../util/rand.h"
#include "signal_util.h"

#include "../util/ringbuffer.h"

#include "../math/mathutil.h"

namespace spl{

/*PRAXIS:
 * use setValue(NULL) to remove an input(same as set to default)
 * setValue() for single inputs addInput() for multiple FIXME: not good, but have been so long now, change to addInput() for all
 * 
 * all Value::nextValue() must return a value between +-SMP_MAX,
 * 
 *(oscillators etc which wants periods >1 
 * must map SMP_MAX to SAMPLERATE/2)
 * 
 * Vals that expect control data to be other ranges:
 * SignalGenerator::setPeriod 0->inf
 * ADSRin::setTimeMult() 0->inf
 * CPinterpolator::setPeriod
 * 
 * gates reset when < GATE_ZERO_VAL (i.e ready for another trig)
 */

# include "defines.h"

using namespace spl;

typedef RingBuffer<smp_t> ring;

//some things need 2d/3d data...
class SigCoord
{
  public:
    SigCoord(smp_t x=0, smp_t y=0, smp_t z=0)
    {
        this->x=x;
        this->y=y;
        this->z=z;
    }
   
    smp_t x,y,z;
};

//CHECK: is this really good:
/*classes uses Value** so that the source can change without need to reset input/ouput*/

class Value
{
  public:
    Value();
    virtual ~Value();
   
    virtual smp_t nextValue() = 0; //this returns the next sample
   
    //get many(if computing audio output, set scale to max output positive value
    //e.g if shorts are wanted(16bit) set to SSHORT_MAX
    //if using for buffered read from ug set T=smp_t and leave scale/clip
    template<class T>
    void make(T* data, uint len, smp_t scale=1.0 /* , bool clip=false*/)
    {
        //static bool warned=false; //fixme?
        if(feq(scale, (smp_t)1.0))
        {
            for(uint i=0;i<len;i++)
            {
                data[i] = (T)nextValue();
            }
        }
        else
        {
            for(uint i=0;i<len;i++)
            {
                smp_t val = nextValue();
                /*
                  if(clip)
                  {
                  smp_t aval = fabs(val);
                  smp_t over = SMP_MAX;
                  * 
                  if(aval >= over)
                  {
                  if(!warned)
                  {
                  DERROR("overflow: " << val);
                  warned=true;
                  }
                  val = (val>0) ? over : -over;
                  }
                  }
                */
                data[i] = (T)(val*scale);
            }
        }
    }
   
    void make(Sample* smp, smp_t scale=1.0/*, bool clip=false*/);
   
    virtual void reset(int index=0);
   
  protected:
    int index; //some use negative for some state...
   
};


/*FOLLOWING CLASSES SHOULD BE MOVED TO OTHER FILES*/



/*might ALSO be used to make a graph of a Value:
 *add a LinearSweep* as setInput() then run:
 * 
 *          Value::make(ycoord, ncoord_wanted, 1) as Y-coord, 
 *and LinearSweep::make(xcoord, ncoord_wanted, 1) as X-coord (if needed)
 */ 
class LinearSweep : public Value
{
  public:
    //n_smp>0
    //n_smp=1 outputs hold with first sample
    //outputs 'start' with first nextValue() e.g n_smp=2 out : start, end
    LinearSweep(smp_t start=-1, smp_t end=1, uint n_smp=DEF_SRATE);
   
    //use reset() to start at 'start' again
   
    //output lineary from 'start' to 'end' in 'n_smp'
    //after n_smp: setLoop(), holdX() decides
    //default: as constructor
    //n_smp>0
    void setRange(smp_t start, smp_t end, uint n_smp);
   
    //restart after 'smp' is done, otherwise holdVal() is used
    //default: false
    void setLoop(bool val); 
    bool getLoop();
   
    enum HOLD {START, END, ZERO};
   
    //setLoop(false) for this:
    //after 'cycle'(n_smp) is done this value is outputed untill reset()
    //default: END
    void setHold(HOLD h);
    HOLD getHold();
   
    smp_t nextValue();
   
  private:
    bool loop;
    HOLD hold;
   
    uint n_smp;
    smp_t start;
    smp_t end;
   
    smp_t val_inc;
    smp_t val;
};


/*this doubbles a input Value
 * getSlave() returns the doppelganger
 * this means that ValueDoppler->nextValue() and slave->nextValue() will
 * always have the same value for one given time, no matter which one is first to 
 * run nextvalue()*/

class ValueDoppler : public Value
{
  public:
   
    /*in: num>0
     *
     * this makes a total of 1+num_slaves outs, this one(master) included*/
   
    ValueDoppler(uint num_slaves=1, Value** input=NULL);
    ~ValueDoppler();
   
    Value** getSlave(uint num=0);
   
    //def=NULL
    //if input=NULL out val=0
    void setInput(Value** r);
   
    smp_t nextValue();
   
  private:
   
    class DopplerSlave : public Value
    {
      public:
        DopplerSlave(ValueDoppler* master);
	
        void setValue(smp_t v);
	
        smp_t nextValue();
	
        //FIXME:
        //private:
        bool first;
        ValueDoppler* master;
        smp_t val;
    };
   
    void getValue();
   
    uint num;
    DopplerSlave** slaves;
    bool first;
    Value** real;
    smp_t val;
};


//this "just" forwards in, this can be good if a class needs to be able to change
//an out value, then it can setInput here and return a pointer to ValueForward

class ValueForward : public Value
{
  public:
    ValueForward();
   
    void setInput(Value** in);
    Value** getInput();
   
    virtual smp_t nextValue();
   
  private:
    Value** forward;
};


class Const : public Value
{
  public:
    Const(smp_t v=0);
    
    smp_t nextValue();
   
    void setValue(smp_t v);
    smp_t getValue();
   
    //FIXME: obsolete names?
    void set(smp_t v)
    {
        setValue(v);
    }
   
    smp_t get()
    {
        return getValue();
    }
   
   
  private:
    smp_t val;
};

//returns what 'v' point to
class SmpPoint : public Value
{
  public:
    SmpPoint(const smp_t* v=NULL);
    void setSmp(const smp_t* v);
   
    smp_t nextValue();
   
    //void set(smp_t v);
   
  private:
    const smp_t* val;
};



/**********/
/*CONVERTS*/
/**********/

class ConvertHz2Per : public Value
{
  public:
    ConvertHz2Per(const Convert& c, Value** v=NULL);
    void setInput(Value** v);
    smp_t nextValue();
   
  private:
    Value** val;
    Convert c;
};


//i.e len: num samples
class ConvertSec2Len : public Value
{
  public:
    ConvertSec2Len(const Convert& c, Value** v=NULL);
    void setInput(Value** v);
    smp_t nextValue();
   
  private:
    Value** val;
    Convert c;
};


//converts 0.0=(0hz) and 1.0=(samplerate/2 (nyquist) hz) => approp period
class ConvertUnifFreq2Per : public Value
{
  public:
    ConvertUnifFreq2Per(const Convert& c);
    void setInput(Value** v);
    smp_t nextValue();
   
  private:
    Value** val;
    Convert c;
};

class Stereo : public Value //stereo value shifter
{
  public:
    Stereo(int chans=1);
    void setNChan(int chans);
   
    //alternates; so four nextValue() calls is: left|right|left|right
    smp_t nextValue();
    void addInput(Value** in, int chan);
   
    //FIXME: this name
   
    //if this is true, it will replicate first channel over all out channels
    //otherwise it will take samples as usual
    void distFirstCh(bool v);
   
  private:
    smp_t ret;
    bool mix;
    Value** chans[2];
    int curr;
    int n;
};


//FIXME: move (but where?)

class Gate : public Value
{
  public:
    Gate(smp_t level=SMP_MAX);
   
    //effective next trig, good?
    void setOnLevel(smp_t lvl);
   
    void setOn(bool v);
    bool getOn();
   
    //set gate to off but retrig in 'smpls' 
    void offRetrigIn(unsigned int smpls);
   
    smp_t nextValue();
   
    const smp_t* gatePtr();
   
    //automaticaly shuts gate off after 'len' after each setOn(true)
    //default: false
    void setOneShot(bool on);
    bool getOneShot();
   
    //def: 1
    //if shot is on: change after next off
    //if gate on => make it a shot from now(i.e offs in len)
    //if gate on and shotMode true->false : gate stays on
    void setShotLen(uint len);
   
  private:
    bool shot_mode;
    uint shot_len;
   
    smp_t gate;
    bool on;
    smp_t lvl;
    unsigned int retrig_c;
};



//output 'lvl' for 'len' samples then go to 0
class OneShot : public Value
{
  public:
    OneShot(smp_t lvl=SMP_MAX);
    smp_t nextValue();
    void setLevel(smp_t l);
   
    //default:1
    //in samples(use convert if sec's wanted(ConvertSec2Len))
    void setShotLen(uint len);
   
    void set();//sets to last default(from constructor) or set(lvl)
    void set(smp_t lvl); //WARNING: resets 'lvl'
    void unset();
   
    //if setAlwaysOn(true) : works as a manual gate
    //i.e not a 'OneShot' any more(must unset() to off it)
    //if a shot is on when this is set gate stays on
    //if gate is on when this is unset a shot will remain
    void setAlwaysOn(bool v);
   
  private:
    bool ison;
    bool always;
    uint ticks;
    smp_t lvl;
};



class Linear : public Value
{
  public:
    //this ticking index one every drain divided by d: set 'd' to samplerate to make a timer
    Linear(smp_t d=1);
   
    //only ticks when in>GATE_ZERO_LVL, still resets though
    void setRunInput(Value** in);
   
    //resets after zero cross
    void setResetInput(Value** in);
   
    //returns seconds
    smp_t nextValue();
   
    //make sure it is not zero
    void setD(smp_t d);
   
    //def:NORMAL(same as all other gates in program)
    enum MODE {NORMAL, DURING_GATE};
    void setRetrigMode(MODE m);
   
  private:
    MODE m;
    Value** run;
    Value** reset;
    bool retrig;
    uint tick;
   
    smp_t d;
};

}
#endif
