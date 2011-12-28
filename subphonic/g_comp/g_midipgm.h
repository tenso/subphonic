#ifndef G_MIDIPGM_H
# define G_MIDIPGM_H

# include "g_common.h"

//set controller and channel to 0 for any controller

class GMIDIpgm : public SoundComp
{
  public :
    GMIDIpgm(BitmapFont* f, GroundMixer& g);
    ~GMIDIpgm();
   
    virtual string name() const
    {
        return string("GMIDIpgm");
    }
   
    Value** getOutput(unsigned int id);
   
    int getChan();
    int getId();
    void setChan(int c);
    void setId(int c);
    void setOn();
   
    //flag that comp wants to subscribe to next event
    bool recIdChan(); //after call this  resets
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(int ch, int id,  bool rec)
        {
            dorec=rec;
            channel=ch;
            this->id=id;
        }
        ~State()
        {
        }
	
        bool dorec;
	
        int channel;
        int id;
    };
   
  private:
   
    class REC : public Action
    {
      public:
        REC(GMIDIpgm* src);
        void action(Component* c);
	
      private:
        GMIDIpgm* src;
    };
   
    bool dorec;
   
    int channel;
    int id;
   
    OutputGiver* og;
   
    OneShot* one_shot;
   
    NumberSelect<int>* ns_chan;
    NumberSelect<int>* ns_id;
   
    Button* rec;
   
};

#endif
