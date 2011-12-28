#ifndef G_MINUS_H
#define G_MINUS_H

#include "g_common.h"

class GMinus : public SoundComp 
{
  public:
    GMinus(BitmapFont* f, GroundMixer& g);
    ~GMinus();
   
    virtual string name() const
    {
        return string("GMinus");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
  private:

    Minus* sig;

};

#endif
