#ifndef G_SELECT_IN_H
#define G_SELECT_IN_H

#include "g_common.h"

/*selection priority:
 * highest: select with level (if connected)
 * then select by input(highest input highest prio
 * last cycle*/

class GSelectIn : public SoundComp
{
    static const uint NUM_IN=16; //old was 6
   
  public:
    GSelectIn(BitmapFont* f, GroundMixer& g);
    ~GSelectIn();
   
    virtual string name() const
    {
        return string("GSelectIn");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(Button** b, int selected, double slen)
        {
            this->slen=slen;
            this->selected=selected;
            for(uint i=0;i<NUM_IN;i++)
            {
                buttons[i]=b[i]->isPressed();
            }
            num=NUM_IN;
        }
        ~State()
        {
        }
	
        double slen;
        int selected;
        bool buttons[NUM_IN];
        int num;
    };
   
  private:
    void update();
   
    class Drain : public Value
    {
        //this will drain select in, and also return current out
      public:
        Drain(GSelectIn* src);
	
        smp_t nextValue();
	
        void setSyncIn(Value** v);
        void setNumOnIn(Value** v);
	
      private:
        bool re_tick_f;
        bool re_tick[NUM_IN];
	
        GSelectIn* src;
	
        Value** sync_in;
        bool resync;
	
        Value** in_numon;
        uint last_num_on;
    };
   
    class Out : public Value
    {
        //this will drain select in, and also return current out
      public:
        Out(GSelectIn* src);
	
        smp_t nextValue();
	
      private:

        GSelectIn* src;
    };
   
   
    class C_ON : public Action
    {
      public:

        C_ON(GSelectIn* s, int i);
        void action(Component* co);
	
      private:
        GSelectIn* src;
        int index;
    };
   
    class C_SLEN : public Action
    {
      public:

        C_SLEN(GSelectIn* s);
        void action(Component* co);
	
      private:
        GSelectIn* src;
    };
   

   
    class C_SEL : public Action
    {
      public:

        C_SEL(GSelectIn* s);
        void action(Component* co);
	
      private:
        GSelectIn* src;
    };
   
    Out* out;
   
    Value** sel_v_f;
    Value** sel_v_lvl;
    Value** sel_v[NUM_IN];
   
    Value** inputs[NUM_IN];
   
    int selected;
    uint num_on;
         
    OneShot* sync;
    OutputGiver* og_sync;
    KnobSlider* sync_len;
   
    OutputGiver* og;
    InputTaker* in_sel_f;
    InputTaker* in_sel[NUM_IN];
   
    InputTaker* in_sel_lvl;
      
    InputTaker* in[NUM_IN];
   
    Button* b_sel;
    Button* b_on[NUM_IN];
    LedsIndex<int>* leds;
   
    Drain* drain;
   
    Const* on_out;
   
    InputTaker* in_sel_index;
    Value** sel_v_index;
};

#endif
