#ifndef SPL_MIDI_H
# define SPL_MIDI_H

# include <iostream>

# include <vector>
# include <sstream>

# include <alsa/asoundlib.h>
# include <poll.h>

# include <util/util.h>
# include <util/types.h>
# include <util/debug.h>

//MIDIKEYS: midikey:60 => C4 etc...
//          midikey:24 => C1 (note number=4)
//          
//          gives ===> noteMap(midikey-20)
// 
//uses ALSA, only direct mode(no timestamping/queue) for now

namespace spl
{

//inherit this and override functions that will be called in MIDI::pollEvents()
class MIDIcallbacks
{
    //defaults just echos input on cout
  public:
    MIDIcallbacks()
    {
    }
    virtual ~MIDIcallbacks()
    {
    }
   
    virtual void noteOnOff(unsigned char channel, unsigned char note, unsigned char velocity,
    unsigned char off_velocity, unsigned int duration);
   
    virtual void noteOn(unsigned char channel, unsigned char note, unsigned char velocity);
   
    virtual void noteOff(unsigned char channel, unsigned char note, unsigned char velocity);
   
    virtual void control(unsigned char channel,  unsigned int param, signed int value);
   
    virtual void pgmchange(unsigned char channel,  unsigned int param, signed int value);
   
    //value -8192, 8191
    virtual void pitchbend(unsigned char channel,  signed int value);
   
    //use these to get info; set for each event in poll
    uint getTick();
    uint getSec();
    uint getNSec();
   
    uchar getClient(uchar& client);
    uchar getPort();
   
   
    //these are called before any event callback
    void setEventTime(unsigned int ticktime, unsigned int sec, unsigned int nsec);
    void setEventClient(uchar client, uchar port);
   
  private:
    unsigned int ticktime;
    unsigned int sec;
    unsigned int nsec;
   
    uchar client;
    uchar port;
};


class MIDI
{
  public:
    //returns false on fail/error
   
    class Event;
   
    MIDI();
    ~MIDI();
   
    //open for midi input, check isOk() after,
    //the portnumber is delivered to the event via getPort()
    bool open(const std::string& client_name, uint num_in_ports=1, uint num_out_ports=0);
    bool close();
   
    //MIDIcallbacks::members called by pollEvents()
    void setEventCallbacks(MIDIcallbacks* cb);
    bool pollEvents();
   
    //ports not in alsa
    void sendEventDirect(const Event& event, uint out_port=0);
   
    //remember: NoteOn with vel=0 ==NoteOff
    Event eventMakeNoteOn(uchar chan, uchar note, uchar vel);
    Event eventMakeNoteOff(uchar chan, uchar note, uchar vel);
    Event eventMakeNote(uchar chan, uchar note, uchar vel, uchar off_vel, uint duration);
    Event eventMakeCtrl(uchar chan, uint param, sint value);
   
    std::string getClientName();
    int getClientId();
   
    //FIXME: fix name to getALSA...
    //get alsa ports, not used in sendEvent***()!
    int getClientInPort(uint i);
    int getClientOutPort(uint i);
   
    uint getNumInPorts();
    uint getNumOutPorts();
   
    bool isOk();   
    std::string errorMess();
   
    //negative = infinite wait
    //ms=0 is good for fps
    void setPollTimeout(int ms);
    int getPollTimeout();
   
   
  private:
    std::string name;
    MIDIcallbacks* callback;
    bool isok;
    std::string err_mess;
   
    snd_seq_t* seq;
    std::vector<int> in_ports;
    std::vector<int> out_ports;
   
    int poll_timeout; /*milli sec*/
    int num_p_fd;
    struct pollfd* p_fd;
   
    snd_seq_event_t* event;
};

class MIDI::Event
{
  public:
    enum TYPE{NOTE, NOTE_ON, NOTE_OFF, CTRL};
    TYPE type;
    unsigned char chan;
   
    //for note*
    unsigned char note;
    unsigned char vel;
    //for 'note'
    unsigned char off_vel;
    unsigned int duration;
   
    //for ctrl
    unsigned int param;
    signed int value;
   
};

}//end of namespace
#endif
