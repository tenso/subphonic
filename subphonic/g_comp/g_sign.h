#ifndef G_SIGN_H
#define G_SIGN_H

#include "g_common.h"

class GSign : public SoundComp 
{
  public:
    GSign(BitmapFont* f, GroundMixer& g);
    ~GSign();
   
    virtual string name() const
    {
        return string("GSign");
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

    Sign* sig;

};

#endif
