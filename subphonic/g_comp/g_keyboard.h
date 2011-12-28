#ifndef G_KEYBOARD_H
#define G_KEYBOARD_H

#include "g_common.h"

class GKeyboard : public SoundComp
{
  public :
    GKeyboard(BitmapFont* f, GroundMixer& g);
  
    virtual string name() const
    {
        return string("GKeyboard");
    }
   
    Value** getOutput(unsigned int id);
   
    //FIXME:name missleading
    //sets same as noteMap(...) (sig_util.h)
    void setHz(int key);
    void gateOn(bool on);
   
    //FIXME:move this
    bool stay;
   
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(bool aon, int oct)
        {
            this->aon=aon;
            this->oct=oct;
        }
	
        bool aon;
        int oct;
    };
   
  private:
   
    class C_HOLD : public Action
    {
      public:
	
        C_HOLD(bool* in);
        void action(Component* comp);
	
        bool* in;
	
    };
   
    SmpPoint* sig_hz;
    SmpPoint* sig_gate;
    OutputGiver* og[2];
    smp_t hz;
   
    int octave;
    NumberSelect<int>* s_oct;
   
    Label* l_note;
    smp_t ison;
   
    Button* b_hold;
};

#endif
