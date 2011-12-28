#ifndef G_SIN_H
#define G_SIN_H

#include "g_common.h"

class GSin : public SoundComp 
{
  public:
    GSin(BitmapFont* f, GroundMixer& g);
    ~GSin();
   
    virtual string name() const
    {
        return string("GSin");
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

    Sin* sig;

};

#endif
