#ifndef SIGNAL_FILTER_H
#define SIGNAL_FILTER_H

#include "sample.h"
#include "signal.h"

namespace spl{

//FIXME: namechange, FIR also possible with all YC=0
class IIRFilter : public Value
{
  public:
    IIRFilter(int num_x, int num_y); //min is 1 for both
    virtual ~IIRFilter();
   
    //standard causal(does not depend on future input) difference eq i.e 1):
    //y[n]*YC[0] + y[n-1]*YC[1] + y[n-2]*YC[2]...  =  x[n]*XC[0] + x[n-1]*XC[1] ...
   
    //this means that output is 2):
    //y[n] = x[n]*XC[0]/YC[0] + x[n-1]*XC[1]/YC[0] - y[n-1]*YC[1]/YC[0] - y[n-2]*YC[2]/YC[0]...
   
    void setCoeff(smp_t* XC, smp_t* YC);
    void resetState();
   
    void setInput(Value** val);
    Value** getInput();
   
    virtual smp_t nextValue();
   
    int numX();
    int numY();
   
  protected:
    Value** in;
   
  private:
    
    int num_x, num_y;
    smp_t* Xcoeff;
    smp_t* Ycoeff;
   
    RingBuffer<smp_t>* outbuff;
    RingBuffer<smp_t>* inbuff;
};

/*class makes sure to update coeffecients in IIRFilterwhen cutoff etc changes
 * 
 */

//also: parameter change is instantanious, can introduce artifacts
//(use some slew-limiter on input if needed)
class BasicFilter : public IIRFilter
{
  public:
    BasicFilter(int nx, int ny, smp_t samplerate/*=DEF_SRATE*/);

    smp_t nextValue();
   
    //cutoff input
    void setCutoff(Value** cut);
    Value** getCutoff();
   
    void setSampleRate(smp_t rate);
    smp_t getSampleRate();
   
    //only used by constant-q filters, i.e filters
    //that keep the Q = cutoff/bandwitdh ratio constant(Q = Wc/B )
    //eg. Q = 0.01
    void setQ(Value** cut);
    Value** getQ();
   
    //FIXME: index ignored
    void reset(int index);
   
    //default: false
    void setUseSafe(bool v);
    bool getUseSafe();
   
    //use this to force cutoff between [lo hi]
    //def=10, samplerate
    void cutSafe(smp_t lo_hz, smp_t hi_hz);
    //def 0.1, 10
    void qSafe(smp_t lo, smp_t hi);
      
  protected:
    //this function should use 'cutoff' and 'q' appropriately
    virtual void calcCoeff() = 0;
   
    smp_t srate;
    Value** cut_in;
    Value** q_in;
    smp_t cutoff;
    smp_t q;
   
    smp_t old_cut;
    smp_t old_q;
   
    bool use_safe;
    smp_t cs_lo, cs_hi;
    smp_t qs_lo, qs_hi;
};

/*********/
/*FILTERS*/
/*********/

/*filters have no default values so run calcCoeff() before using them*/

class Filter_Identity : public BasicFilter
{
  public:
    Filter_Identity(smp_t samplerate=DEF_SRATE) : BasicFilter(1,1, samplerate)
    {
    }
    smp_t nextValue();
   
  private:
    void calcCoeff()
    {
    }
     
};

/*REF: "any" DSP book*/

/*Analog digitized filters
 *
 * n=1 can be made from
 -R---  
 *          Vin     || Vout
 *              -----
 *
 *with Bilinear transform: s=(2/T)(1-z^-1)/(1+z^-1)
 * frequency pre warping:  W=(2/T)tan(2*PI*fc/2*fs) 
 * where W is the analog freq and fc is then WANTED digital freq
 * (2/T can be omitted, will divide away)
 *
 *  remember :
 * the cutoff in the analogue filter(1/RC) is in(rad/s)
 * this is why must make digital in (rad/s) also (normalized between -pi, pi);
 *
 *will have ~-6db/octave rolloff over cutoff(-3db point) 
 *
 * ( this is excatly a butterworth filter with n=1 )
 */
/*made from butterworth analog filter design function |M(w)|^2 = 1/ ( 1+w^2n )
 * all filters have -6n dB/octave */



//Filter_CLASS_TYPE_-DB/OCT

/****/
/*LP*/
/****/

class Filter_BW_LP_6: public BasicFilter
{
  public:
    Filter_BW_LP_6(smp_t samplerate=DEF_SRATE) : BasicFilter(2,2, samplerate)
    {
        //FIXME: add to ALL
        //change name: 	calcCoeff(cut, q );?
        //calcCoeff();
	
    }
   
  private:
    void calcCoeff();
   
    smp_t X[2];
    smp_t Y[2];
};

class Filter_BW_LP_12 : public BasicFilter
{
  public:
    Filter_BW_LP_12(smp_t samplerate=DEF_SRATE) : BasicFilter(3,3, samplerate)
    {
    }
   
  private:
    void calcCoeff();
   
    smp_t X[3];
    smp_t Y[3];
};

class Filter_BW_LP_18 : public BasicFilter
{
  public:
    Filter_BW_LP_18(smp_t samplerate=DEF_SRATE) : BasicFilter(4,4, samplerate)
    {
    }
   
  private:
    void calcCoeff();
    smp_t X[4];
    smp_t Y[4];
};


/****/
/*HP*/
/****/

class Filter_BW_HP_6 : public BasicFilter
{
  public:
    Filter_BW_HP_6(smp_t samplerate=DEF_SRATE) : BasicFilter(2,2, samplerate)
    {
    }
   
  private:
    void calcCoeff();
    smp_t X[2];
    smp_t Y[2];
};


class Filter_BW_HP_12 : public BasicFilter
{
  public:
    Filter_BW_HP_12(smp_t samplerate=DEF_SRATE) : BasicFilter(3,3, samplerate)
    {
    }
   
  private:
    void calcCoeff();
    smp_t X[3];
    smp_t Y[3];
};


class Filter_BW_HP_18 : public BasicFilter
{
  public:
    Filter_BW_HP_18(smp_t samplerate=DEF_SRATE) : BasicFilter(4,4, samplerate)
    {
    }
   
  private:
    void calcCoeff();
    smp_t X[4];
    smp_t Y[4];
};




/****/
/*BP*/
/****/

class Filter_BW_BP_6 : public BasicFilter
{
  public:
    Filter_BW_BP_6(smp_t samplerate=DEF_SRATE) : BasicFilter(3,3, samplerate)
    {
    }
   
  private:
    void calcCoeff();
    smp_t X[3];
    smp_t Y[3];
};


class Filter_BW_BP_12 : public BasicFilter
{
  public:
    Filter_BW_BP_12(smp_t samplerate=DEF_SRATE) : BasicFilter(5,5, samplerate)
    {
    }
   
  private:
    void calcCoeff();
    smp_t X[5];
    smp_t Y[5];
};

}
#endif
