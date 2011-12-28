#ifndef G_VECTOR_H
#define G_VECTOR_H

#include "g_common.h"

class GVector : public SoundComp
{
    typedef SelectPlane::Coord Coord;
   
  public :
    GVector(BitmapFont* f, GroundMixer& g);
    ~GVector();
   
    virtual string name() const
    {
        return string("GVector");
    }
   
   
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
    Value** getOutput(unsigned int id);
      
  private:
   
    void updateGains(const Coord& c);
   
    class C_UpdatePAD : public Action
    {
      public:
        C_UpdatePAD(GVector* src)
        {
            this->src=src;
        }
        void action(Component* co);
	
        GVector* src;
    };
   
    class C_RemSel : public Action
    {
      public:
        C_RemSel(GVector* src)
        {
            this->src=src;
        }
        void action(Component* co);
	
        GVector* src;
    };
   
    //updates in/out pos
    class Drain : public Value
    {
      public:
        Drain(GVector* s);
	
        smp_t nextValue();
	
      private:
        GVector* src;
    };
   
   
    Button* rem_sel;
    Mixer* mix;
    Gain* gains[4];
    Value** inputs[4];
    InputTaker* in[4];
    OutputGiver* out;
    SelectPlane* plane;
   
    Drain* drain;
   
    Value** pos_in_v;
    Value** pos_in_h;
   
    Const* pos_out_v;
    Const* pos_out_h;
};

#endif
