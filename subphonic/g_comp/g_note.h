#ifndef G_NOTE_H
#define G_NOTE_H

#include "g_common.h"

class GNote : public SoundComp
{
  public :
    GNote(BitmapFont* f, GroundMixer& g);
   
    virtual string name() const
    {
        return string("GNote");
    }
   
    Value** getOutput(unsigned int id);
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(int key, int oct)
        {
            this->key=key;
            this->oct=oct;
        }
        ~State()
        {
        }
        int key;
        int oct;
    };
     
   
  private:
      
    void setHz(int key);
   
    class PMODE : public Action
    {
      public:
	
        PMODE(GNote* s, int i);
        void action(Component* comp);
	
        GNote* src;
        int key;
	
    };
   
    class Update : public Action
    {
      public:
	
        Update(GNote* s);
	
        void action(Component* comp);
	
        GNote* src;
		
    };

   
    Button* key[12];
    static char key_str[12][3]; //12*2 bytes saved
    SmpPoint* sig0;
    SmpPoint* sig_gate;
    OutputGiver* og;
    smp_t hz;
    smp_t octave;
    NumberSelect<smp_t>* ns0;
    Label* l;
    smp_t ison;
   
    int lastkey;
};

#endif
