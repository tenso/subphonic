#ifndef G_BITCRUSH_H
#define G_BITCRUSH_H

#include "g_common.h"

class GBitCrush : public SoundComp
{
  public :
    GBitCrush(BitmapFont* f, GroundMixer& g);
    ~GBitCrush();
   
    virtual string name() const
    {
        return string("GBitCrush");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** in, uint id);
    void remInput(Value** out, uint id);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(uint n)
        {
            this->n=n;
        }
	
        uint n;

    };
   
   
  private:
   
    class C_Update : public Action
    {
      public:
        C_Update(GBitCrush* src);
        void action(Component* c);
	
      private:
        GBitCrush* src;
    };
   
    smp_t nbit;
    NumberSelect<smp_t>* ns_nbit;
    BitCrush* sig;
    SmpPoint* v_bits;
};

#endif
