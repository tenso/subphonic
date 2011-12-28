#ifndef G_MAX_H
#define G_MAX_H

#include "g_common.h"

class GMax : public SoundComp 
{
  public:
    GMax(BitmapFont* f, GroundMixer& g);
    ~GMax();
   
    virtual string name() const
    {
        return string("GMax");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
  private:

    Max* sig;

};

#endif
