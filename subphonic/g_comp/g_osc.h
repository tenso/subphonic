#ifndef G_OSC_H
# define G_OSC_H

# include "g_common.h"
# include "../sig/signal_osc.h"

class GOsc : public SoundComp
{
    static const uint NUM_OSC=6;
    static const int KNOB_NUMREVS=2;
   
    static const int FQMAX_C=1000;
    //static const smp_t FQMAX_F=3;

    static const int AMPMAX_C=1;
    //static const smp_t AMPMAX_F=0.1;
   
  public :
    GOsc(BitmapFont* f, GroundMixer& g);
    ~GOsc();
    virtual string name() const;
   
   
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
    Value** getOutput(unsigned int id);
   
      
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    int syncToIndex(unsigned int index);
   
    class State
    {
      public:
        State(int n, double shp, double amp_c, double amp_f, double hz_c, double hz_f)
        {
            this->amp_c=amp_c;
            this->amp_f=amp_f;
	     
            this->hz_c=hz_c;
            this->hz_f=hz_f;
	     
            shape=shp;
	     
            hscl=false;//clear
            ascl=false;
	     
            num=n;
        }
        ~State()
        {
        }
	
        int num;
	
        double shape;
	
        double amp_c;
        double hz_c;

        bool hscl;	/*unused*/
        bool ascl;	/*unused*/
	
        double amp_f;/*unused*/
        double hz_f;/*unused*/
    };
  
  private:
   
    void changeWF();
    void updateHz();
    void updateAmp();
   
    class C_WavForm : public Action
    {
      public:
        C_WavForm(GOsc* src);
        void action(Component* c);
	
        GOsc* src;
    };
   
    class C_SHAPE : public Action
    {
      public:	
        C_SHAPE(GOsc* gen);
        void action(Component* c);
	
        GOsc* src;
    };
   
    class C_AMP : public Action
    {
      public:	
        C_AMP(GOsc* gen);
        void action(Component* c);
	
        GOsc* src;
    };
   
   
    class C_FREQ : public Action
    {
      public:	
        C_FREQ(GOsc* gen);
        void action(Component* c);
	
        GOsc* src;
    };
   
    class R_RESET : public Action
    {
      public:
        R_RESET(GOsc* src)
        {
            this->src=src;
        }
	
        void action(Component* co);
	
        GOsc* src;
    };
   
    /******/
    /*DATA*/
    /******/
   
    Const* c_shape;
    Const* c_amp;
    Const* c_hz;
    Const* c_sync_in;
    ConvertHz2Per* c_per;
   
    ComboGen* gen;
   
    ValueForward* sig; //current sel generato
         
    OutputGiver* og_out;
    OutputGiver* og_osc[NUM_OSC];
    OutputGiver* og_sync;
   
    InputTaker* it_amp;
    InputTaker* it_per;
    InputTaker* it_shape;
    InputTaker* it_sync;
    InputTaker* it_phase;
   
    SDL_Surface* wforms[NUM_OSC];
    int curr_wf;
   
    Pixmap* sig_pix;
    Button* cw_button;

    Button* res;
   
    KnobSlider* slide;
   
    KnobSlider* aslide_c;
    //KnobSlider* aslide_f;
   
    KnobSlider* hslide_c;
    //KnobSlider* hslide_f;
};

#endif
