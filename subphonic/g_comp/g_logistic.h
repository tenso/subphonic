#ifndef G_LOGISTIC_H
#define G_LOGISTIC_H

#include "g_common.h"

class GLogisticMap : public SoundComp
{
  public :
    GLogisticMap(BitmapFont* f, GroundMixer& g);
    ~GLogisticMap();
   
    virtual string name() const
    {
        return string("GLogisticMap");
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
      
    class C_Reset : public Action
    {
      public:
        C_Reset(GLogisticMap* src);
        void action(Component* c);
	
      private:
        GLogisticMap* src;
    };
   
    Button* b_reset;
   
    LogisticMap* lmap;
   
    /*KnobSlider* r_knob;
      KnobSlider* seed_knob;*/
};

#endif
