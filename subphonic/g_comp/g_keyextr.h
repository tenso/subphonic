#ifndef G_KEYEXTR_H
#define G_KEYEXTR_H

#include "g_common.h"

class GKeyExtr : public SoundComp 
{
  public:
    GKeyExtr(BitmapFont* f, GroundMixer& g);
    ~GKeyExtr();
   
    virtual string name() const
    {
        return string("GKeyExtr");
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

    KeyExtract* sig;

};

#endif
