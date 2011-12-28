#ifndef G_FILTER_H
# define G_FILTER_H

# include "g_common.h"
# include "../sig/signal_filter.h"

/*FIXME: lacks bandstop*/

class GFilter : public SoundComp
{
    #define Q_MIN 0.1
    static const int KNOB_NUMREVS=4;
    static const int FQMAX=5000; //100*100=10.000
    /*static const smp_t FQSCALE_OFF=10;
      static const smp_t FQSCALE_ON=1;*/
   
    static const int Q_MAX=10;
   
  public:
   
    GFilter(BitmapFont* f, GroundMixer& g);
    ~GFilter();
   
    virtual string name() const
    {
        return string("GFilter");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);   
    void remInput(Value** out, unsigned int fromid);
   
    void updateFilter();
    void updateParam();
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    int syncToIndex(unsigned int index);
   
    class State
    {
      public:
        State(uint c_db, uint c_type, double cut, double q, bool hzscl)
        {
            this->c_db=c_db;
            this->c_type=c_type;
            this->cut=cut;
            this->hzscl=hzscl;
            this->q=q;
        }
        ~State()
        {
        }
	
        double q;
        double cut;
        bool hzscl; //unused
	
        uint c_db;
        uint c_type;
    };
   
  private:
  
   
    class C_PARAM : public Action
    {
      public:	
        C_PARAM(GFilter* gen);
        void action(Component* c);
      
      private:
        GFilter* src;
    };
   
    class C_FILTER : public Action
    {
      public:
	
        C_FILTER(GFilter* in);
        void action(Component* comp);
	
      private:
        GFilter* src;
    };
   
    class C_RESET : public Action
    {
      public:	
        C_RESET(GFilter* gen);
        void action(Component* c);
      
      private:
        GFilter* src;
    };
   
    KnobSlider* cutoff;
    KnobSlider* q;
    Button* hz_scl;

    Const* cut_val;
    Const* q_val;
   
    NButton* c_db;
    NButton* c_type;
   
    Button* reset;
   
    Value** in;
   
    BasicFilter* sig; //out
   
    static const uint N_DECIBEL=3;
    static const uint DB_6=0;
    static const uint DB_12=1;
    static const uint DB_18=2;
   
    static const uint N_TYPE=3;
    static const uint T_LP=0;
    static const uint T_BP=1;
    static const uint T_HP=2;

   
   
    BasicFilter* filters[N_DECIBEL][N_TYPE];
    //current from buttons
   
    InputTaker* it[3];
};

#endif
