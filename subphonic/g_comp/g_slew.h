#ifndef G_SLEW_H
#define G_SLEW_H

#include "g_common.h"
#include "../sig/signal_effect.h"

class GSlew : public SoundComp 
{
  public:
    GSlew(BitmapFont* f, GroundMixer& g);
    ~GSlew();
   
    virtual string name() const
    {
        return string("GSlew");
    }

    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(double rt, double ft, bool mode)
        {
            this->rt=rt;
            this->ft=ft;
	     
            this->mode = mode;
        }
		
        double rt;
        double ft;
        bool mode;
    };
   
  private:
    void update();
   
    static const int KNOB_MAX=1;
    static const int KNOB_NREVS=2;
   
    class C_UPDATE : public Action
    {
      public:
        C_UPDATE(GSlew* src);
        void action(Component* c);
      
      private:
        GSlew* src;
    };
   
   
    Const* rt_v;
    Const* ft_v;
    KnobSlider* rt_knob;
    KnobSlider* ft_knob;
    Button* slew_mode;
   
    SlewLimitTime* slew_t;
    //SlewLimitLvl* slew_lvl;
};
#endif
