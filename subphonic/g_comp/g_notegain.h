#ifndef G_NOTEGAIN_H
#define G_NOTEGAIN_H

#include "g_common.h"
#include "../sig/signal_effect.h"


class GNoteGain : public SoundComp 
{
  public:
    GNoteGain(BitmapFont* f, GroundMixer& g);
    ~GNoteGain();
   
    virtual string name() const
    {
        return string("GNoteGain");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
      
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);   
   
    class State
    {
      public:
        State(int cent, int key, int oct, bool quantize)
        {
            this->cent=cent;
            this->key=key;
            this->oct=oct;
            this->quantize=quantize;
        }
	
        int cent;
        int key;
        int oct;
	
        bool quantize;
    };
   
  private:
   
   
    class C_Quantize : public Action
    {
      public:

        C_Quantize(GNoteGain* gen);
        void action(Component* co);
	
        GNoteGain* src;
    };
   
    NoteTran* sig;
   
    NumberSelect<smp_t>* ns_cent;
    NumberSelect<smp_t>* ns_key;
    NumberSelect<smp_t>* ns_oct;
   
    smp_t cent;
    smp_t key;
    smp_t oct;

   
    Value* v_cent;
    Value* v_key;
    Value* v_oct;
   
    Button* b_quantize;
};

#endif
