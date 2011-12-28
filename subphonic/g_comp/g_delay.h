#ifndef G_DELAY_H
# define G_DELAY_H

#include "g_common.h"
#include "../sig/signal_effect.h"

#define DELMULT 5.0f;

class GDelay : public SoundComp
{
public:
    GDelay(BitmapFont* f, GroundMixer& g);
    ~GDelay();
  
    virtual string name() const
    {
        return string("GDelay");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(double n, bool bs)
        {
            num=n;
            b_sec=bs;
        }
        ~State()
        {
        }
	
        double num;
        bool b_sec;
    };
   
  private:
   
    class C_Mix : public Action
    {
      public:
        C_Mix(GDelay* d);
	
        void action(Component* co);
      
      private:
        GDelay* src;
    };
   
    class C_Sec : public Action
    {
      public:
        C_Sec(GDelay* d);
	
        void action(Component* co);
      
      private:
        GDelay* src;
    };
   
    Slider* mix_slide;
   
    Value** in;
    Delay_MK2* del;
    Crossfade* cross;
    ValueDoppler* dop;
   
    Const* fade_v;
   
    Button* sec_button;
};

#endif
