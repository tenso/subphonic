#include "fft.h"

namespace spl
{

int bitReverse(int num, int len)
{
    //'len' is number of bits
    //assume len is a power of 2
    //runs in O(len/2+1)
    len--;
    int leftMask=1<<len;
    int rightMask=1;
   
    while(rightMask<=leftMask)
    {
        if(num&leftMask)
        {
            if(!(num&rightMask))num^=leftMask|rightMask;
        }
        else
        {
            if(num&rightMask)num^=leftMask|rightMask;
        }
        rightMask<<=1;
        leftMask>>=1;
    }
    return num;
}

}
