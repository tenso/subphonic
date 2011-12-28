#ifndef SIGNAL_OTHER_H 
#define SIGNAL_OTHER_H

#include <queue>

#include "signal.h"

namespace spl{

class GateExecCmd : public Value
{
  public:
    GateExecCmd();
   
    smp_t nextValue();
   
    //this pushes 'cmd' on 'queue' everytime v is gated
    //but only if queue.size()<maxsize
   
    void setInput(Value** v);
   
    void setCmdStr(const string& cmd);
   
    void setCmdQueue(std::queue<string>* q, uint maxsize=100);
   
  private:
    Value** in;
    string cmd;
    std::queue<string>* cmd_q;
    uint maxsize;
    bool retrig;
};


//FIXME: rename file to signal_metrics or something

//stores input in buffer
class ValueScope : public Value
{
  public:
    ValueScope(int len);
   
    void addInput(Value** in);

    void setBoost(float val);

    smp_t nextValue();
   
    RingBuffer<smp_t>* getBuffer();
   
    void clear();
      
  private:
    float boost;
    int l;
    RingBuffer<smp_t>* buffer;
    Value** in;
};


//instant abs peak, holds peaks for 'falloff' samples
class PeakMeter : public Value
{
  public:
    PeakMeter();
   
    void setAbs(); //def: true
   
    void setPeakHold(uint samples); //def:11025
    void setFalloffRate(smp_t persample);//def: 0.5/11025
   
    const smp_t* peakPtr(); //returns pointer that contains peak value
   
    void setInput(Value** in);
    smp_t nextValue();//returns input
   
    void reset(int index=0);
   
  private:
    Value** in;
   
    smp_t peak;
   
    uint hold;
    smp_t falloff;
    bool doabs;
    uint holdtick;
};


class OstreamEcho : public Value
{
  public:
    OstreamEcho(ostream& os, const string& postfix);
    void setInput(Value** in);
   
    smp_t nextValue();
   
  private:
    ostream& out;
    string post;
   
    Value** in;
};


class PrintValue : public Value
{
  public:
    PrintValue(ostream& stream);
   
    void setInput(Value** in);
   
    //default 44100, set this to 1 to print every value
    void setPeriod(uint p);
   
    smp_t nextValue(); //forwards input value
   
  private:
   
    Value** input;
    ostream& os;
    uint p;
};

//does not drain if off
class OnOff : public Value
{
  public:
    OnOff();
   
    //default:NULL
    void setOnOff(Value** v); //in < 0.5 off | in>=0.5 on | NULL=off
    void setInput(Value** v);
   
    smp_t nextValue();
   
  private:
    Value** onoff;
    Value** in;
};

}

#endif
