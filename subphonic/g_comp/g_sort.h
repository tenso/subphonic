#ifndef G_SORT_H
#define G_SORT_H

#include "g_common.h"

class GSort : public SoundComp
{
    static const uint NUM = 16;
  public :
    GSort(BitmapFont* f, GroundMixer& g);
    ~GSort();
   
    virtual string name() const
    {
        return string("GSort");
    }
   
    void addInput(Value** out, unsigned int id);
    void remInput(Value** out, unsigned int id);
    Value** getOutput(unsigned int id);
   
    /*
      void getState(SoundCompEvent* e);
      void setState(SoundCompEvent* e);
      
      class State
      {
      public:
      State()
	  {

	  }
      double dummy[2];
      };*/
   
   
  private:
      
    Sort* sort;
};

#endif
