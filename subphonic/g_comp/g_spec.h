#ifndef G_SPEC_H
# define G_SPEC_H

# include "g_common.h"
# include "../sig/window.h"

#include "../math/complex.h"

class GSpec : public SoundComp
{
  public:
    GSpec(BitmapFont* f, GroundMixer& g);
    ~GSpec();
   
    virtual string name() const
    {
        return string("GSpec");
    }
   
    Value** getOutput(unsigned int id);
   
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
  private:
   
    class C_VQuota : public Action
    {
      public:
        C_VQuota(GSpec* src);
        void action(Component* c);
	
      private:
        GSpec* src;
    };
   
    //extracts magnitude
    class Visualize : public St_fft::Process
    {
      public:
        //len can be smaller than 'n'
        Visualize(uint len);
        ~Visualize();
	
        //if set >0.5 the total fft spectrum will be plotted(mirrored )
        //def:0.5
        void setPlotQuota(double p); //1.0= 100%
	
        bool operator()(spl::Complex* fft_out, uint n, bool shifted);
	
        const smp_t* getMagData();
        const smp_t* getPhaseData();
        uint getPlotDataLen();
	
	
        //def:MAG
        /*enum MODE {MAG, PHASE};
          void setMode(MODE mode);*/
	
      private:
        //MODE mode;
	
        smp_t* mag_data;
        smp_t* phase_data;
        uint len;
        uint p_len;
    };
   
    Visualize* viz;
   
    Histogram<smp_t>* p_spec;
    DataPlot<smp_t>* p_plot;
     
    InputTaker* in;
    OutputGiver* og;
    St_fft* sig;
    Window<smp_t>* bwindow;
   
    Slider* s_vquota;
    Label* l_vquota;
};
#endif
