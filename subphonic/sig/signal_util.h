#ifndef SIG_UTIL_H
#define SIG_UTIL_H

#include <math.h>

#include <iostream>
#include <string>
#include <sstream>
#include <string.h>

#include "../util/debug.h"
#include "../util/types.h"

#include "defines.h"

namespace spl{

int ramp(int val);
int step(int val);

//all waveforms use radians; periodic with 2pi, and range -1, 1, all start high(1.0)
//neative argument 'w' ok

//traingle waveform: periodic with 2pi, 
//shape [0 1] s=0.5 is a triangle s=0 is a upward going saw s=1 is a downward going saw
//acts like cos in that tri(0) = 1 tri(pi/2) = 0 ...
//use this for saw, shape=0,1
smp_t tri(smp_t w, smp_t shape=0.5);

//shape decides high/low ratio shape=0 => only low shape=1 => only high
smp_t squ(smp_t w, smp_t shape=0.5);

//shape decides high/low ratio shape=0 => only low(0) shape=1 => only high(1)
//different than use of (squ() + 1.0)/2.0 for a pulse:
//if shape is=0 w=0 still gives a 1
smp_t pul(smp_t w, smp_t shape);


/*
  standard piano "tuning" i.e key=1,2,3,4,5 is A0,A#0, B0, C1, C#1 etc
  ocatve is seen as number of 12*notes from A0, (is this the convention?)
  ex: oct=1 key=1 is A1 oct=1, key=4 is C2 (because oct=0, key=4 is C1)
  * FIXME: should oct=1, key=4 be C1 ? now noteMap(4,0) = C1 etc
  
  reference key is 49 for 440hz (A4)
  
  in octave:0,1,...,N key:1,2,...,M
  out: hz
*/

static const smp_t notemap_ref_hz=440.0;
static const int notemap_ref_key=49;

smp_t noteHz(int key, int octave=0);

//reverses noteMap(), truncated return, roundoff some errors so 466.163 could become key=50, 466.164hz
//not a problem if only exact 'hz' are entered(e.g from noteMap)
int hzNote(smp_t hz);


//FIXME: remove
smp_t noteMap(int key, int octave=0);
int mapNote(smp_t hz);

int pianoToMidi(int key);
int midiToPiano(int key);



//returns a string in the format "C#4" etc
std::string noteStr(int key, int octave=0);

//reverse noteStr, octave is lost, (key-1)%12 to get key, (key-1)/12 to get octave
int strNote(const std::string& str);

//FIXME: remove?
//smp_t noteMapLinear(int octave, int key, smp_t first_key_hz);

class Convert
{
  public:
    Convert(int smprate);

    smp_t hzToPeriod(smp_t hz) const;
   
    //in samples
    int secToLen(smp_t sec) const;
      
    int sampleRate() const;
    int halfSampleRate() const;
   
  private:
    int smplrate;
    int hsmplrate;
};

}

#endif
