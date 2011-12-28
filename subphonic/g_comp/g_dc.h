#ifndef G_DC_H
#define G_DC_H

#include "g_common.h"

class GDc : public SoundComp
{
    static const uint NUM_WHOLE = 5;
    static const uint NUM_FRAC = 5;
  public :
    GDc(BitmapFont* f, GroundMixer& g);
    ~GDc();
   
    virtual string name() const
    {
        return string("GDc");
    }
   
    void updateOut();
    void updateLimits();
   
    Value** getOutput(unsigned int id);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
      
    class State
    {
      public:
        State(double knob, double val_left, double val_right)
        {
            this->knob=knob;
            this->val_left=val_left;
            this->val_right=val_right;
        }
	
        double knob;
        double val_left; //former val
        double val_right;
	
        double unused[1];
        bool neg; //usnused in ver>3
    };
   
   
  private:
      
    class C_UpdateLimits : public Action
    {
      public:
        C_UpdateLimits(GDc* src);
        void action(Component* c);
	
      private:
        GDc* src;
    };
   
    class C_UpdateOut : public Action
    {
      public:
        C_UpdateOut(GDc* src);
        void action(Component* c);
	
      private:
        GDc* src;
    };
   
    //Button* b_neg;
   
    Label* l_valshow;
   
    Const* sig_out;
   
    NumberSelect<smp_t>* ns_l;
    smp_t val_left;
   
    NumberSelect<smp_t>* ns_r;
    smp_t val_right;
   
    KnobSlider* knob;
};

#endif
