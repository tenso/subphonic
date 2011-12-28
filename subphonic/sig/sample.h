#ifndef SAMPLE_H
#define SAMPLE_H

#include <string.h>
#include <iostream>

using namespace std;

#include "../util/types.h"
#include "../util/debug.h"

#include "defines.h"

namespace spl{

//FIXME:: only 16bit samples, make template
//ALSO: fixed length

//name refers to a "sampled" sound... maby not the best name...
class Sample
{
  public:
    Sample(uint len);
    ~Sample();
   
    void zero();   
   
    //make a copy of 'smp' in this
    void copy(const Sample* smp);
   
    //append 'l' samples from s->data starting at off to this
    //all offsets in samples
    bool append(const Sample* s, uint off, uint l);
   
    //i.e "add" 's' to this, mix as much as possible, i.e minLen(this,s)
    //return mixed, if clip_detect!=NULL: return number of clips>clip_at in *clip_detect
    int mix(const Sample* s, int* clip_detect=NULL, smp_t clip_at=1.0);
   
    uint getNumBytes() const;
    uint getLen() const;
   
    float maxValue();
    float maxValueInv();
   
    //address samples
    short& operator [](uint i)
    {
        DASSERT(i<len);
        return data[i];
    }
   
    short operator [](uint i) const
    {
        DASSERT(i<len);
        return data[i];
    }
   
    char* getBytes();
    short* getSmpls();
   
    //private:
    short* data;
    uint len;
    uint bytes;
};

}

#endif