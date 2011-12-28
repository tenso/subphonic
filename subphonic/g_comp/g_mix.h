#ifndef G_MIX_H 
#define G_MIX_H

#include "g_common.h"

class GMix : public SoundComp
{
  public:
    static const uint NUM_IN=6;
   
    GMix(BitmapFont* f, GroundMixer& g);
    ~GMix();
   
    virtual string name() const
    {
        return string("GMix");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);

    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);

    class State
    {
      public:
        State(Button** b)
        {
            for(uint i=0;i<NUM_IN;i++)
            {
                buttons[i]=b[i]->isPressed();
            }
        }
        ~State()
        {
        }
	
        bool buttons[NUM_IN];
    };
   
  private:
      
    class R_A : public Action
    {
      public:

        R_A(GMix* s, int i);
        void action(Component* co);
	
        GMix* src;
        int index;
    };
   
    MixerArray* mix;
    OutputGiver* og;
    InputTaker* in[NUM_IN];
    Button* rb[NUM_IN];
    Value** inputs[NUM_IN];
};

#endif
