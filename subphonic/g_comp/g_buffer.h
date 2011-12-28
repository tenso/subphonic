#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "g_common.h"

class GBuffer : public SoundComp
{
  public :
    GBuffer(BitmapFont* f, GroundMixer& g);
    ~GBuffer();
   
    virtual string name() const
    {
        return string("GBuffer");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** in, uint id);
    void remInput(Value** out, uint id);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(smp_t n/*, uint suppressed*/)
        {
            len=n;
            //this->suppressed=suppressed;
        }
		
        smp_t len;
        //uint suppressed;
    };
   
   
  private:
   
    class C_Update : public Action
    {
      public:
        C_Update(GBuffer* src);
        void action(Component* c);
	
      private:
        GBuffer* src;
    };
   
    smp_t len;
    NumberSelect<smp_t>* ns_len;
    ValueBuffer* sig;
};

#endif
