#ifndef G_CLIP_H
# define G_CLIP_H

#include "g_common.h"
#include "../sig/signal_effect.h"

class GClip : public SoundComp
{
    static const int VALMULT = 5;
 
  public:
   
    GClip(BitmapFont* f, GroundMixer& g);
    ~GClip();
  
    virtual string name() const
    {
        return string("GClip");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
         
    class State
    {
      public:
        State(double mix)
        {
            this->mix=mix;
        }

        double mix;
    };
   
  private:
   
    void update();
    void makeGraph(smp_t* graph, uint len);
   
    class C_Update : public Action
    {
      public:
        C_Update(GClip* d);
	
        void action(Component* co);
      
      private:
        GClip* src;
    };
   
    class C_ResetGain : public Action
    {
      public:
        C_ResetGain(GClip* d);
	
        void action(Component* co);
      
      private:
        GClip* src;
    };
   
    //Slider* mix_slide;
    KnobSlider* gain_knob;
    Button* gain_reset;
   
    //Mixer* mix;
    Gain* depth_dry;
    //Gain* depth_wet;
    //ValueDoppler* doppler;
   
    DataPlot<smp_t>* plot;
    smp_t* graph;
    uint graph_len;
   
    ClipOverdrive* clip_od;
    //Value** input;
};

#endif
