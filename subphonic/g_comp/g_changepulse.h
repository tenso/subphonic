#ifndef G_CHANGEPULSE_H
#define G_CHANGEPULSE_H

#include "g_common.h"

class GChangePulse : public SoundComp 
{
  public:
    GChangePulse(BitmapFont* f, GroundMixer& g);
    ~GChangePulse();
   
    virtual string name() const
    {
        return string("GChangePulse");
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

    ChangePulse* sig;

};

#endif
