#ifndef G_STEPMAT_H
# define G_STEPMAT_H

# include "g_common.h"

/*WARNING: do not use pointers in State*/

static const int MAX_COLUMNS=64;
static const int NUM_COLUMNS=64;
static const int MAX_ROWS=8;
static const int NUM_ROWS=8;

struct RowData
{
    bool data[MAX_COLUMNS];
    uint ncols;
};

class StepRow : public SoundComp
{
  public:
    StepRow(uint ncol);
    ~StepRow();
   
    void update();   
    void step(uint stop);
    uint numCol() const;
   
    uint getIndex();
    void setIndex(uint i);
   
    void setAll(const RowData& s);
    void getAll(RowData& s);
   
    Value** getOut();
   
    int buttonW() const;
    int buttonH() const;
   
    //default: true
    void setUseShot(bool m);
   
    bool isOn();
   
    void clear();
  private:
      
    class Data
    {
      public:
        Data(uint ncol);
        Data(const Data& rh);
        ~Data();
	
        void setAll(const RowData& s);
        void getAll(RowData& s);
		
        void step(uint stop);
        bool isOn();
        void setIndex(uint i);
        uint getIndex();
		
        void set(uint i, bool v);
        bool get(uint i);
			      
        uint numCol() const;
	
        void clear();
	
      private:
        uint index;
        uint ncol;
	
        bool* vec;
      
        Data& operator=(const Data&);
    };
   
   
    class C_COL : public Action
    {
      public:
	
        C_COL(StepRow* s, uint i)
        {
            src=s;
            index=i;
        }
	
        void action(Component* co)
        {
            Button* b = (Button*)co;
            src->vec->set(index, b->isPressed());
        }
	
        StepRow* src;
        uint index;
    };

    Data* vec;
    uint ncol;
   
    Button** buttons;
      
    OneShot* out;
   
};

class GStepMat : public SoundComp
{
  public :
    GStepMat(BitmapFont* f, GroundMixer& g);
   
    ~GStepMat();
   
    virtual string name() const
    {
        return string("GStepMat");
    }
   
    Value** getOutput(unsigned int id);
   
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    int syncToIndex(unsigned int index);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
   
  private:   
    void updateOuts();
   
    class State
    {
      public:
        State(StepRow** rows, uint nrows, uint stopcol, bool gmode, bool endless, bool hold_gate)
        {
            this->nrows=nrows;
            this->stopcol=stopcol;
	     
            for(uint i=0;i<nrows;i++)
            {
                rows[i]->getAll(vec[i]);
            }
	     
            this->gmode=gmode;
            this->endless=endless;
            this->hold_gate=hold_gate;
        }
	
        bool gmode;
        RowData vec[MAX_ROWS];
        uint nrows;
        uint stopcol;
        bool endless;
        bool hold_gate;
    };
   
     
    class MatUpdater : public Value
    {
      public:
        MatUpdater(GStepMat* in);
	
        void setStep(Value** in);
        void setSyncIn(Value** in);
        void setTrig(Value** in);
	
        smp_t nextValue();
	
      private:
        int len;
        GStepMat* src;
	
        Value** step;
        Value** trig;
        Value** sync;
	
        bool resync;
        bool retrig;
        bool restep;
	
        uint sync_wait;
    };
   
      
    class C_STOPCOL : public Action
    {
      public:
	
        C_STOPCOL(GStepMat* s, uint i)
        {
            src=s;
            this->i = i;
        }
	
        void action(Component* comp);
	
      private:
        GStepMat* src;
        uint i;
    };
   
    class C_GMode : public Action
    {
      public:
	
        C_GMode(GStepMat* s)
        {
            src=s;
        }
	
        void action(Component* comp);
	
      private:
        GStepMat* src;
    };
   
    class C_ENDLESS : public Action
    {
      public:
	
        C_ENDLESS(GStepMat* s)
        {
            src=s;
        }
	
        void action(Component* comp);
	
      private:
        GStepMat* src;
    };
   
    class C_HOLDGATE : public Action
    {
      public:
	
        C_HOLDGATE(GStepMat* s)
        {
            src=s;
        }
	
        void action(Component* comp);
	
      private:
        GStepMat* src;
		
    };
   
    class C_Clear : public Action
    {
      public:
	
        C_Clear(GStepMat* s)
        {
            src=s;
        }
	
        void action(Component* comp);
	
      private:
        GStepMat* src;
		
    };

   
    Button* gate_mode;
    Button** b_stopcol;
   
    OneShot* patch;
   
    LedsIndex<int>* leds;
    StepRow** rows;
   
    uint stopcol;
    uint nrows;
    int col; //used for leds
    uint ncols;
   
    InputTaker* in;
    InputTaker* sync_in;
    OutputGiver* og;
   
    OutputGiver* row_givers[NUM_ROWS];
   
    MatUpdater* drain;
   
    Button* b_endless;
    bool endless;
    bool run;
    bool hold_gate;
    Button* b_holdgate;
    bool col_wait;
    LedsLevel<bool>* col_wait_led;
};

#endif
