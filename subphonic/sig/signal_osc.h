#ifndef SIGNAL_OSC_H
# define SIGNAL_OSC_H

# include <stdlib.h>
#include <time.h>

# include "signal.h"

namespace spl{

//generators can run with negative period

/*FIXME:
 * period problem: int->smp_t OK in ComboGen
 * suppressed periods for combo gen
 * phase for SigGen */

//DONT USE THIS:

class SigGenValue
{
  public:
    virtual ~SigGenValue()
    {
    }
   
    virtual smp_t genValue(smp_t amp, int period, smp_t shape, int index, smp_t i_time) = 0;
};


class SignalGenerator : public Value
{
  public:
   
    //will takeover 'gen' so caller must discard it afterwards
    SignalGenerator(SigGenValue* gen);
    virtual ~SignalGenerator();
   
    void setAmp(Value** a);
    void setPeriod(Value** p);
    void setShape(Value** v); //0, 1
   
    //ouput 0 for this many periods before staring(good for some metronome usage)
    //def:0
    //reset at sync or reset()
    void setSuppressedPeriods(uint supp); //sets both
    void setSuppressedPeriodsSync(uint supp);
    void setSuppressedPeriodsReset(uint supp);
   
    //FIXME realy give back something not yours? 
    /*Value** getAmp();
      Value** getPeriod();
      Value** getShape();*/
   
    smp_t nextValue();
   
    virtual void reset(int i=0);
    void reset(int i, smp_t i_t); //(re)set index to i, and time to i_t /*decopled but should correspond for prober use*/
   
    //hard sync
    void setSyncIn(Value** in); //sync on in, i.e reset wf
    //Value** getSyncIn();
   
    Value** getSyncOut(); //emitts a pulse OneShot after each period
   
  private:
    SigGenValue* valgen;
   
    Value** amp;
    Value** period;
    Value** shape;
   
    Value** sync_in;
    OneShot* sync_out;
   
    bool do_sync;
    bool resync;
   
    smp_t i_time;
   
    smp_t a;
    int p;
    smp_t val;
   
    uint sy_sup;
    uint re_sup;
    uint sup_count;
};

//FIXME: rename to SquareOscValue

class SquareValue : public SigGenValue
{
  public:
    smp_t genValue(smp_t a, int p, smp_t s, int index, smp_t i_time);
};

class TriangleValue : public SigGenValue
{
  public:
    smp_t genValue(smp_t a, int p, smp_t s, int index, smp_t i_time);
   
  private:
    smp_t slope;
};

class SawtoothValue : public SigGenValue
{
  public:
    smp_t genValue(smp_t a, int p, smp_t s, int index, smp_t i_time);
};

class SineValue : public SigGenValue
{
  public:
    smp_t genValue(smp_t a, int p, smp_t s, int index, smp_t i_time);
};

class PulseValue : public SigGenValue
{
  public:
    smp_t genValue(smp_t a, int p, smp_t s, int index, smp_t i_time);
};

class WNoiseValue : public SigGenValue
{
  public:
    WNoiseValue()
    {
        //FIXME: not here!
        srand(time(NULL));
    }
   
    smp_t genValue(smp_t a, int p, smp_t s, int index, smp_t i_time);
};

/**************/
/*DEFAULT GENS*/
/**************/

class OscSquare : public SignalGenerator
{
  public:
    OscSquare() : SignalGenerator(new SquareValue())
    {
	
    }
};

class OscSine : public SignalGenerator
{
  public:
    OscSine() : SignalGenerator(new SineValue())
    {
	
    }
};

class OscTriangle : public SignalGenerator
{
  public:
    OscTriangle() : SignalGenerator(new TriangleValue())
    {
	
    }
};

class OscSawtooth : public SignalGenerator
{
  public:
    OscSawtooth() : SignalGenerator(new SawtoothValue())
    {
	
    }
};

class OscPulse : public SignalGenerator
{
  public:
    OscPulse() : SignalGenerator(new PulseValue())
    {
	
    }
};

class OscWNoise : public SignalGenerator
{
  public:
    OscWNoise() : SignalGenerator(new WNoiseValue())
    {
	
    }
};

/*USE THIS:*/

/***********************/
/*ALL GENERATORS IN ONE*/
/***********************/

/*NOTICE: must add this class to ground(or something that is always drained).
 * Then use getGen(TYPE) to get the individual generators which all use the same index/sync/inputs etc
 * added through this class, this is good: only one instance that calculates time_s sync etc, while providing all
 * shapes
 */

//FIXME: WARNING: phase not implemented for pulse

class ComboGen : public Value
{
    static const uint NUM_OSC=6;
   
  public:
   
    enum TYPE { SQUARE=0, SAW, TRI, SINE, PULSE, NOISE};
   
    ComboGen();
    virtual ~ComboGen();
   
    //always returns the same gen for a 't' and gen is only a value, so it is OK to get many, no dopplers needed
    //WARNING: make sure not to use these after this class goes out of scope...
    Value** getGen(TYPE t);
   
    //def: 1.0
    void setAmp(Value** a);
    //def:2205
    void setPeriod(Value** p);
    //def: 0.5 == default shape
    void setShape(Value** v); //0, 1
   
    void setPhase(Value** v); //1.0==one period off
   
    //FIXME realy give back something not yours? atleast make const
    /*Value** getAmp();
      Value** getPeriod();
      Value** getShape(); */
   
    smp_t nextValue(); //returns 0, just a drain updater
   
    virtual void reset(int i=0);
   
    //hard sync
    void setSyncIn(Value** in); //sync on in, i.e reset wf
    //Value** getSyncIn();
   
    Value** getSyncOut(); //emitts a pulse OneShot after each period
   
  private:
   
    void reset(int i/*unused*/, smp_t i_t); //(re)set index to i, and time to i_t /*decoupling is not an error*/
   
    /*these are thightly coupled with 'master', uses references to all parameters
     * for speed, atomatically recalc values when master says so*/
   
    class SingleGenST : public Value
    {
      public:
        SingleGenST(ComboGen* master, uint i);
        ~SingleGenST();
	
        smp_t nextValue();
	
      protected:
        virtual void reCalc()=0;
	
        smp_t& a;
        smp_t& p;
        smp_t& s;
        smp_t& ph;

        bool& recalc;
        smp_t& i_time;
		
        smp_t val;
	
    };
   
    class SGST_Square : public SingleGenST
    {
      public:
        SGST_Square(ComboGen* master, uint i) : SingleGenST(master,i)
        {
        }
	
        void reCalc();
    };
    class SGST_Sine : public SingleGenST
    {
      public:
        SGST_Sine(ComboGen* master, uint i) : SingleGenST(master,i)
        {
        }
        void reCalc();
    };
    class SGST_Sawtooth : public SingleGenST
    {
      public:
        SGST_Sawtooth(ComboGen* master, uint i) : SingleGenST(master,i)
        {
        }
        void reCalc();
    };
   
    class SGST_Triangle : public SingleGenST
    {
      public:
        SGST_Triangle(ComboGen* master, uint i) : SingleGenST(master,i)
        {
        }
        void reCalc();
    };
   
    class SGST_Pulse : public SingleGenST
    {
      public:
        SGST_Pulse(ComboGen* master, uint i) : SingleGenST(master,i)
        {
        }
        void reCalc();
    };
    class SGST_WNoise : public SingleGenST
    {
      public:
        SGST_WNoise(ComboGen* master, uint i) : SingleGenST(master,i)
        {
        }
        void reCalc();
    };
   
   
    smp_t a;
    smp_t p;
    smp_t s;
    smp_t ph;
   
    smp_t i_time; 
    smp_t a_i_time;
   
    bool recalc[NUM_OSC];
   
   
    SingleGenST* gen[NUM_OSC];
   
    Value** amp;
    Value** period;
    Value** shape;
    Value** phase;
   
    Value** sync_in;
    OneShot* sync_out;
   
    bool do_sync;
    bool re_sync;
   
};

}

#endif
