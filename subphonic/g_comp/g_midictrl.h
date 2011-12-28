#ifndef G_MIDICTRL_H
# define G_MIDICTRL_H

# include "g_common.h"

//set controller and channel to 0 for any controller

class GMIDIctrl : public SoundComp
{
    static const uint NUM_WHOLE = 5;
    static const uint NUM_FRAC = 5;
   
  public :
    GMIDIctrl(BitmapFont* f, GroundMixer& g);
    ~GMIDIctrl();
   
    virtual string name() const
    {
        return string("GMIDIctrl");
    }
   
    Value** getOutput(unsigned int id);
   
    void updateGain();
   
    int getChan();
    int getCtrl();
    void  setChan(int c);
    void  setCtrl(int c);
    void setValue(smp_t v);
   
    //flag that comp wants to subscribe to next event
    bool recCtrlChan(); //after call this  resets
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(int ch, int ctrl, smp_t val, bool rec, double g, double ng)
        {
            dorec=rec;
            channel=ch;
            controller=ctrl;
            value=val;
            gain=g;
            nsgain=ng;
        }
        ~State()
        {
        }
	
        bool dorec;
	
        int channel;
        int controller;
	
        smp_t value;
        double gain;
        double nsgain;
    };
   
  private:
   
    class REC : public Action
    {
      public:
        REC(GMIDIctrl* src);
        void action(Component* c);
	
      private:
        GMIDIctrl* src;
    };
   
    class C_GAIN : public Action
    {
      public:
        C_GAIN(GMIDIctrl* src);
        void action(Component* c);
	
      private:
        GMIDIctrl* src;
    };
   
   
    bool dorec;
   
    int channel;
    int controller;
    smp_t value;
   
    OutputGiver* og;
    OutputGiver* og_one;
   
    SmpPoint* sig;
    Gain* sig_gain;
    OneShot* one_shot;
   
    NumberSelect<int>* ns_chan;
    NumberSelect<int>* ns_ctrl;
   
    NumberSelect<smp_t>* ns_gain;
    smp_t ns_val;
   
    Label* val_l;
    Button* rec;
    KnobSlider* gain;
   
   
};

#endif
