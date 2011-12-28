#ifndef G_TRACKER_H
# define G_TRACKER_H

# include "g_common.h"

# include "tracker_base.h"

#include "../screen/input.h"

class GTracker : public SoundComp
{
    static const uint MAX_CHAN=16; //REALLY ENOUGH?
    static const uint TRACKER_PH=700;
    static const uint TRACKER_PW=740;
    static const uint TRACKER_N_ROW_VIS=32;
    static const uint TRACKER_N_TRACKS_VIS=5;
   
  public :
    GTracker(BitmapFont* f, GroundMixer& g);
    ~GTracker();
   
    virtual string name() const
    {
        return string("GTracker");
    }
   
    Value** getOutput(unsigned int id);
   
    virtual void addInput(Value** out, unsigned int fromid);
    virtual void remInput(Value** out, unsigned int fromid);
   
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e); //this can set any size T-E (thatStateHead demands)
   
    //gives to tracker base
    virtual bool giveInput(spl::Input& in);
   
    int syncToIndex(uint index);
   
  private:
   
    void updateTracker();
   
    //because variable state size is needed this acts as a head
    //actual tracker entrys are appended after this in state stream (SoundCompEvent->data)
    class StateHead
    {
      public:
        int head_size;
	
        int ch;
        bool ch_on;
        int octave;
        int plen;
        int ntracks;
	
        bool edit;
        bool play_on;
	
        char unused2[3];
	
        uint num_gate_mode;
        uint total_size;
        int add_step;
        int del_step;
	
        int unused[2];
	
        //tracker data; total bytes: entry_size*nentry
        int entry_size; //expected size(bytes) (error handling, etc)
        int nentry; 
	
        //nentry * 'TrackerEntry' appended after this
	
	
        //num_gate_mode * bool appended
    };
      
   
    //returns 0, add to ground
    class Drain : public Value 
    {
      public:
        Drain(GTracker* src);
        ~Drain();
	
        void setRecGate(Value** v);
        void setRecHz(Value** v);
        void setRecCtrl(Value** v);
	
        //steps on rising positive edge
        void setStep(Value** v);
	
        void setSync(Value** v); //restarts pattern at start
        Value** getSync(); //signals end of pattern
	
        //channel corresponds to tracker's
        Value** getCtrl(uint chan);
        Value** getNote(uint chan); //FIXME: name should be Hz
        Value** getGate(uint chan); //on as long as note is "pressed"
	
        //sustain or one_shot
        //def: sust=true
        void setGateMode(uint chan, bool sust);
	
        smp_t nextValue();
	
        //default: 1
        void setDir(int d);//+1,-1
        int getDir();
	
      private:
        GTracker* src;
	
        Value** i_step;
        Value** i_sync;
	
        OneShot*  o_sync;
	
        Const* o_ctrl[MAX_CHAN];
        Const* o_note[MAX_CHAN];
        Gate*  o_gate[MAX_CHAN];
	
        bool resync;
        bool restep;
	
        int dir;
	
        int sync_wait;
	
        uint preview_gate_ch;
        bool preview_gate;
	
        Value** rec_hz;
        Value** rec_gate;
        Value** rec_ctrl;
        smp_t last_rec_hz;
        smp_t last_rec_ctrl;
        uint last_rec_row;
	
        int rec_ch;
    };
   
   
    class C_UTRACKER : public Action
    {
      public:
        C_UTRACKER(GTracker* src);
	
        void action(Component* c);
	
      private:
        GTracker* src;
    };
   
    class C_MulPLen : public Action
    {
      public:
        C_MulPLen(GTracker* src);
	
        void action(Component* c);
	
      private:
        GTracker* src;
    };
    class C_DivPLen : public Action
    {
      public:
        C_DivPLen(GTracker* src);
	
        void action(Component* c);
	
      private:
        GTracker* src;
    };
   
    class C_AddStep : public Action
    {
      public:
        C_AddStep(GTracker* src);
	
        void action(Component* c);
	
      private:
        GTracker* src;
    };
   
    class C_DelStep : public Action
    {
      public:
        C_DelStep(GTracker* src);
	
        void action(Component* c);
	
      private:
        GTracker* src;
    };
   
    class C_GateMode : public Action
    {
      public:
        C_GateMode(GTracker* src, uint i);
	
        void action(Component* c);
	
      private:
        GTracker* src;
        uint i;
    };
   
    class C_Edit : public Action
    {
      public:
        C_Edit(GTracker* src);
	
        void action(Component* c);
	
      private:
        GTracker* src;
    };
   
    class C_Follow : public Action
    {
      public:
        C_Follow(GTracker* src);
	
        void action(Component* c);
	
      private:
        GTracker* src;
    };
   
    class C_Tran : public Action
    {
      public:
        C_Tran(GTracker* src, int dir, bool all);
	
        void action(Component* c);
	
      private:
        GTracker* src;
        int dir;
        bool all;
    };
   
    class C_Clear : public Action
    {
      public:
        C_Clear(GTracker* src, bool all);
	
        void action(Component* c);
	
      private:
        GTracker* src;
        bool all;
    };
   
    class C_Preview : public Action
    {
      public:
        C_Preview(GTracker* src);
	
        void action(Component* c);
	
      private:
        GTracker* src;
    };
   
    Tracker* tracker;
   
    Drain* drain;
   
    Button* b_ch;
    Button* b_edit;
    Button* b_follow;
   
    NumberSelect<int>* s_plen;
    NumberSelect<int>* s_ntracks;
    NumberSelect<int>* s_oct;
    NumberSelect<int>* s_ch;
   
    LedsLevel<bool>* led_on;
   
    Button* b_gate_mode[MAX_CHAN];
   
    int ch;
    int octave;
    int plen;
    int ntracks;
   
    NumberSelect<int>* ns_addstep;
    NumberSelect<int>* ns_delstep;
   
    int add_step;
    int del_step;
   
    bool markers_follow;
    bool do_preview;
    /*Label* dd;
      int ndd;*/
};

#endif
