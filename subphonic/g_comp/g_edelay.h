#ifndef G_EDELAY_H
# define G_EDELAY_H

//easy delay

#include "g_common.h"
#include "../sig/signal_effect.h"

class GEDelay : public SoundComp
{
    //static const int DELMULT = 5;
  public:
    GEDelay(BitmapFont* f, GroundMixer& g);
    ~GEDelay();
  
    virtual string name() const
    {
        return string("GEDelay");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(bool bs)
        {
            b_sec=bs;
        }
        ~State()
        {
        }
	
        bool b_sec;
    };
   
  private:
   
    class C_Sec : public Action
    {
      public:
        C_Sec(GEDelay* d);
	
        void action(Component* co);
      
      private:
        GEDelay* src;
    };
   
    Delay_MK2* del;
    Button* sec_button;
};

#endif
