#ifndef G_EXECCMD_H
#define G_EXECCMD_H

#include "../sig/signal_other.h"
#include "g_common.h"

class GExecCmd : public SoundComp 
{
  public:
    GExecCmd(BitmapFont* f, GroundMixer& g);
    ~GExecCmd();
   
    virtual string name() const
    {
        return string("GExecCmd");
    }
   
    void setCmdStr(const string& cmd);
    void setCmdQueue(std::queue<string>* cmd_q);
   
    //Value** getOutput(unsigned int id);
   
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
/*   void getState(SoundCompEvent* e);
     void setState(SoundCompEvent* e);   
   
     class State
     {
     public:
     State()
     {
     }
     };*/
   
  private:
    Label* cmd_l;
   
    GateExecCmd* sig;

};

#endif
