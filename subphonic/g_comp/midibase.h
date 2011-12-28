#ifndef MIDIBASE_H
#define MIDIBASE_H

#include "../audio/midi.h"

#include <list>

class MIDIcall;
#include "g_midictrl.h"
#include "g_midikey.h"
#include "g_midipgm.h"


/*returns 0-SMP_MAX(1.0) to all GMIDI*::setVal..*/
class MIDIcall : public spl::MIDIcallbacks
{
  public:
    MIDIcall();
            
    virtual void noteOnOff(unsigned char channel, unsigned char note, unsigned char velocity,
    unsigned char off_velocity, unsigned int duration);
   
    //REMEMBER: 'note':  MIDI numbering
    //
    virtual void noteOn(unsigned char channel, unsigned char note, unsigned char velocity);
   
    virtual void noteOff(unsigned char channel, unsigned char note, unsigned char velocity);
   
    virtual void control(unsigned char channel,  unsigned int param, signed int value);
   
    //value -8192, 8191
    virtual void pitchbend(unsigned char channel,  signed int value);
   
    virtual void pgmchange(unsigned char channel,  unsigned int param, signed int value);
   
    int numComps();
   
    bool addKey(GMIDIkey* key);
    bool remKey(GMIDIkey* key);
   
    bool addCtrl(GMIDIctrl* c);
    bool remCtrl(GMIDIctrl* c);
   
    bool addPgm(GMIDIpgm* p);
    bool remPgm(GMIDIpgm* p);
   
  private:
    int num_press;
   
    //use list so that pointers(controller, channel) from comps can be used(they can change anytime)
    list<GMIDIctrl*> ctrl;
    list<GMIDIkey*>  keyboards;
    list<GMIDIpgm*>  pgm;
   
    typedef list<GMIDIkey*>::iterator k_it;
    typedef list<GMIDIctrl*>::iterator c_it;
    typedef list<GMIDIpgm*>::iterator p_it;
};

#endif
