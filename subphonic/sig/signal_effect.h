#ifndef SIGNAL_EFFECT_H
#define SIGNAL_EFFECT_H

/*The name "effect" is very loose here: pretty much anything taking an input and doing something to it
  i.e this is the MISC
*/

#include "signal.h"
#include "window.h"
#include <vector>

#include "../math/fft.h"

namespace spl{

//REMEMBER def is when in=NULL => def

//only accepts -1, 1
class BitCrush : public Value
{
  public:
    BitCrush();
    ~BitCrush();
   
    void setInput(Value** in);
   
    //value bits, i.e not counting sign
    void setNBits(uint n=32); //formula b = (int)(2^(n-1)*in)/2^(n-1)
    void setNBitsIN(Value** in=NULL); //overrides setNBits() (used at NULL)
   
    smp_t nextValue();
 
  private:
    uint nbits;
    uint maxval;
    Value** input;
    Value** in_nbits;
};


class ValueBuffer : public Value
{
  public:
    ValueBuffer();
    ~ValueBuffer();
   
    //drains input even if not sampling
    void setInput(Value** in); //sampling src
   
    //length in samples, when sampling starts full length is read
    void setLen(uint samples=44100);//1sec def
   
    //if this is set read op's will be clipped to the current sample run's sampled samples
    //otherwise old buffer vals will be returned
    void setOnlyReadSampled(bool val=false);
   
    //resamples into buffer
    void setGate(Value** in);
   
   
    //sampled buffer output sample
    void setRead(Value** in);
    smp_t nextValue(); //returns what setRead() is pointing to
   
  private:
    Value** input;
    Value** v_gate;
    Value** read;

    std::vector<smp_t> buffer;
   
    //so as to skip resize every time, i.e 'buffer' only grows (by 2) when buff_len is smaler than setLen()
    uint len;
    bool sampling;
    bool regate_ok;
    uint sampled;
    bool only_sampled;
};


//WAVESHAPING
//some variations on overdrive/distortion static curves: DAFX, Zölzer ch. 5.3
//name: Clip, OK?

class ClipOverdrive : public Value
{
  public:
    ClipOverdrive();
    void setInput(Value** in);
   
    smp_t nextValue();
   
  private:
    Value** in;
};

class ClipTubeDistortion
{
    //uses asym curve
    //add a HP/LP in serial for a tube distortion effect
};


//SLEW limiter rate - set maxium increase per time unit
//this makes the change take different time, depending on how big it is

//example 1, if hz are used and porta wanted: say about 0.1s around A(440hz):
//rate should probably be ~40*10/44100 : ~0.01

//FIXME: this makes it hard to use good for both amp and hz, different magnitudes
//maybe check input?

//FIXME: untestes
class SlewLimitRate : public Value
{
  public:
    //ordered by complexity 
    enum MODE {LINEAR, /*SINE, SIGMOID*/};
   
    SlewLimitRate();
    ~SlewLimitRate();
   
    //default: LINEAR
    void setMode(MODE m);
      
    //if some is NULL value is 1 i.e instantanious rise(for max smp val=1.0)
    void setRiseRate(Value** v);
    void setFallRate(Value** v);
   
    void setInput(Value** v);
   
    virtual smp_t nextValue();
   
  private:
    Value** r_in;
    Value** f_in;
    Value** val_in;
   
    MODE mode;
   
    smp_t c_val;
    smp_t c_target;
    bool rise;
};


//SLEW limiter time - it takes 'time' to rise however small or big the change in level.
//one usage: a portamento that takes the same time


/*WARNING: this "restarts" the time counter every time the level changes(however small)
 */
  
class SlewLimitTime : public Value
{
  public:
    //ordered by complexity 
    enum MODE {LINEAR, SINE/*, SIGMOID*/};
   
    SlewLimitTime();
    ~SlewLimitTime();
   
    //default: LINEAR
    void setMode(MODE m);
   
    //if for example sample rate is 44100 set this to that
    //and then a setRiseTime of 1.0 will be one second
    //default: 1.0
    void setTimeMult(smp_t m);
   
    //if some is NULL value is 0
    void setRiseTime(Value** v);
    void setFallTime(Value** v);
   
    void setInput(Value** v);
   
    virtual smp_t nextValue();
   
  private:
    Value** rt_in;
    Value** ft_in;
    Value** val_in;
   
    MODE mode;
   
    smp_t c_val;
    smp_t c_target;
    smp_t c_time;
    smp_t c_dv;
    smp_t c_dv_dt;
    smp_t c_start;
   
    bool rise;
    smp_t time_m;
};

//i.e Multiply
//out = setGainIn * setGain * input
class Gain : public Value
{
  public:
   
    Gain(smp_t gain=1.0, Value** depth=NULL);
   
    //def: NULL = 1.0
    void setGainIn(Value** v);
   
    //def:1.0
    void setGain(smp_t d);
    smp_t getGain();
   
    //def: NULL = 0.0
    void setInput(Value** v);
    smp_t nextValue();
   
  private:
    smp_t d;
    Value** depth;
    Value** in;
};


class Rectify : public Value
{
  public:
    Rectify();
   
    void setInput(Value** v);
   
    //if half=true: neg vals are clipped,
    //else abs'ed
    //def:false
    void setHalf(bool h);
    bool getHalf();
   
    //bool* halfPtr();
   
    smp_t nextValue();
   
  private:
    bool half;
    Value** val;
};




/*samples 'input' at setPeriod overwritten by setClockIn() (zero crossing into pos)
 * singals a sampling with getClockOut*/

class SampleAndHold : public Value
{
  public:
    SampleAndHold();
    ~SampleAndHold();
   
    void setPeriod(Value** in);
    void setInput(Value** in);
    void setClockIn(Value** in);
   
    //only used if 'in'==NULL
    //default: 1
    void setClockLen(smp_t v); //in samples
   
    void setClockLenIn(Value** in); //in samples
   
    //outputs gate every update
    Value** getClockOut();
   
    smp_t nextValue();
   
  private:
    bool retrig;
   
    Value** per;
    Value** in;
    Value** clock_in;
    Value** c_len_in;
   
    OneShot* clock_out;
   

    uint c_len;
   
    smp_t sample;
};

/*this is SPECIAL while it must be grounded and actual out is on getOutput()
  takes upto 2inputs and either mixes them(out0) or stereo out(out0/out1)*/

class Crossfade : public Value
{
  public:
    Crossfade();
    ~Crossfade();
   
    void setInput(Value** in, uint chan/*0,1*/);
   
    //if mix is on, chan0 is the only one used; chan1 = 0
    Value** getOutput(uint chan=0); 
   
    //default=true, if mix is on both inputs are mixed to out0 otherwise acts like a stereo mixer
    void setMix(bool mix_to_ch0);
   
    //all other values that 0-1 clipped
    //a fade value of 0: only chan0  1:only chan1
    void setFade(Value** ctrl);
   
    //unused if setFade(Value**)
    void setFade(smp_t v);
   
    //this returns 0, must be added to ground(or mixed with output) if it is to work
    smp_t nextValue();
   
  private:
    smp_t fval;
    bool mix;
   
    Value** in[2];
    Value** fade;
   
    Const* out[2];
};
  




/*
 * 
 * 
 #define SHAPE_POLYNOM 0
 #define SHAPE_SINE 1
 #define GET_SHARP 0
 #define GET_SMOOTH 1

 * 
 class WaveShaper : public Value
 {
 public:
 //res power of 2, number of elements in a table entry
 WaveShaper(int res, int ser_len);
 void setGetMode(int mode);
 void zeroTable();
 void addInput(Value** v);
 void calcTable();
 smp_t lookup(smp_t v);
 smp_t lookup_interpolate(smp_t v);
 smp_t nextValue();
 void setCoeff(int n, smp_t v);
 void setShape(int mode)
 {
 calc_mode=mode;
 }
   
 smp_t* getTable()
 {
 return table;
 }
   
 int getTableSize()
 {
 return nelm;
 }
   
 private:
 Value** in;
   
 int ncoeff;
 smp_t* coeff;
 smp_t* table;
   
 int resolution;
 int nelm;
 int get_mode;
 int calc_mode;
 };

 class Compressor : public Value
 {
 public:
 Compressor();
 void setAttack(int a);
 void setDecay(int d);
 void setRatio(smp_t r);
 void setThreshold(short t);
 void setInput(Value** in);
 void reset();
 short nextValue();
   
 private:
 bool over;
 bool under;
 short threshold;
 smp_t a_slope;
 smp_t d_slope;
 int attack;
 int decay;
 smp_t ratio;
 smp_t c_ratio;
 int on_i;
 int off_i;
 Value** input;
 };
*/


//Short Time Fast Fourier Transform

//'n'-1 samples delayed output
class St_fft : public Value
{
  public:
   
    class Process
    {
      public:
        virtual ~Process()
        {
        }
	
        //should return true on sucess
        //gets complete fft, i.e a mirrored sequence ( remember: sample rate bin(max possible) is at fft_out[n/2])
        virtual bool operator()(Complex* fft_out, uint n, bool shifted) = 0;
    };
   
   
    //window better be n-long!
    //takes over 'window'
    St_fft(uint n, uint hop_size, Window<smp_t>* window);
    ~St_fft();
   
    //called with fft_out data, should change it inplace
    //set to NULL for remove/identity
    //WARNING: NOT deleted by St_fft
    void setProcess(Process* p);
   
    smp_t nextValue();
   
    void setInput(Value** v);
   
    //shifts spectrum so that zero frequency is in middle i.e fft_out[n/2]
    //slow for now
    //def:false
    void setShiftSpectrum(bool v);
    bool getShiftSpectrum();
   
    //defualt:off, => d_ functions will not work
    //OBSERVE: these are (probably) only usefull when setProcess(NULL)
    void setDebug(bool v);
   
    //clear running status
    void resetDebug();
   
    //returns the error introduced by the fft->ifft step, total run.
    //error =  before_fft - return_of_ifft
    //measured after input have been windowed
    smp_t d_getMaxAbsFFTError();
   
    //returns the maximum error between input and output
    //smp_t d_getMaxResynthError();
   
  private:
   
    //swaps left/right halfsides of input
    //this is slow
    template<class T>
    void circularHalfShift(T* in_out, uint n);
   
    Process* process;
   
    bool shift;
    bool do_debug;
    smp_t max_fft_err;
   
    uint n;
      
    ring* in_buff;
    ring* out_buff;
   
    smp_t* fft_in;
    Complex* fft_out;
   
    smp_t* ifft_out;
   
   
    Window<smp_t>* win;
    uint hop_size;
   
    Value** in;
   
    int out_ready;
    int oadd_index;
   
    FFTfixed<smp_t>* fft;
};

}


#endif
