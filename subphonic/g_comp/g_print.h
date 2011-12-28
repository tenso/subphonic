#ifndef G_PRINT_H
#define G_PRINT_H

#include "g_common.h"
#include "../sig/signal_other.h"

class GPrint : public SoundComp
{
  public :
    GPrint(BitmapFont* f, GroundMixer& g);
    ~GPrint();
   
    virtual string name() const
    {
        return string("GPrint");
    }
   
    Value** getOutput(unsigned int id);

    virtual void addInput(Value** out, unsigned int fromid);
    virtual void remInput(Value** out, unsigned int fromid);
   
  private:
      
    class C_Update : public Action
    {
      public:
        C_Update(GPrint* src);
        void action(Component* c);
	
      private:
        GPrint* src;
    };
   
    KnobSlider* knob;
    PrintValue* sig;
   

};

#endif
