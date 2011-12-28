#include "midibase.h"

MIDIcall::MIDIcall()
{
    num_press=0;
}

int MIDIcall::numComps()
{
    return keyboards.size()+ctrl.size()+pgm.size();
}


bool MIDIcall::addKey(GMIDIkey* key)
{
    keyboards.push_back(key);
    return true;
}

bool MIDIcall::remKey(GMIDIkey* key)
{
    keyboards.remove(key);
    return true; //FIXME
}


bool MIDIcall::addCtrl(GMIDIctrl* c)
{
    ctrl.push_back(c);
    return true;
}

bool MIDIcall::remCtrl(GMIDIctrl* c)
{
    ctrl.remove(c);
    return true; //FIXME
}

bool MIDIcall::addPgm(GMIDIpgm* p)
{
    pgm.push_back(p);
    return true;
}

bool MIDIcall::remPgm(GMIDIpgm* p)
{
    pgm.remove(p);
    return true; //FIXME
}


void MIDIcall::noteOnOff(unsigned char channel, unsigned char note, unsigned char velocity,
unsigned char off_velocity, unsigned int duration)
{
    cout << "implement me" << endl;
}

//MIDI LATENCY DEBUG
/*#import "../main_th.h"
  extern SharedAudio shared_audio;
*/

void MIDIcall::noteOn(unsigned char channel, unsigned char note, unsigned char velocity)
{
    //cout << "noteon event" << endl;
   
    //this is NoteOff as defined in MIDI*/
    if(velocity==0)
    {
        //cout << " note off" << endl;
        noteOff(channel, note, velocity);
        return;
    }
   
    //cout << " note on" << endl;
   
    //num_press++;
   
    for(k_it it = keyboards.begin();it!=keyboards.end();it++)
    {
        GMIDIkey* src = *it;
	
        //FIXME:
        //if(src->getChan()==channel)
	
        /*src->setHz(note);
          if(num_press>1)src->retrigGate();
          else src->setGate(true);*/
        smp_t vel = SMP_MAX*velocity/127.0;
        src->noteOn(note, vel);
    }
   
    //MIDI LATENCY DEBUG
    /*static Wav* wsamp=NULL;
      if(wsamp==NULL)
      {
      wsamp = new Wav();
      wsamp->load("test.wav");
      }
      if(wsamp->ok())shared_audio.adev.playMix(wsamp->getBytes(), wsamp->getNumBytes());*/
}

void MIDIcall::noteOff(unsigned char channel, unsigned char note, unsigned char velocity)
{
    //cout << "note off" << endl;
   
    /*if(num_press>0)num_press--;
      if(num_press==0)*/
    {
        for(k_it it = keyboards.begin();it!=keyboards.end();it++)
        {
            GMIDIkey* src = *it;
	     
            //FIXME:
            //if(src->getChan()==channel)
            //src->setGate(false);
	     
            src->noteOff(note);
        }
    }
}


void MIDIcall::control(unsigned char channel,  unsigned int param, signed int value)
{
    //cout << "control  " << param << " val: " << value << endl;
   
    for(c_it it = ctrl.begin();it!=ctrl.end();it++)
    {
        GMIDIctrl* src = *it;
	
        //comp wants to subscribe to next event
        if(src->recCtrlChan())
        {
            src->setCtrl(param); 
            src->setChan(channel);
        }
	
	
        if(src->getCtrl()==param && src->getChan()==channel)
        {
            src->setValue(SMP_MAX*value/127.0);
        }
    }
   
}


//value -8192, 8191
void MIDIcall::pitchbend(unsigned char channel,  signed int value)
{
    for(k_it it = keyboards.begin();it!=keyboards.end();it++)
    {
        GMIDIkey* src = *it;
	
        //map to 0-SSHORTMAX
        smp_t val = SMP_MAX*(value+8192)/16383.0;
        src->setPitchwheel(val);
    }
}

void MIDIcall::pgmchange(unsigned char channel,  unsigned int param, signed int value)
{
    //'value' decides pgm ID
   
    for(p_it it = pgm.begin();it!=pgm.end();it++)
    {
        GMIDIpgm* src = *it;
	
        //comp wants to subscribe to next event
        if(src->recIdChan())
        {
            src->setId(value); 
            src->setChan(channel);
        }
	
	
        if(src->getId()==value && src->getChan()==channel)
        {
            src->setOn();
        }
    }
}
