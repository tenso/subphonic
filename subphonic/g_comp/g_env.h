#ifndef G_ENV_H
#define G_ENV_H

#include "g_common.h"
#include "../sig/signal_effect.h"

class GEnv : public SoundComp 
{
  public:
    GEnv(BitmapFont* f, GroundMixer& g);
    ~GEnv();
   
    virtual string name() const
    {
        return string("GEnv");
    }

    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(double per, bool mode)
        {
            this->per=per;
            this->mode=mode;
        }
		
        double per;
        bool mode;
    };
   
  private:
   
    void updatePer();
   
    #define MAX_PERIOD 0.2
    static const int KNOB_NREVS=2;
   
    class C_UP_PER : public Action
    {
      public:
        C_UP_PER(GEnv* src);
        void action(Component* c);
      
      private:
        GEnv* src;
    };
   
    class PMODE : public Action
    {
      public:
        PMODE(GEnv* src);
        void action(Component* c);
      
      private:
        GEnv* src;
    };
   
   
    KnobSlider* per_knob;
    Button* mode;
    EnvFollow* sig;
    Const* per_v;
    ConvertSec2Len* converted;
};
#endif
