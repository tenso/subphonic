#ifndef SPL_RINGBUFFER_H
# define SPL_RINGBUFFER_H

#include <iostream>

#include "util.h"
#include "debug.h"

//using namespace std;

//FIXME: checkout and fixthis

namespace spl
{

//FIFO
template<class TYPE>
class RingBuffer 
{
  public:
    //if len==0 (why?) it's both empty() and full()
    RingBuffer(uint len);
    ~RingBuffer();
   
    RingBuffer(const RingBuffer& from);
    RingBuffer& operator=(const RingBuffer& from);
   
    //when growing: keeps data as is
    //FIXME: maybe keep closer to head?
    //when shrinking: saves "oldest" added data(i.e starting from tail going as much as possible towards head)
    void resize(uint len);
   
    void clear();
   
    //add at head, adding to fullbuffer does nothing & returns false
    //return true if added
    bool add(TYPE val);
   
    TYPE get(); //get at tail, get from empty returns garbage
    bool get(TYPE* ret);
     
    //peek at offset i from tail, peek more than inBuffer()-1 and get junk
    //i.e peek(0) is tail peek(inBuffer()-1) is head
    TYPE peek(uint i) const;
    TYPE& peek(uint i);
    TYPE* peekP(uint i);
   
    //error checks
    bool peek(uint i, TYPE& ret) const;
   
    //same as 'i' number of get()
    void jump(uint i)
    {
        jumpFwd(i);
    }
   
    void jumpFwd(uint i); //truncated jump, i.e never more than to head
   
    uint size(); //i.e 'len'
    uint inBuffer(); //i.e number of elms currently in buffer(not 'len')
   
    bool full();
    bool empty();
   
  private:
    TYPE* buffer;
    uint head;
    uint tail; //need to be signed
    uint len;
    uint b_len;
};

}

#include "ringbuffer_T.h"

#endif
