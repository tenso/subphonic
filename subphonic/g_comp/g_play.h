#ifndef G_PLAY_H
#define G_PLAY_H

#include "g_common.h"
#include "../sig/signal_other.h"

class GPlay : public SoundComp
{
  public:
   
    //default is mono, use setNChan for stereo
    GPlay(BitmapFont* f, GroundMixer& g);
    ~GPlay();
   
    virtual string name() const
    {
        return string("GPlay");
    }
   
    void setNChan(int chans);
    bool isOn();
   
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    //only used by m_main
    Value** getOutput();
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(bool stereo)
        {
            this->stereo=stereo;
        }
	
        bool stereo;
    };
   
  private:
    class C_STEREO : public Action
    {
      public:
        C_STEREO(GPlay* s);
	
        void action(Component* co);
	
      private:
        GPlay* src;
    };
   
    bool mix;
    Button* rb;
    InputTaker* in[2];
    bool on;
    Stereo* sig;
    int len;
    bool aplay;
   
    PeakMeter* meters[2]; //one for each channel
    LedsLevel<smp_t>* meter_leds[2];
};

#endif
