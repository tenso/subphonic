#include "midi.h"

namespace spl
{

using namespace std;

void MIDIcallbacks::noteOnOff(unsigned char channel, unsigned char note, unsigned char velocity,
unsigned char off_velocity, unsigned int duration)
{
    cout << "noteOnOff" << endl;
}

void MIDIcallbacks::noteOn(unsigned char channel, unsigned char note, unsigned char velocity)
{
    /*   cout << "tick: " << ticktime << endl;
         cout << "sec: " << sec << endl;*/
    cout << "noteOn: " << (int)note << endl;
    cout << "chan: " << (int)channel<< endl;
    cout << "velocity: " << (int)velocity << endl;
   
    if(velocity==0)
    {
        cout << "velocity==0 => note off" << endl;
    }
}


void MIDIcallbacks::noteOff(unsigned char channel, unsigned char note, unsigned char velocity)
{
    cout << "noteOff" << endl;
}

void MIDIcallbacks::control(unsigned char channel,  unsigned int param, signed int value)
{
    cout << "control, param: " << param << " val: " << value<< endl;
}

void MIDIcallbacks::pgmchange(unsigned char channel,  unsigned int param, signed int value)
{
    cout << "pgmchange, param: " << param << " val: " << value << " ch:" << (int)channel << endl;
}


//value -8192, 8191
void MIDIcallbacks::pitchbend(unsigned char channel,  signed int value)
{
    cout << "pitchbend: " << value << endl;
}

void MIDIcallbacks::setEventTime(unsigned int ticktime, unsigned int sec, unsigned int nsec)
{
    this->ticktime=ticktime;
    this->sec=sec;
    this->nsec=nsec;
}

void MIDIcallbacks::setEventClient(uchar client, uchar port)
{
    this->client=client;
    this->port=port;
}

uint MIDIcallbacks::getTick()
{
    return ticktime;
}

uint MIDIcallbacks::getSec()
{
    return sec;
}

uint MIDIcallbacks::getNSec()
{
    return nsec;
}


uchar MIDIcallbacks::getClient(uchar& client)
{
    return client;
}

uchar MIDIcallbacks::getPort()
{
    return port;
}


/******/
/*MIDI*/
/******/

MIDI::MIDI()
{
    p_fd=NULL;
    isok=false;
    callback=NULL;
    seq=NULL;
   
    err_mess.assign("uninitialized");
   
    poll_timeout=0; //milliseconds
}

MIDI::~MIDI()
{
    if(isok)close();
}


//open for midi input, check isOk() after,
//returns false on fail
bool MIDI::open(const string& client_name, uint num_in_ports, uint num_out_ports)
{
    if(isok)
    {
        err_mess.assign("already open");
        return false;
    }
   
    name=client_name;
   
    if(num_in_ports==0&&num_out_ports==0)
    {
        ERROR("no in or out ports to open, fail");
        return false;
    }
   
    int omode=0;
    if(num_in_ports!=0 && num_out_ports==0)omode = SND_SEQ_OPEN_INPUT;
    if(num_in_ports==0 && num_out_ports!=0)omode = SND_SEQ_OPEN_OUTPUT;
    else omode = SND_SEQ_OPEN_DUPLEX;
   
   
    //open
    int err = snd_seq_open(&seq, "default", omode, 0);    //last: block mode 0 or SND_SEQ_NONBLOCK
    if(err!=0)
    {
        err_mess.assign("open failed");
        return false;
    }
   
    //alsa name
    snd_seq_set_client_name(seq, client_name.c_str());
   
   
    //port capabilities: allow read/write to and subscription read/write(however that works)
    //make IN PORTS, 
    int caps = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;
    for(uint i=0;i<num_in_ports;i++)
    {
        ostringstream portname;
        portname << client_name << " IN:" << i;
        int portid = snd_seq_create_simple_port(seq, portname.str().c_str(), caps, SND_SEQ_PORT_TYPE_APPLICATION);
	
        if(portid<0)
        {
            err_mess.assign( "IN port " + toStr(i) + " open failed");
            return false;
        }
	
        in_ports.push_back(portid);
    }
   
    //make OUT PORTS
    caps = SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ;
    for(uint i=0;i<num_out_ports;i++)
    {
        ostringstream portname;
        portname << client_name << " OUT:" << i;
        int portid = snd_seq_create_simple_port(seq, portname.str().c_str(), caps, SND_SEQ_PORT_TYPE_APPLICATION);
	
        if(portid<0)
        {
            err_mess.assign( "OUT port " + toStr(i) + " open failed");
            return false;
        }
	
        out_ports.push_back(portid);
    }
      
   
    //setup polling(only for input)
   
    //POLLIN or POLLOUT
    num_p_fd = snd_seq_poll_descriptors_count(seq, POLLIN);
    if(num_p_fd<=0)
    {
        err_mess.assign("no poll descriptors");
        return false;
    }
   
    p_fd = new pollfd[num_p_fd];
   
    int count = snd_seq_poll_descriptors(seq, p_fd, num_p_fd, POLLIN);
   
    if(count != num_p_fd)
    {
        D_ERROR("error");
    }
   
    isok=true;
   
    return true;
}

bool MIDI::close()
{
    int err;
    if(!isok)
    {
        err_mess.assign("not open");
        return false;
    }
   
    for(uint i=0;i<in_ports.size();i++)
    {
        err = snd_seq_delete_port(seq, in_ports[i]);
        if(err<0)
        {
            err_mess.assign("IN port delete failed");
            //return false;
        }
    }
   
    for(uint i=0;i<out_ports.size();i++)
    {
        err = snd_seq_delete_port(seq, out_ports[i]);
        if(err<0)
        {
            err_mess.assign("OUT port delete failed");
            //return false;
        }
    }

   
    err = snd_seq_close(seq);
    if(err<0)
    {
        err_mess.assign("close failed");
        return false;
    }
   
    delete p_fd;
    p_fd=NULL;
   
    isok=false;
   
    return true;
}

string MIDI::getClientName()
{
    return name;
}

int MIDI::getClientId()
{
    D_ASSERT(seq!=NULL);
    return snd_seq_client_id(seq);
}

int MIDI::getClientInPort(uint i)
{
    D_ASSERT(i<in_ports.size());
    return in_ports[i];
}

int MIDI::getClientOutPort(uint i)
{
    D_ASSERT(i<out_ports.size());
    return out_ports[i];
}


uint MIDI::getNumInPorts()
{
    return in_ports.size();
}

uint MIDI::getNumOutPorts()
{
    return in_ports.size();
}


void MIDI::setEventCallbacks(MIDIcallbacks* cb)
{
    callback=cb;
}


bool MIDI::pollEvents()
{
    D_ASSERT(callback!=NULL);
    if(callback==NULL)
    {
        D_ERROR("no callback");
        return false;
    }
   
    int err = poll(p_fd, num_p_fd, poll_timeout);
    if(err<0)
    {
        D_ERROR("poll event failed");
        err_mess.append(" ,poll failed");
        return false;
    }
   
    if(err==0)return true; //timeout
   
    //handle events
   
    do{
        snd_seq_event_input(seq, &event);
      
        callback->setEventClient(event->source.client, event->source.port);
        callback->setEventTime(event->time.tick, event->time.time.tv_sec, event->time.time.tv_nsec);
      
        switch(event->type)
        {
            case SND_SEQ_EVENT_CONTROLLER:
                callback->control(event->data.control.channel, event->data.control.param, event->data.control.value);
                break;
	   
            case SND_SEQ_EVENT_NOTEON:
                callback->noteOn(event->data.note.channel, event->data.note.note, event->data.note.velocity);
                break;
	   
            case SND_SEQ_EVENT_NOTEOFF:
                callback->noteOff(event->data.note.channel, event->data.note.note, event->data.note.velocity);
                break;
	   
            case SND_SEQ_EVENT_NOTE:
                callback->noteOnOff(event->data.note.channel, event->data.note.note, event->data.note.velocity,
                event->data.note.off_velocity, event->data.note.duration);
                break;
	   
            case SND_SEQ_EVENT_PITCHBEND:
                callback->pitchbend(event->data.control.channel, event->data.control.value);
                break;
	   
            case SND_SEQ_EVENT_PGMCHANGE:
                callback->pgmchange(event->data.control.channel, event->data.control.param, event->data.control.value);
                break;
	   
                /***************/
                /*UNIMPLEMENTED*/
                /***************/
	   
            case SND_SEQ_EVENT_SYSTEM:
                //cout << "sys event" << endl;
                break;
	   
            case SND_SEQ_EVENT_START:
                //cout << "start" << endl;
                break;
	   
            case SND_SEQ_EVENT_STOP:
                //cout << "stop" << endl;
                break;
	   
            case SND_SEQ_EVENT_CONTINUE:
                //cout << "continue" << endl;
                break;
	   
            case SND_SEQ_EVENT_CLIENT_START:
                //cout << "client start" << endl;
                break;
	   
            case SND_SEQ_EVENT_CLIENT_EXIT:
                //cout << "client exit" << endl;
                break;
	   
            case SND_SEQ_EVENT_CLIENT_CHANGE:
                //cout << "client change" << endl;
                break;
	   
            case SND_SEQ_EVENT_PORT_SUBSCRIBED:
                //cout << "port subscribe" << endl;
                break;
	   
            case SND_SEQ_EVENT_PORT_UNSUBSCRIBED:
                //cout << "port unsubscribe" << endl;
                break;
	   
            default:
                cout << "unimplemented event type: " << (int)event->type << endl;
                break;
        }
      
        snd_seq_free_event(event);
    }while(snd_seq_event_input_pending(seq,0));
   
    return true;
}


bool MIDI::isOk()
{
    return isok;
}

string MIDI::errorMess()
{
    return err_mess;
}


void MIDI::setPollTimeout(int ms)
{
    poll_timeout=ms;   
}

int MIDI::getPollTimeout()
{
    return poll_timeout;
}

void MIDI::sendEventDirect(const Event& event, uint out_port)
{
    snd_seq_event_t ev;
   
    D_ASSERT(out_port<out_ports.size());
   
    //...init event
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, out_ports[out_port]);
   
    snd_seq_ev_set_subs(&ev);//send to all subscribers
    snd_seq_ev_set_direct(&ev);
   

   
    //build event type
    switch(event.type)
    {
        case Event::NOTE:
            ev.type=SND_SEQ_EVENT_NOTE;
            ev.data.note.channel=event.chan;
            ev.data.note.note=event.note;
            ev.data.note.velocity=event.vel;
            ev.data.note.off_velocity=event.off_vel;
            ev.data.note.duration=event.duration;
            break;
      
        case Event::NOTE_ON:
            ev.type=SND_SEQ_EVENT_NOTEON;
            ev.data.note.channel=event.chan;
            ev.data.note.note=event.note;
            ev.data.note.velocity=event.vel;
            break;
      
        case Event::NOTE_OFF:
            ev.type=SND_SEQ_EVENT_NOTEOFF;
            ev.data.note.channel=event.chan;
            ev.data.note.note=event.note;
            ev.data.note.velocity=event.vel;
            break;

        case Event::CTRL:
            ev.type=SND_SEQ_EVENT_CONTROLLER;
            ev.data.control.channel=event.chan;
            ev.data.control.param=event.param;
            ev.data.control.value=event.value;
    }
   
   
   
    //...send
    snd_seq_event_output(seq, &ev);
    snd_seq_drain_output(seq);
}


MIDI::Event MIDI::eventMakeNoteOn(uchar chan, uchar note, uchar vel)
{
    Event event;
   
    event.type=Event::NOTE_ON;
    event.chan=chan;
    event.note=note;
    event.vel=vel;
   
    return event;
}

MIDI::Event MIDI::eventMakeNoteOff(uchar chan, uchar note, uchar vel)
{
    Event event;
    event.type=Event::NOTE_OFF;
    event.chan=chan;
    event.note=note;
    event.vel=vel;
    return event;
}

MIDI::Event MIDI::eventMakeNote(uchar chan, uchar note, uchar vel, uchar off_vel, uint duration)
{
    Event event;
    event.type=Event::NOTE;
    event.chan=chan;
    event.note=note;
    event.vel=vel;
    event.off_vel=off_vel;
    event.duration=duration;
    return event;
}

MIDI::Event MIDI::eventMakeCtrl(uchar chan, uint param, sint value)
{
    Event event;
    event.type=Event::CTRL;
    event.param=param;
    event.value=value;
    return event;
}


}//end of namespace spl
