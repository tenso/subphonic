#ifndef G_SCOPE_H
#define G_SCOPE_H

#include "g_common.h"
#include "../sig/signal_other.h"

static const float BOOST=10.0f;

class GScope : public SoundComp
{
  public:
    GScope(BitmapFont* f, GroundMixer& g);
    ~GScope()
    {
    }
      
    virtual string name() const
    {
        return string("GScope");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
  private:
   
    class C_Amp : public Action
    {
      public:

        C_Amp(GScope* src);
        void action(Component* co);
	
        GScope* src;
    };
   
    DataPlotRing<smp_t>* view;
    ValueScope* sig;
    OutputGiver* og;
    InputTaker* it;
};

#endif
