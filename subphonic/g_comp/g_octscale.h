#ifndef G_OCTSCALE_H
#define G_OCTSCALE_H

#include "g_common.h"

/**********/
/*OBSOLETE*/
/**********/

class OctScale: public Value
{
  public:
    OctScale();
   
    /*
     * out =  level*2^( clamp(input,-1,1) * noct)
     * 
     *input:-1  => out: will be setNOct octaves under setLevel()
     *input:0  =>  out: setLevel()
     *input:1.0 => out: will be setNOct octaves over setLevel()
     */ 
    void setInput(Value** in);
   
   
    /*setLevel is just for ease of use;
     *
     *put this in serial with a Multiplication by whatever level would be and use
     *setLevel(Const(1.0)) == setLevel(NULL)
     */ 
   
    //default: NULL => level=1.0
    void setLevel(Value** in);
   
    //min: 1
    //default:4
    void setNOct(uint n);
   
    smp_t nextValue();
   
  private:
    uint noct;
    Value** in;
    Value** level;
};



class GOctScale : public SoundComp 
{
  public:
    GOctScale(BitmapFont* f, GroundMixer& g);
    ~GOctScale();
   
    virtual string name() const
    {
        return string("GOctScale");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** out, unsigned int fromid);
    void remInput(Value** out, unsigned int fromid);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);   
   
    class State
    {
      public:
        State(uint noct)
        {
	     
            this->noct=noct;
        }
	
        uint noct;
    };
   
  private:
    void update();
      
    class C_Update : public Action
    {
      public:

        C_Update(GOctScale* gen);
        void action(Component* co);
	
        GOctScale* src;
    };
   
    uint noct;   
    OctScale* sig;
    NumberSelect<uint>* ns;
};

#endif
