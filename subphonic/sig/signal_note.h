#ifndef SIGNAL_NOTE_H
#define SIGNAL_NOTE_H

#include "signal.h"

namespace spl{

class KeyExtract : public Value
{
  public:
    KeyExtract();
   
    //hz
    //def: 0
    void setInput(Value** in);
   
    //returns key numbers i.e 440hz -> 49
    smp_t nextValue();
   
  private:
    Value** in;
};


class NoteTran : public Value
{
  public:
    NoteTran();
   
    //hz
    //def: 0
    void setInput(Value** in);
   
    //i.e hundreds of a note
    //def:0
    void setCentOff(Value** in);
   
    //def: 0
    void setKeyOff(Value** in);
   
    //def:0
    //i.e same as key*12
    void setOctOff(Value** in);
   
    smp_t nextValue();
   
    //QUANTIZE OFFSETS INPUTS?
    //i.e is KeyOffset etc quantized to integers?
   
    //default:true
    void setQuantize(bool on);
    bool getQuantize();
   
  private:
    Value** in;
    Value** key;
    Value** oct;
    Value** cent;
   
    bool quantize;
};

}

#endif
