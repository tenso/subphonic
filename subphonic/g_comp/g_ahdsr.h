#ifndef G_AHDSR_H
#define G_AHDSR_H

#include "g_common.h"

//FIXME: trigger level not saved

class GAhdsr : public SoundComp
{
#define FINE_MAX 0.2 //sec
#define COARSE_MAX 4.8
   
  public :
    GAhdsr(BitmapFont* f, GroundMixer& g);
    ~GAhdsr();
   
    virtual string name() const
    {
        return string("GAhdsr");
    }
   
    Value** getOutput(unsigned int id);
    virtual void addInput(Value** out, unsigned int fromid);
    virtual void remInput(Value** out, unsigned int fromid);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
      
    int syncToIndex(unsigned int index);
   
    class State
    {
      public:
        State(double af, double ac, double hf, double hc, double df, double dc, double rf, 
        double rc, double s, double g, bool usein, bool hrd, double sus_len, bool sync_reset)
        {
            this->af=af;
            this->hf=hf;
            this->df=df;
            this->rf=rf;
            this->ac=ac;
            this->hc=hc;
            this->dc=dc;
            this->rc=rc;
	     
            this->s=s;
	     
            this->g=g;
	     
            this->usein=usein;
            this->hrd=hrd;
	     
            this->sus_len=sus_len;
	     
            this->sync_reset=sync_reset;
        }
        ~State()
        {
        }
	
        double ac;
        double hc;
        double dc;
        double rc;
        double af;
        double hf;
        double df;
        double rf;

        double s;
        double g;
	
        bool usein;
        bool hrd;
	
        double sus_len;
	
        bool sync_reset;
    };
   
  private:
    void updateVal(int i);
   
    class C_FINE : public Action
    {
      public:
        C_FINE(GAhdsr* s, int i);
        void action(Component* c);
	
      private:
        int i;
        GAhdsr* src;
    };
   
    class C_COARSE : public Action
    {
      public:
        C_COARSE(GAhdsr* s, int i);
        void action(Component* c);
	
      private:
        int i;
        GAhdsr* src;
    };
   
    class C_SUST : public Action
    {
      public:
        C_SUST(GAhdsr* s);
        void action(Component* c);
      
      private:
        GAhdsr* src;
    };
   
    class C_CUTMODE : public Action
    {
      public:
        C_CUTMODE(GAhdsr* s);
        void action(Component* co);
	
      private:
        GAhdsr* src;
    };
   
    class C_GATELVL : public Action
    {
      public:
        C_GATELVL(GAhdsr* s);
        void action(Component* co);
	
      private:
        GAhdsr* src;
    };
   
    class C_GEN : public Action
    {
      public:
        C_GEN(GAhdsr* s);
        void action(Component* co);
	
      private:
        GAhdsr* src;
    };
   
    class C_SUSLEN : public Action
    {
      public:
        C_SUSLEN(GAhdsr* s);
        void action(Component* co);
	
      private:
        GAhdsr* src;
    };
   
  
    SmpPoint* att;
    SmpPoint* hold;
    SmpPoint* dec;
    SmpPoint* sus;
    SmpPoint* rel;
   
    //save slider vals
    double ac,af;
    double hc,hf;
    double dc,df;
    double rc,rf;
    double s;
   
    smp_t att_val;
    smp_t hold_val;
    smp_t dec_val;
    smp_t rel_val;
    smp_t sus_val;

    Button* rb;
    LedsLevel<smp_t>* led;
   
    KnobSlider* sustain;
    KnobSlider* coarse[5]; //waste one
    KnobSlider* fine[5];
   
    KnobSlider* gate_knob;
    InputTaker* it[8];
    OutputGiver* og[2];
   
    Value* out;
   
    AHDSRin* sig;
    SmpPoint* epointer;
   
    AHDSRgen* sig_gen;
    Const* sus_len_val;
    KnobSlider* sus_len_k;
    Button* usein;
   
    bool dummy_fix; // to give led when using in not gen
    LedsLevel<bool>* fix_run_led;
   
    Button* sync_reset;
};

#endif
