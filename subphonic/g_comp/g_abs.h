#ifndef G_ABS_H
#define G_ABS_H

#include "g_common.h"

class GAbs : public SoundComp
{
  public :
    GAbs(BitmapFont* f, GroundMixer& g);
    ~GAbs();
   
    virtual string name() const
    {
        return string("GAbs");
    }
   
    Value** getOutput(unsigned int id);

    virtual void addInput(Value** out, unsigned int fromid);
    virtual void remInput(Value** out, unsigned int fromid);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);

    class State
    {
      public:
        State(bool mode)
        {
            this->mode = mode;
        }
	
        bool mode;
    };
   
  private:
    Rectify* sig;
   
    class C_Mode : public Action
    {
      public:
        C_Mode(GAbs* src);
	
        void action(Component* c);
	
      private:
	
        GAbs* src;
    };
   
    Button* b_mode;
};

#endif
