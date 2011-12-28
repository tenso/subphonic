#ifndef G_INTEGRATE_H
#define G_INTEGRATE_H

#include "g_common.h"

class GIntegrate : public SoundComp 
{
  public:
    GIntegrate(BitmapFont* f, GroundMixer& g);
    ~GIntegrate();
   
    virtual string name() const
    {
        return string("GIntegrate");
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

    Integrate* sig;

};

#endif
