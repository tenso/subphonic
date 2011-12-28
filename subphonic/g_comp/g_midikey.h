#ifndef G_MIDIKEY_H
# define G_MIDIKEY_H

# include "g_common.h"

//FIXME: MOVE Note+PolyNote

class Note
{
  public:
    Note();
   
    int n; //key number
    bool  gate_on;
    smp_t hz;
    smp_t vel;
   
    //-1 for never used
    int rel_index; //recycle keys with first released, first recycled
    int on_index; //for override
};


class PolyNote
{
  public:
    enum OVERRIDE {OFF=0, OLDEST, NEWEST};
    enum RECYCLE {FIRST_RELEASE=0, MIN_FREE_INDEX};
	
    //inititaly num=max
    PolyNote(int max); 
    ~PolyNote();
   
    //'n' is key number
    //if key is overidden there is no new gate
    bool noteOn(int n, smp_t hz, smp_t vel, OVERRIDE override=OFF, RECYCLE recycle=FIRST_RELEASE);
    bool retrigNote(int n);
    bool noteOff(int n);
   
    void allNotesOff();
   
    unsigned int maxNotes();
   
    //number of possible < 'max'
    unsigned int getNumNotes();
    void setNumNotes(unsigned int n);
   
    Gate** getGate(unsigned int n);
    Value** getHz(unsigned int n);
    Value** getVel(unsigned int n);
    Value** getKeyNum(unsigned int n);
   
  private:
   
    //overrides notes
    void readyNote(uint index, int n, smp_t hz, smp_t vel);
   
    Gate** gate;
    Const** hz;
    Const** vel;
    Const** key_num;
   
    int rel_count;
    unsigned int max;
    unsigned int num;
    Note* notes;
    //FIXME: what happes when count overflows?
    int on_count;
};

class GMIDIkey : public SoundComp
{
    static const int NUMPOLY=16; //FIXME: dynamic?
    static const int MAXPOLY=128;
   
  public :
    GMIDIkey(BitmapFont* f, GroundMixer& g);
    ~GMIDIkey();
   
    virtual string name() const
    {
        return string("GMIDIkey");
    }
   
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    Value** getOutput(unsigned int id);
   
    bool retrigNote(int n);
    bool noteOn(int n, smp_t vel);
    bool noteOff(int n);
    void setPitchwheel(smp_t val);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(int n, PolyNote::OVERRIDE override_mode, PolyNote::RECYCLE recycle_mode)
        {
            this->n=n;
            this->override_mode=override_mode;
            this->recycle_mode=recycle_mode;
        }
	
        int n;
        PolyNote::OVERRIDE override_mode;
        PolyNote::RECYCLE recycle_mode;
    };
   
    //name= hz,gate,vel,pwheel
    int outputGiverId(const string& name, uint port);
    int inputTakerId(const string& name, uint port);
   
    void setNPolyUsed(uint n);
   
  private:
   
    class C_POLYUSED : public Action
    {
      public:
        C_POLYUSED(GMIDIkey* src);
        void action(Component* c);
      private:
	
        GMIDIkey* src;
    };
   
    class C_Alloff : public Action
    {
      public:
        C_Alloff(GMIDIkey* src);
        void action(Component* c);
      private:
	
        GMIDIkey* src;
    };
   
    class C_Override : public Action
    {
      public:
        C_Override(GMIDIkey* src);
        void action(Component* c);
      private:
	
        GMIDIkey* src;
    };
   
    class C_Recycle : public Action
    {
      public:
        C_Recycle(GMIDIkey* src);
        void action(Component* c);
      private:
	
        GMIDIkey* src;
    };
      
    /*Drain: for sustain only*/
   
    class Drain : public Value
    {
      public:
        Drain(GMIDIkey* src);
	
        void setSustain(Value** sus);
	
        smp_t nextValue();
		
	
      private:
        GMIDIkey* src;
        Value** sus;
    };
   
   
    static char key_str[12][3]; //12*2 bytes saved
   
    static const unsigned int ID_PW = 0;
    static const unsigned int ID_HZ_OFF = 1;
    static const unsigned int ID_GATE_OFF = ID_HZ_OFF+MAXPOLY;
    static const unsigned int ID_VEL_OFF = ID_GATE_OFF+MAXPOLY;
    static const unsigned int ID_KEYNUM_OFF = ID_VEL_OFF+MAXPOLY;
   
    InputTaker* it_sust;
   
    OutputGiver* og_pw;
    OutputGiver* og_hz[MAXPOLY];
    OutputGiver* og_gate[MAXPOLY];
    OutputGiver* og_vel[MAXPOLY];
    OutputGiver* og_keynum[MAXPOLY];
   

    SmpPoint* pitchwheel;
   
    short octave;
    smp_t pwheel;
   
    NumberSelect<double>* ns0;
    Label* l;
   
    PolyNote notes;
   
    NumberSelect<int>* ns_poly;
    int poly_used;
   
    Label* l_lpress;
   
    list<int> sustain_list;
    bool sustain_on;
   
    Drain* drain;
   
    NButton* b_override;
    PolyNote::OVERRIDE override_mode;
   
    Button* b_recycle;
    PolyNote::RECYCLE recycle_mode;
};

#endif
