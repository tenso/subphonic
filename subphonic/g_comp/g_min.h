#ifndef G_MIN_H
#define G_MIN_H

#include "g_common.h"

class GMin : public SoundComp 
{
  public:
    GMin(BitmapFont* f, GroundMixer& g);
    ~GMin();
   
    virtual string name() const
    {
        return string("GMin");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
  private:

    Min* sig;

};

#endif
