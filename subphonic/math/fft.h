#ifndef SPL_FFT_H
#define SPL_FFT_H

#include "../util/types.h"
#include "../util/debug.h"
#include "complex.h"
//FIXME: len fix?

namespace spl
{

int bitReverse(int num, int len);

//WARNING:
//len = power of 2!
//runs in O(n*lg(n))

template<class T>
void fft(T* in, spl::Complex* out, int len);

template<class T>
void ifft(spl::Complex* in, T* out, int len);



/*MEMORY/CYCLE tradeoff: "optimized" but also FIXED length.
  Memory: 2 * len * log_2(len) * sizeof(smp_t)
  
  so this should probably not be used for anything other 
  than realtime i.e small 'len'.
  
  len=8196 => 208*4 kb memory: ~1MB*/

template<class T>
class FFTfixed
{
  public:
    FFTfixed();
    ~FFTfixed();
   
    //DONT FORGET
    void setup(uint len);
    void free();
   
    //WARNING: input/output better be 'len' long
    void fft(T* in, spl::Complex* out) const;
    void ifft(spl::Complex* in, T* out) const;
   
    uint getBytesUsed() const;
   
  private:
    uint mem_usage;
    uint len;
    uint loglen;
    spl::Complex** pos_e;
    spl::Complex** neg_e;
};

#include "fft_T.h"

}

#endif
