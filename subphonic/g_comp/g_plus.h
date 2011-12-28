#ifndef G_PLUS_H
#define G_PLUS_H

#include "g_common.h"

class GPlus : public SoundComp 
{
  public:
    GPlus(BitmapFont* f, GroundMixer& g);
    ~GPlus();
   
    virtual string name() const
    {
        return string("GPlus");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
  private:

    Plus* sig;

};

#endif
