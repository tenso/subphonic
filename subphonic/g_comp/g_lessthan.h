#ifndef G_LESSTHAN_H
#define G_LESSTHAN_H

#include "g_common.h"

class GLessThan : public SoundComp 
{
  public:
    GLessThan(BitmapFont* f, GroundMixer& g);
    ~GLessThan();
   
    virtual string name() const
    {
        return string("GLessThan");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
  private:

    LessThan* sig;

};

#endif
