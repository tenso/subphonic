#ifndef G_JOYSTICK_H
#define G_JOYSTICK_H

#include "g_common.h"

class GJoystick : public SoundComp
{
  public :
    GJoystick(BitmapFont* f, GroundMixer& g);
    ~GJoystick();
   
    virtual string name() const
    {
        return string("GJoystick");
    }
   
    Value** getOutput(unsigned int id);
   
    int getId();
    void setOpen(bool val);
    void giveAxisPos(uint axis, double pos);
    void giveButtonState(uint button, bool state);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(int id)
        {
            this->id=id;
        }
	
        int id;
    };
   
  private:

    static const uint N_AXIS = 6;
    static const uint N_BUTTONS = 12;

    /*
      class C_HOLD : public Action
      {
      public:
	
      C_HOLD(bool* in);
      void action(Component* comp);
	
      bool* in;
	
      };*/
    //Button* b_hold;
   
    int id;
   
    Const* axis[N_AXIS];
    Const* buttons[N_BUTTONS];
    OutputGiver* og[N_AXIS+N_BUTTONS];
   
    NumberSelect<int>* ns_id;
   
    bool open;

};

#endif
