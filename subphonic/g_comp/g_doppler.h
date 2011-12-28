#ifndef G_DOPPLER_H
#define G_DOPPLER_H

#include "g_common.h"

class GDoppler : public SoundComp
{
    static const uint NUM=6;
   
  public:
    GDoppler(BitmapFont* f, GroundMixer& g);
    ~GDoppler()
    {
    }
   
    virtual string name() const
    {
        return string("GDoppler");
    }
     
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    /*void getState(SoundCompEvent* e)
      {
      }
   
      void setState(SoundCompEvent* e)
      {
      }
   
   
      class State
      {
      public:
      State()
	  {
	  }
      };*/
   
  private: 
    Value** in;
    ValueDoppler* doppler;
    OutputGiver* og[NUM];
    InputTaker* it;
};

#endif
