#ifndef G_GAIN_H
#define G_GAIN_H

#include "g_common.h"
#include "../sig/signal.h"

static const float GAIN=0.01f;

class GGain : public SoundComp 
{
  public:
    GGain(BitmapFont* f, GroundMixer& g);
    ~GGain();
   
    virtual string name() const
    {
        return string("GGain");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
    void updateGain();
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);   
   
    class State
    {
      public:
        State(double s, double iboost, bool b)
        {
            slider=s;
            button=b;
            this->iboost=iboost;
        }
	
        double slider;
        double iboost;
        bool button;
    };
   
  private:
   
      
    class C_Amp : public Action
    {
      public:

        C_Amp(GGain* gen);
        void action(Component* co);
	
        GGain* src;
    };
   
    class Update : public Action
    {
      public:

        Update(GGain* s);
	
        void action(Component* co);
	
        GGain* src;
    };
   
    class FLIP : public Action
    {
      public:
        FLIP(GGain* src);
	
        void action(Component* co);
	
        GGain* src;
    };
   
    Slider* amp_slide;
    Gain* gain;
    smp_t inboost;
    NumberSelect<smp_t>* ns;
    Button* pmode1;
    int sgn;
};

#endif
