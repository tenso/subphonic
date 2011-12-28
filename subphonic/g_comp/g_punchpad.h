#ifndef G_PUNCHPAD_H
#define G_PUNCHPAD_H

#include "g_common.h"

#define MAX_COLS 64

#define NKEYS 12
#define NOCTS 9

//#define KEYSTR "A\nA#\nB\nC\nC#\nD\nD#\nE\nF\nF#\nG\nG#\0"

class ColState
{
  public:
    int key; //-1 is off
    short oct;
};

class GPunchPad;


class PunchCol : public SoundComp
{
  public:
    PunchCol(BitmapFont* f, GroundMixer& g);
    ~PunchCol();
   
    void setMaster(GPunchPad* m);
   
    smp_t getHz();
   
    void setOctOff(short o);
    short getOctOff();
   
    bool isOn();
   
    void getState(ColState& s);
    void setState(const ColState& s);
   
    //must do manual update of 'master'
    void transpose(int n);
   
    //FIXME: add
    /*void setOn(bool val);
      bool getOn();*/
   
    void setKey(int k); //0 is off
    void setOctave(short o);
   
  private:
   
    void setHz();
   
    class Update : public Action
    {
      public:
	
        Update(PunchCol* s);
	
        void action(Component* comp);
	
        PunchCol* src;
		
    };
      
    class R_A : public Action
    {
      public:

        R_A(PunchCol* s, int i);
        void action(Component* co);
	
      private:
        PunchCol* src;
        int index;
    };

      
    GPunchPad* master;
    smp_t hz;
    int n;
    Button** buttons;
    int key;
    short octave;
    short oct_off;
    NumberSelect<short>* ns;
    BitmapFont* font;
};



class GPunchPad : public SoundComp
{
  public :
    GPunchPad(BitmapFont* f, GroundMixer& g);
    ~GPunchPad();
   
    virtual string name() const
    {
        return string("GPunchPad");
    }
   
    Value** getOutput(unsigned int id);
   
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    bool lastCol();
      
    int syncToIndex(unsigned int index);
      
    void updateOuts();
   
    /*state*/
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
  private:
   
    //void step();
   
    class State
    {
      public:
        State(PunchCol** b, uint n, short oct, bool endless, bool hold_gate, uint stopcol)
        {
            this->hold_gate=hold_gate;
            this->endless=endless;
            this->oct=oct;
            this->n=n;
            this->stopcol=stopcol;
	     
            for(uint i=0;i<n;i++)
            {
                b[i]->getState(data[i]);
            }
        }
        ~State()
        {
        }
	
        ColState data[MAX_COLS];
        uint n;
	
        short oct;
        bool endless;
        bool hold_gate;
	
        uint stopcol;
    };
   
    class PadUpdater : public Value
    {
      public:
	
        PadUpdater(GPunchPad* in);
	
        void setRecGate(Value** in);
        void setRecHz(Value** in);
	
        void setStep(Value** in);
	
        //only on when !src->endless
        void setTrig(Value** in);
	
        //syncs on rising
        void setSyncIn(Value** in);
	
        smp_t nextValue();
	
      private:
        GPunchPad* src;
	
        Value** trig;
        Value** sync;
        Value** step;
        bool restep;
        bool resync;
        bool retrig;
        int sync_wait;
	
        Value** rec_hz;
        Value** rec_gate;
        smp_t last_rec_hz;
        int last_rec_col;
    };
   
    class C_OCTAVE : public Action
    {
      public:
	
        C_OCTAVE(GPunchPad* s)
        {
            src=s;
        }
	
        void action(Component* comp);
	
      private:
        GPunchPad* src;
		
    };
   
    class C_ENDLESS : public Action
    {
      public:
	
        C_ENDLESS(GPunchPad* s)
        {
            src=s;
        }
	
        void action(Component* comp);
	
      private:
        GPunchPad* src;
    };
   
    class C_HOLDGATE : public Action
    {
      public:
	
        C_HOLDGATE(GPunchPad* s)
        {
            src=s;
        }
	
        void action(Component* comp);
	
      private:
        GPunchPad* src;
		
    };
   
    class C_STOPCOL : public Action
    {
      public:
	
        C_STOPCOL(GPunchPad* s, uint i)
        {
            src=s;
            this->i = i;
        }
	
        void action(Component* comp);
	
      private:
        GPunchPad* src;
        uint i;
    };
   
    class C_Transpose : public Action
    {
      public:
	
        C_Transpose(GPunchPad* s, int m)
        {
            src=s;
            mode=m;
        }
	
        void action(Component* comp);
	
      private:
        GPunchPad* src;
        int mode;
    };
   
    class C_Clear : public Action
    {
      public:
	
        C_Clear(GPunchPad* s)
        {
            src=s;
        }
	
        void action(Component* comp);
	
      private:
        GPunchPad* src;
		
    };

   
    static char key_str[12][3];
   
    BitmapFont* font;
    Pixmap* back_pix;
    Label** l;
    Label* text;
    NumberSelect<short>* ns;
    LedsIndex<int>* leds;
   
    Button* b_endless;
    bool endless;
    bool run;
   
    bool hold_gate;
    Button* b_holdgate;
   
    PunchCol** cols;
   
    int n;
    int col;
    uint ncols;
    uint stopcol;
    short octave;
   
    InputTaker* in;
    InputTaker* sync_in;
    OutputGiver* og[3];
      
    OneShot* patch;
    PadUpdater* updater;
    Const* keyout;
    Const* gate;
   
    Button** b_stopcol;
   
    bool col_wait;
    LedsLevel<bool>* col_wait_led;
};

#endif
