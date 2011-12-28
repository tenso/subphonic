#ifndef G_SAMPLERATE_H
#define G_SAMPLERATE_H

//FIXME: if samplerate could change this would not be updated

#include "g_common.h"

class GSampleRate : public SoundComp 
{
  public:
    GSampleRate(BitmapFont* f, GroundMixer& g);
    ~GSampleRate();
   
    virtual string name() const
    {
        return string("GSampleRate");
    }
   
    Value** getOutput(unsigned int id);
   
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

    Const* sig;

};

#endif
