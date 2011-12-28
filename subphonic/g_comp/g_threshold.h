#ifndef G_THREASHOLD_H
#define G_THREASHOLD_H

#include "g_common.h"

class GThreshold : public SoundComp
{
  public :
    GThreshold(BitmapFont* f, GroundMixer& g);
    ~GThreshold();
   
    virtual string name() const
    {
        return string("GThreshold");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int id);
    void remInput(Value** out, unsigned int id);

   
    /*
      void getState(SoundCompEvent* e);
      void setState(SoundCompEvent* e);
      
      class State
      {
      public:
      State(smp_t v, double k, bool neg)
	  {
      knob=k;
      val=v;
      this->neg=neg;
	  }
	
      double knob;
      smp_t val;
      double unused[2];
      bool neg;
      };
    */
   
  private:
   
    /*
      class C_Reset : public Action
      {
      public:
      C_Reset(GThreshold* src);
      void action(Component* c);
	
      private:
      GThreshold* src;
      };
   
      Button* b_reset;*/
   
    Threshold* th;
   
    /*KnobSlider* r_knob;
      KnobSlider* seed_knob;*/
};

#endif
