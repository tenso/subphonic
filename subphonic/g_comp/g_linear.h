#ifndef G_LINEAR_H
#define G_LINEAR_H

#include "g_common.h"

class GLinear : public SoundComp 
{
  public:
    GLinear(BitmapFont* f, GroundMixer& g);
    ~GLinear();
   
    virtual string name() const
    {
        return string("GLinear");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
/*   void getState(SoundCompEvent* e);
     void setState(SoundCompEvent* e);   
   
     class State
     {
     public:
     State()
     {
     }
     };*/
   
  private:

    Linear* sig;

};

#endif
