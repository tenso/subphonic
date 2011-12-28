#ifndef G_LESSTHANEQUAL_H
#define G_LESSTHANEQUAL_H

#include "g_common.h"

class GLessThanEqual : public SoundComp 
{
  public:
    GLessThanEqual(BitmapFont* f, GroundMixer& g);
    ~GLessThanEqual();
   
    virtual string name() const
    {
        return string("GLessThanEqual");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
  private:

    LessThanEqual* sig;

};

#endif
