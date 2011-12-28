#ifndef G_CROSSFADE_H
# define G_CROSSFADE_H

#include "g_common.h"
#include "../sig/signal.h"

class GCrossfade : public SoundComp
{
  public:
    GCrossfade(BitmapFont* f, GroundMixer& g);
    ~GCrossfade();
  
    virtual string name() const
    {
        return string("GCrossfade");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);

    class State
    {
      public:
        State(double c_slide, bool stereo)
        {
            this->c_slide=c_slide;
            this->stereo=stereo;
        }
	
        double c_slide;
        bool stereo;
    };
   
  private:
   
    class C_CROSS : public Action
    {
      public:
        C_CROSS(GCrossfade* d);
	
        void action(Component* co);
      
      private:
        GCrossfade* src;
    };
   
    class C_STEREO : public Action
    {
      public:
        C_STEREO(GCrossfade* d);
	
        void action(Component* co);
      
      private:
        GCrossfade* src;
    };
   
    Slider* c_slide;
    Button* stereo;
    Const* f_val;
   
    Crossfade* cross;
   
};

#endif
