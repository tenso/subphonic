#ifndef G_TRUNC_H
#define G_TRUNC_H

#include "g_common.h"

class GTrunc : public SoundComp 
{
  public:
    GTrunc(BitmapFont* f, GroundMixer& g);
    ~GTrunc();
   
    virtual string name() const
    {
        return string("GTrunc");
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

    Trunc* sig;

};

#endif
