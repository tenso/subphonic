#ifndef G_LOG_H
#define G_LOG_H

#include "g_common.h"

class GLog : public SoundComp 
{
  public:
    GLog(BitmapFont* f, GroundMixer& g);
    ~GLog();
   
    virtual string name() const
    {
        return string("GLog");
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

        C_Update(GLog* gen);
        void action(Component* co);
	
        GLog* src;
    };
   
    uint base_no;   
    Logarithm* sig;
    Button* b_base;
};

#endif
