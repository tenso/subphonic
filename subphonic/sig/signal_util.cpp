#include "signal_util.h"
#include "../math/mathutil.h"

namespace spl{

using namespace std;

smp_t tri(smp_t w, smp_t shape)
{
    //remove all multiples of 2PI:
    int m = (int)(floor(w/M_2PI));
    smp_t arg = w - M_2PI*m;
   
    if(arg<0)arg = M_2PI+arg;
    DASSERT(arg>=0 && arg<=M_2PI);

    smp_t sp = shape*M_2PI;
   
    if(arg<sp)
    {
        return 1.0 - 2.0*arg/sp;
    }
    else 
    {
        if(!feq(sp, (smp_t)0.0))return -1.0 + 2.0*(arg-sp)/(M_2PI-sp);
        else return -1.0 + 2.0*arg/M_2PI;
    }
}

smp_t squ(smp_t w, smp_t shape)
{
    //remove all multiples of 2PI:
    int m = (int)(floor(w/M_2PI));
    smp_t arg = w - M_2PI*m;
   
    if(arg<0)arg = M_2PI+arg;
    DASSERT(arg>=0 && arg<=M_2PI);

    smp_t sp = shape*M_2PI;
   
    if(arg<sp)
    {
        return 1.0;
    }
    else 
    {
        return -1.0;
    }
}


smp_t pul(smp_t w, smp_t shape)
{
    //remove all multiples of 2PI:
    int m = (int)(floor(w/M_2PI));
    smp_t arg = w - M_2PI*m;
   
    if(arg<0)arg = M_2PI+arg;
    DASSERT(arg>=0 && arg<=M_2PI);

    smp_t sp = shape*M_2PI;
   
    if(feq(arg,sp, (smp_t)0.00001) || arg < sp)
    {
        return 1.0;
    }
    else 
    {
        return 0;
    }
}


smp_t noteMap(int key, int octave)
{
    return noteHz(key, octave);
}
int mapNote(smp_t hz)
{
    return hzNote(hz);
}

int ramp(int val)
{
    if(val<0)return 0;
    else return val;
}

int step(int val)
{
    if(val<0)return 0;
    else return 1;
}

std::string noteStr(int key, int octave)
{
    //static ok: const, otherwise static is NOT ok: no thread safe
    static const char key_str[12][3]= {"A-","A#","B-","C-","C#","D-","D#","E-","F-","F#","G-","G#"};
   
    if((key-1)%12>=3)octave+=1;
   
    stringstream ss;
    ss << octave+(key-1)/12;
   
   
    string ret;
   
    int look = (key-1)%12;
    if(look<0)look=12+look;
    ret.assign(key_str[look]);
    ret+= ss.str();
   
    return ret;
}

int strNote(const std::string& str)
{
    static const char key_str[12][3]= {"A-","A#","B-","C-","C#","D-","D#","E-","F-","F#","G-","G#"};
   
    int key=0;
    bool found=false;
    for(uint i=0;i<12;i++)
    {
        if(!strncmp(key_str[i], str.c_str(), 2))
        {
            key=i+1;
            found=true;
            break;
        }
    }
   
    if(!found)
    {
        cout << "strNote(): format error: " << str  << endl;
        return key;
    }
      
    string sub;
    sub = str.substr(2);
   
    //expensive to create
    istringstream ss;
    ss.clear();
    ss.str(sub);
   
    int oct=0;
    ss >> oct;
    if(ss.fail())
    {
        cout << "strNote(): format error, ss failed: " << str << "->" << oct << endl;
        return key;
    }
   
    if((key-1)%12>=3)oct-=1;
   
   
    return key+oct*12;
}


smp_t noteHz(int key, int octave)
{
   
    key = octave*12+key;
   
    return notemap_ref_hz*powf(2.0,(key-notemap_ref_key)/12.0f);
}

int hzNote(smp_t hz)
{
    //round is important
    return (int)(notemap_ref_key + round(12.0*log2(hz/notemap_ref_hz)));
}

int pianoToMidi(int key)
{
    return key+20;
}

int midiToPiano(int key)
{
    return key-20;
}


/*
  smp_t noteMapLinear(int octave, int key, smp_t first_key_hz)
  {
  if(key<0)
  {
  octave += -1+key/12;
  key=12+key%12;
  }
  if(key>11)
  {
  octave += key/12;
  key=key%12;
  }
  if(octave<0)octave=0;
   
  smp_t base = first_key_hz*(1<<(octave-1));
  smp_t slope = base/12.0;
   
  return base+(slope*key);
  }*/

Convert::Convert(int smprate)
{
    smplrate = smprate;
    hsmplrate=smplrate>>1;
}

smp_t Convert::hzToPeriod(smp_t hz) const
{
    if(feq(hz, (smp_t)0.0))return 0.0;
   
    return smplrate/hz;
}

int Convert::secToLen(smp_t sec) const
{
    return (int)(sec*smplrate);
}   

int Convert::sampleRate() const
{
    return smplrate;
}

int Convert::halfSampleRate() const
{
    return hsmplrate;
}

}