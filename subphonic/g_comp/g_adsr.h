#ifndef G_ADSR_H
#define G_ADSR_H

#include "g_common.h"

//STILL HERE FOR COMPATIBILITY, not maintained
//TO BE REMOVED DONT USE

//FIXME: trigger level not saved

class GAdsr : public SoundComp
{
#define FINE_MAX 0.2 //sec
#define COARSE_MAX 4.8
   
  public :
    GAdsr(BitmapFont* f, GroundMixer& g);
    ~GAdsr();
   
    virtual string name() const
    {
        return string("GAdsr");
    }
   
    Value** getOutput(unsigned int id);
    virtual void addInput(Value** out, unsigned int fromid);
    virtual void remInput(Value** out, unsigned int fromid);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
      
    class State
    {
      public:
        State(double af, double ac, double df, double dc, double rf, 
        double rc, double s, double g, bool usein, bool hrd)
        {
            this->af=af;
            this->df=df;
            this->rf=rf;
            this->ac=ac;
            this->dc=dc;
            this->rc=rc;
	     
            this->s=s;
	     
            this->g=g;
	     
            this->usein=usein;
            this->hrd=hrd;
        }
        ~State()
        {
        }
	
        double ac;
        double dc;
        double rc;
        double af;
        double df;
        double rf;

        double s;
        double g;
	
        bool usein;
        bool hrd;
    };
   
  private:
    void updateVal(int i);
   
    class C_FINE : public Action
    {
      public:
        C_FINE(GAdsr* s, int i);
        void action(Component* c);
	
      private:
        int i;
        GAdsr* src;
    };
   
    class C_COARSE : public Action
    {
      public:
        C_COARSE(GAdsr* s, int i);
        void action(Component* c);
	
      private:
        int i;
        GAdsr* src;
    };
   
    class C_SUST : public Action
    {
      public:
        C_SUST(GAdsr* s);
        void action(Component* c);
      
      private:
        GAdsr* src;
    };
   
    class C_CUTMODE : public Action
    {
      public:
        C_CUTMODE(GAdsr* s);
        void action(Component* co);
	
      private:
        GAdsr* src;
    };
   
    class C_GATELVL : public Action
    {
      public:
        C_GATELVL(GAdsr* s);
        void action(Component* co);
	
      private:
        GAdsr* src;
    };
   
    class C_GEN : public Action
    {
      public:
        C_GEN(GAdsr* s);
        void action(Component* co);
	
      private:
        GAdsr* src;
    };
   
    class C_SUSLEN : public Action
    {
      public:
        C_SUSLEN(GAdsr* s);
        void action(Component* co);
	
      private:
        GAdsr* src;
    };

   
    SmpPoint* att;
    SmpPoint* dec;
    SmpPoint* sus;
    SmpPoint* rel;
   
    //save slider vals
    double ac,af;
    double dc,df;
    double rc,rf;
    double s;
   
    smp_t att_val;
    smp_t dec_val;
    smp_t rel_val;
    smp_t sus_val;

    Button* rb;
    LedsLevel<smp_t>* led;
   
    KnobSlider* sustain;
    KnobSlider* coarse[4]; //waste one
    KnobSlider* fine[4];
   
    KnobSlider* gate_knob;
    InputTaker* it[7];
    OutputGiver* og[2];
   
    Value* out;
   
    AHDSRin* sig;
    SmpPoint* epointer;
   
    AHDSRgen* sig_gen;
    Const* sus_len_val;
    KnobSlider* sus_len_k;
    Button* usein;
};

#endif
