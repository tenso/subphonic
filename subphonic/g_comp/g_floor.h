#ifndef G_FLOOR_H
#define G_FLOOR_H

#include "g_common.h"

class GFloor : public SoundComp 
{
  public:
    GFloor(BitmapFont* f, GroundMixer& g);
    ~GFloor();
   
    virtual string name() const
    {
        return string("GFloor");
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

    Floor* sig;

};

#endif
