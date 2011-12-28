#ifndef G_WAV_H
#define G_WAV_H

#include <sys/types.h>
#include <dirent.h>

#include <vector>
#include <string>

#include "g_common.h"
#include "../sig/wav.h"
#include "../util/file_io.h"
   
//holds one sample value
class WavSample
{
  public:
    WavSample(const string& n, SampleValue* s);
    WavSample(const string& n, SampleValue* left, SampleValue* right);
   
    //deletes all SampleValue(i.e also Sample)
    ~WavSample();
   
    void setGateMode(SampleValue::GATE_MODE mode);
   
    bool stereo;
    string name;
    SampleValue* samp[2];
    SampleValue::GATE_MODE g_mode;
};

class SampleHold
{
  public:
    SampleHold();
    ~SampleHold();
    int loadDir(const string& dir);
    int singleZeroSample();
    int getNum() const;
    WavSample* getSample(unsigned int i);
    WavSample* operator[](unsigned int i);
   
    void setGateMode(SampleValue::GATE_MODE mode);
   
  private:
    vector<WavSample*> samples;
};

//FIXME: stereo is defunct: all inuts are shared without dopplers
class GWav : public SoundComp
{
  public:
    GWav(BitmapFont* f, GroundMixer& g);
    virtual ~GWav();
   
    //IF this has not been run DO NOT use this class, probably segfault...
    void loadPath(const string& path);
   
    //if setAppendix()
    void loadByAppendix(const string& prefix=string(""));
   
    virtual string name() const
    {
        return string("GWav");
    }
   
    Value** getOutput(unsigned int id);
    virtual void addInput(Value** out, unsigned int fromid);
    virtual void remInput(Value** out, unsigned int fromid);
      
    DiskComponent getDiskComponent();
    void setAppendix(const Uint8* data, unsigned int len);
   
    //returns what loadPath() got
    string getPathName() const
    {
        return pathname;
    }
   
      
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
      
    class State
    {
      public:
        State(double speed, bool gmode)
        {
            this->speed=speed;
            this->gmode=gmode;
        }
	
        double speed;
        bool gmode;
	
        double unused[10];
	
    };
   
    int syncToIndex(unsigned int index);
   
  private:
   
    //updates sample to what 'curr' is
    void updateCurrSamp();
   
    //speed slide
    class C_Len : public Action
    {
      public:
        C_Len(GWav* c);
        void action(Component* co);
	
      private:
        GWav* src;

    };
   
    //reset speed slide
    class R_RESET : public Action
    {
      public:
        R_RESET(GWav* s);
        void action(Component* co);
	
      private:
        GWav* src;
    };
   
    //for changeing, sample
    class C_CHANGE : public Action
    {
      public:
        C_CHANGE(GWav* s, bool d)
        {
            dir=d;
            src=s;
        }
        void action(Component* co);
	
        bool dir;
        GWav* src;
    };
   
    class C_ResetPlay : public Action
    {
      public:
        C_ResetPlay(GWav* s);
        void action(Component* co);
	
      private:
        GWav* src;
    };
   
    class C_PlayPause : public Action
    {
      public:
        C_PlayPause(GWav* s);
        void action(Component* co);
	
      private:
        GWav* src;
    };
   
    class C_Loop : public Action
    {
      public:
        C_Loop(GWav* s);
        void action(Component* co);
	
      private:
        GWav* src;
    };
   
    class C_GMode : public Action
    {
      public:
        C_GMode(GWav* s);
        void action(Component* co);
	
      private:
        GWav* src;
    };
   
    bool preview;
    string pathname;
    bool loaded;
    bool curr_is_stereo;
   
    const smp_t* trigval;
   
   
    Const* speed;
      
    SampleHold samples;
    int nsamples;
    int curr;
    SampleValue* current[2];//left/right
   
    DataPlot<short>* pd;

    OutputGiver* og[2];
    Slider* len_slide;
    Label* l;
    LedsLevel<smp_t>* leds;
    LedsLevel<bool>* stereo_led;
    LedsLevel<bool>* playing_led;
   
    Button* b_loop;
    Button* b_play;
    Button* b_gmode;
    smp_t zero;
   
    string sel_name;
    bool do_sel_name;
    bool have_app;
    ValueDoppler* gate_doppler;
    ValueDoppler* stop_doppler;
    ValueDoppler* pos_doppler;
    ValueDoppler* speed_doppler;
    ValueDoppler* start_i_doppler;
    ValueDoppler* stop_i_doppler;
   
    Const* smp_len;
   
    Value** c_pos;
    Value** c_start_i;
    Value** c_stop_i;
};

#endif
