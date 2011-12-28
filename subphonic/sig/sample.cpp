#include "sample.h"

#include "../math/mathutil.h"

namespace spl{

Sample::Sample(uint len)
{
    this->len = len;
    data = new short[len];
    bytes = len*2;
}

Sample::~Sample()
{
    delete[] data;
    data = NULL;
}

void Sample::zero()
{
    for(uint i=0;i<len;i++)data[i]=0;
}

int Sample::mix(const Sample* s, int* clip_detect, smp_t clip_at)
{
    uint min = spl::minOf(len, s->getLen());
    for(uint i=0;i<len;i++)
    {

        if(clip_detect!=NULL)
        {
            smp_t val = data[i]+s->data[i];
            if(fabs(val)>=clip_at)
            {
                (*clip_detect)++;
            }
            data[i]=val;
        }
        else data[i]+=s->data[i];
    }
    return min;
}


void Sample::copy(const Sample* smp)
{
    if(len!=smp->len)
    {
        delete[] data;
        len = smp->len;
        data = new short[len];
        bytes = len*2;
    }
    memcpy(data,smp->data,bytes);
}

bool Sample::append(const Sample* s, uint off, uint l)
{
    if(off+l>len)return false;
    if(l*2>s->getNumBytes())return false;
   
    /*DASSERT(off+l>len);
      DASSERT(l*2<=s->getNumBytes());*/
   
    memcpy(&data[off], s->data, l*2);
   
    return true;
}

uint Sample::getNumBytes() const
{
    return bytes;
}


uint Sample::getLen() const
{
    return len;
}

float Sample::maxValue()
{
    return SSHORT_MAX;
}

float Sample::maxValueInv()
{
    return 1.0f/(float)SSHORT_MAX;
}

char* Sample::getBytes()
{
    return (char*)data;
}

short* Sample::getSmpls()
{
    return data;
}

}