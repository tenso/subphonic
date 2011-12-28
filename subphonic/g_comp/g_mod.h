#ifndef G_MOD_H
#define G_MOD_H

#include "g_common.h"

class GMod : public SoundComp 
{
  public:
    GMod(BitmapFont* f, GroundMixer& g);
    ~GMod();
   
    virtual string name() const
    {
        return string("GMod");
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

    Mod* sig;

};

#endif
