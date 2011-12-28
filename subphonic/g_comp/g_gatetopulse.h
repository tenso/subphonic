#ifndef G_GATETOPULSE_H
#define G_GATETOPULSE_H

#include "g_common.h"

class GGateToPulse : public SoundComp 
{
  public:
    GGateToPulse(BitmapFont* f, GroundMixer& g);
    ~GGateToPulse();
   
    virtual string name() const
    {
        return string("GGateToPulse");
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

    GateToPulse* sig;

};

#endif
