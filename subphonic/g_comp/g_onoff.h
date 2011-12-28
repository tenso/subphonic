#ifndef G_ONOFF_H
#define G_ONOFF_H

#include "g_common.h"

#include "../sig/signal_other.h"

class GOnOff : public SoundComp
{
  public :
    GOnOff(BitmapFont* f, GroundMixer& g);
    ~GOnOff();
   
    virtual string name() const
    {
        return string("GOnOff");
    }
   
    Value** getOutput(unsigned int id);

    virtual void addInput(Value** out, unsigned int fromid);
    virtual void remInput(Value** out, unsigned int fromid);
   
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);

    class State
    {
      public:
        State(bool mode)
        {
            this->mode = mode;
        }
	
        bool mode;
    };
   
  private:
    OnOff* sig;
    Button* b_onoff;
    Const* v_onoff;
   
    class C_OnOff : public Action
    {
      public:
        C_OnOff(GOnOff* s);
        void action(Component* c);
	
      private:
        GOnOff* src;
    };
   
};

#endif
