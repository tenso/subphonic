#ifndef G_MULT_H
#define G_MULT_H

#include "g_common.h"

class GMult : public SoundComp 
{
  public:
    GMult(BitmapFont* f, GroundMixer& g);
    ~GMult();
   
    virtual string name() const
    {
        return string("GMult");
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

    Mult* sig;

};

#endif
