#ifndef G_EXP_H
#define G_EXP_H

#include "g_common.h"

class GExp : public SoundComp 
{
  public:
    GExp(BitmapFont* f, GroundMixer& g);
    ~GExp();
   
    virtual string name() const
    {
        return string("GExp");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);   
   
    class State
    {
      public:
        State(uint base_no) //0 = "2", 1="E"
        {
	     
            this->base_no=base_no;
        }
	
        uint base_no;
    };
   
  private:
    void update();
      
    class C_Update : public Action
    {
      public:

        C_Update(GExp* gen);
        void action(Component* co);
	
        GExp* src;
    };
   
    uint base_no;   
    Exponent* sig;
    Button* b_base;
};

#endif
