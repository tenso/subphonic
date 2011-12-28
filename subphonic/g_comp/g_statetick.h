#ifndef G_STATETICK_H
# define G_STATETICK_H

# include "g_common.h"
# include "../m_main/m_bench.h"
# include "../m_main/progstate.h"

class GStateTick : public SoundComp
{
  public :
    GStateTick(BitmapFont* f, GroundMixer& g);
    ~GStateTick();
   
    virtual string name() const
    {
        return string("GStateTick");
    }
   
   
   
    /*IMPORTANT run one of these before use!
      differance is:
      * when statetick is in main program it has to update  a visual marker etc
      * but when loaded in say a module it only needs to apply states
      * setBenchStates needs some extra data contained in MainBench for setBench()
      */
   
    void setBench(MainBench* b);
    void setBenchStates(BenchStates* bs, int current_slot, int start_marker,
    int stop_marker, int max_slots);
   
    virtual void addInput(Value** out, unsigned int fromid);
    virtual void remInput(Value** out, unsigned int fromid);
      
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(bool on)
        {
            this->on=on;
        }
	
        bool on;
    };
   
  private:
   
    /*
      class C_FLIP : public Action
      {
      public:
      C_FLIP(bool* ptr)
	  {
      this->ptr=ptr;
	  }
	
      void action(Component* c)
	  {
      *ptr=!(*ptr);
	  }
	
      private:
      bool* ptr;
      };*/
   
    class StateTickerBench :  public Value
    {
      public:
        StateTickerBench(GStateTick* src);
	
        void setBench(MainBench* b);
        void setTrig(Value** v);
        void setSel(Value** v);
        smp_t nextValue();
		
      private:
        GStateTick* src;
        MainBench* bench;
	
        Value** trig;
        Value** sel_lvl;
        bool retrig;
	
    };
   
    class StateTickerStateOnly :  public Value
    {
      public:
        StateTickerStateOnly(GStateTick* src);
	
        void setBenchStates(BenchStates* bs, int current_slot, int start_marker,
        int stop_marker, int max_slots);

        void setTrig(Value** v);
        void setSel(Value** v);
        smp_t nextValue();
	
	
      private:
        GStateTick* src;
	
        BenchStates* bs;
        int current_slot;
        int start_marker;
        int stop_marker;
        int max_slots;
	
        Value** trig;
        Value** sel_lvl;
        bool retrig;
	
    };
   
    Button* on_button;
   
    StateTickerBench* ticker_bench;
    StateTickerStateOnly* ticker_states;
};

#endif
