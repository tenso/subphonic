#ifndef G_FFT_FILT_H
#define G_FFT_FILT_H

#include "g_common.h"
#include "../sig/window.h"
#include "../math/complex.h"

class GFFTFilt : public SoundComp
{
    static const uint MAX_SLIDERS = 128;
   
    static const uint FFT_LEN=1024*4; //make sure this is power of 2!
   
  public:
    GFFTFilt(BitmapFont* f, GroundMixer& g);
    ~GFFTFilt();
   
    virtual string name() const
    {
        return string("GFFTFilt");
    }
   
    Value** getOutput(unsigned int id);
   
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
      
    class State
    {
      public:
        State(vector<Slider*>& sliders)
        {
            DASSERT(sliders.size()<MAX_SLIDERS);
            num=sliders.size();
	     
            for(uint i=0;i<sliders.size();i++)
            {
                coeff[i]=sliders[i]->getValue();
            }
        }
	
        double coeff[MAX_SLIDERS];
        uint num;
	
        uint unused[10];
    };
   
  private:
   
    class Transform : public St_fft::Process
    {
      public:
        //len MUST be same as 'n' in operator()
        //def: all coeff = 0
        Transform(uint len);
        ~Transform();
	
        bool operator()(spl::Complex* fft_out, uint n, bool shifted);
	
        //sets  symmetric i.e data[bin_i] & data[len-1-bin_i]
        //this means use bin_i [0 len/2]
        void setCoeffSym(uint bin_i, double c);
	
        void setCoeff(uint bin_i, double c);
	
        double getHzPerBin() const;
	
        //remember: all freq>samplerate == -(freq-half_srate)
        double getBinHz(uint bin_i) const;
	
        //truncated
        uint getBinIndex(double freq) const;
	
        uint getNumBins() const;
	
      private:
        smp_t* data;
        uint len;
    };
   
    class C_SliderUpdate : public Action
    {
      public:
        C_SliderUpdate(GFFTFilt* src, uint id, uint start_bin, uint num_bins);
	
        void action(Component* c);
	
      private:
        GFFTFilt* src;
        uint id;
        uint start_bin;
        uint num_bins;
    };
   
    class C_Change : public Action
    {
      public:
        enum MODE {ZERO, HALF, ONE};
	
        C_Change(GFFTFilt* src, MODE mode);
	
        void action(Component* c);
	
      private:
        GFFTFilt* src;
        MODE mode;
    };
   
   

   
    Transform* tran;
   
    //Histogram<smp_t>* p_spec;
   
    //Diagram* dia;
    InputTaker* in;
    OutputGiver* og;
    St_fft* sig;
    Window<smp_t>* bwindow;
   
    uint h_fft_len;
    uint bins_per_slider;
    double hz_per_slider;
   
    vector<Slider*> sliders;
   
    Button* b_zero;
    Button* b_half;
    Button* b_one;
};
#endif
