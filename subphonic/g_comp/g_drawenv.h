#ifndef G_DRAWENV_H
#define G_DRAWENV_H

#include "g_common.h"
#include "../sig/signal_effect.h"
#include "g_help.h"

class GDrawEnv : public SoundComp
{
    static const uint MAX_WAYP=24;
    static const uint MAX_INDEX=44100;
   
  public :
    GDrawEnv(BitmapFont* f, GroundMixer& g);
    ~GDrawEnv();
   
        
    virtual string name() const
    {
        return string("GDrawEnv");
    }
   
   
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
    Value** getOutput(unsigned int id);
  
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(const vector<SelectPlane::Coord>& coord, smp_t sust, bool gate,
        bool holdend, bool fincyc)
        {
            DASSERT(coord.size()<=MAX_WAYP);
            b_gate=gate;
            this->sust=sust;
            n_wp=coord.size();
            for(unsigned int i=0;i<n_wp;i++)data[i]=coord[i];
	     
            this->holdend=holdend;
            this->fincyc=fincyc;
        }
	
        bool b_gate;
        smp_t sust;
        unsigned int n_wp;
        SelectPlane::Coord data[MAX_WAYP]; //not memory efficient but..
	
        bool fincyc;
        bool holdend;
	
    };
   
   
  private:
      
    class C_UpdatePAD : public Action
    {
      public:
        C_UpdatePAD(GDrawEnv* src)
        {
            this->src=src;
        }
        void action(Component* co);
	
        GDrawEnv* src;
    };
   
    class C_RemSel : public Action
    {
      public:
        C_RemSel(GDrawEnv* src)
        {
            this->src=src;
        }
        void action(Component* co);
	
        GDrawEnv* src;
    };
   
    class C_GateOn : public Action
    {
      public:
        C_GateOn(GDrawEnv* src)
        {
            this->src=src;
        }
        void action(Component* co);
	
        GDrawEnv* src;
    };
   
    class C_SustPos : public Action
    {
      public:
        C_SustPos(GDrawEnv* src)
        {
            this->src=src;
        }
        void action(Component* co);
	
        GDrawEnv* src;
    };

    class Drain : public Value
    {
      public:
	
        Drain(GDrawEnv* src);
        smp_t nextValue();
	
        GDrawEnv* src;
        smp_t cval;
    };
   
    class C_HOLDEND : public Action
    {
      public:
        C_HOLDEND(GDrawEnv* src)
        {
            this->src=src;
        }
        void action(Component* co);
	
        GDrawEnv* src;
    };
   
    class C_FINCYC : public Action
    {
      public:
        C_FINCYC(GDrawEnv* src)
        {
            this->src=src;
        }
        void action(Component* co);
	
        GDrawEnv* src;
    };
   
   
    ConvertHz2Per* v;
   
    //FIXME: diff coords? 
    //OK, this is not effective but considering that wp's are ~ 24 it hardly matters
    //but better solution should probably be done some time...
    vector<SelectPlane::Coord> vals;
    vector<SigCoord> sigvals;
   
    Drain* drain;
    SmpPoint* out;
    CPInterpolator* sig;
   
    Button* rem_sel;
    Button* gate_on;
    Slider* sust_slide;
    smp_t sust_index;
   
    InputTaker* in[3];
    OutputGiver* og;
    SelectPlane* plane;
   
    Button* holdend;
    Button* fincyc;
};

#endif
