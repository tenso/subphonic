#ifndef G_SAMPLEHOLD_H 
#define G_SAMPLEHOLD_H

#include "g_common.h"
#include "../sig/signal_effect.h"

class GSampleAndHold : public SoundComp
{
  public:
    GSampleAndHold(BitmapFont* f, GroundMixer& g);
    ~GSampleAndHold();
   
    virtual string name() const
    {
        return string("GSampleAndHold");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(double hz, double len)
        {
            this->len=len;
            this->hz=hz;
        }
	
        double len;
        double hz;
    };
   
  private:
    void update();
   
    class C_UPD : public Action
    {
      public:

        C_UPD(GSampleAndHold* s);
        void action(Component* co);
	
      private:
        GSampleAndHold* src;
	
    };
   
    ConvertHz2Per* cvrt;
    Const* hz;
    SampleAndHold* out;
   
    OutputGiver* og_clock;
    OutputGiver* og;
   
    InputTaker* in;
    InputTaker* in_hz;
    InputTaker* in_smp;
   
    KnobSlider* knob_hz;
    KnobSlider* knob_len;
   
    LedsLevel<bool>* led_int;
    LedsLevel<bool>* led_ext;
    bool c_int;
    bool c_ext;
   
    ConvertSec2Len* convert_sec;
};

#endif
