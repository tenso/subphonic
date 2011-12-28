#ifndef G_DIVIDE_H
#define G_DIVIDE_H

#include "g_common.h"

class GDivide : public SoundComp 
{
  public:
    GDivide(BitmapFont* f, GroundMixer& g);
    ~GDivide();
   
    virtual string name() const
    {
        return string("GDivide");
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

    Divide* sig;

};

#endif
