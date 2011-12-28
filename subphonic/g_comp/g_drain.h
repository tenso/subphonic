#ifndef G_DRAIN_H
#define G_DRAIN_H

#include "g_common.h"

class GDrain : public SoundComp
{
  public :
    GDrain(BitmapFont* f, GroundMixer& g);
    ~GDrain();
   
    virtual string name() const
    {
        return string("GDrain");
    }
   
    Value** getOutput(unsigned int id);

    virtual void addInput(Value** out, unsigned int fromid);
    virtual void remInput(Value** out, unsigned int fromid);
   
  private:
    ValueDoppler* dop;
   

};

#endif
