#ifndef G_CEIL_H
#define G_CEIL_H

#include "g_common.h"

class GCeil : public SoundComp 
{
  public:
    GCeil(BitmapFont* f, GroundMixer& g);
    ~GCeil();
   
    virtual string name() const
    {
        return string("GCeil");
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

    Ceil* sig;

};

#endif
