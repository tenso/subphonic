//WARNING: size is: between len and len+1 but only len is garanteed, dont try to use len+1!

namespace spl
{

template<class TYPE>
RingBuffer<TYPE>::RingBuffer(uint len)
{
    this->len=len;
    tail=0;
    head=0;
   
    //need +1 to mark full buffer(head will stand here when buff full)
    b_len=len+1;
    buffer = new TYPE[b_len];
}

template<class TYPE>
RingBuffer<TYPE>::~RingBuffer()
{
    delete[] buffer;
}

template<class TYPE>
RingBuffer<TYPE>::RingBuffer(const RingBuffer& from)
{
    tail=from.tail;
    head=from.head;
   
    len=from.len;
    b_len=from.b_len;
   
    buffer = new TYPE[b_len];
   
    memcpy(buffer, from.buffer, sizeof(TYPE)*b_len);
}

template<class TYPE>
RingBuffer<TYPE>& RingBuffer<TYPE>::operator=(const RingBuffer& from)
{
    if(this == &from)return *this;
   
    delete[] buffer;
   
    tail=from.tail;
    head=from.head;
   
    len=from.len;
    b_len=from.b_len;
   
    buffer = new TYPE[b_len];
   
    memcpy(buffer, from.buffer, sizeof(TYPE)*b_len);
   
    return *this;
}

template<class TYPE>
void RingBuffer<TYPE>::resize(uint len)
{
    if(this->len==len)return;
   
    TYPE* n_buffer = new TYPE[len+1];
   
   
    if(this->len<len)
    {
        //growing: keep head/tail as is
        memcpy(n_buffer, buffer, sizeof(TYPE)*this->len);
	
    }
    else
    {
        uint i=tail;
        uint j=0;
	
        while(i!=head && j<len)
        {
            n_buffer[j] = buffer[i];
	     
            i=(i+1)%b_len;
            j++;
        }
        tail=0;
        head=j; //this have +1 in it
    }
   
    delete[] buffer;
    buffer = n_buffer;
   
    this->len=len;
    this->b_len=len+1;
}

template<class TYPE>
void RingBuffer<TYPE>::clear()
{
    tail=0;
    head=0;
}

template<class TYPE>
bool RingBuffer<TYPE>::full()
{
    if((head+1)%b_len==tail)return true;
    return false;
}

template<class TYPE>
bool RingBuffer<TYPE>::empty()
{
    if(head==tail)return true;
    return false;
}

template<class TYPE>
bool RingBuffer<TYPE>::add(TYPE val)
{
    //if full do nothing
    if( full() )
    {
        DERROR("add to full, discarding data");
        return false;
    }
   
   
    buffer[head]=val;
    head=(head+1)%b_len;
   
    return true;
}

template<class TYPE>
TYPE RingBuffer<TYPE>::get()
{
    if(empty())
    {
        DERROR("get from empty, return garbage");
        return buffer[0];
    }
   
    TYPE ret = buffer[tail];
   
    tail=(tail+1)%b_len;
   
    return ret;
}

template<class TYPE>
bool RingBuffer<TYPE>::get(TYPE* ret)
{
    if(empty())
    {
        DERROR("get from empty, return no set");
        return false;
    }
   
    *ret = buffer[tail];
   
    tail=(tail+1)%b_len;
   
    return true;
}

template<class TYPE>
TYPE RingBuffer<TYPE>::peek(uint i) const
{
    if(empty())
    {
        DERROR("peek from empty, return garbage");
        return buffer[0];
    }
   
    if(i > inBuffer()-1)
    {
        DERROR("peek to long, return garbage");
        return buffer[0];
    }
   
   
    return buffer[(tail+i)%b_len];
}

template<class TYPE>
TYPE& RingBuffer<TYPE>::peek(uint i)
{
    if(empty())
    {
        DERROR("peek from empty, return garbage");
        return buffer[0];
    }
   
    if(i > inBuffer()-1)
    {
        DERROR("peek to long, return garbage");
        return buffer[0];
    }
   
    return buffer[(tail+i)%b_len]; //peek more than inBuffer is undefined!
}

template<class TYPE>
TYPE* RingBuffer<TYPE>::peekP(uint i)
{
    if(empty())
    {
        DERROR("peek from empty, return garbage");
        return buffer[0];
    }
   
    if(i > inBuffer()-1)
    {
        DERROR("peek to long, return garbage");
        return buffer[0];
    }
   
    return &buffer[(tail+i)%b_len]; //peek more than inBuffer is undefined!
}

template<class TYPE>
bool RingBuffer<TYPE>::peek(uint i, TYPE& ret) const
{
    if(empty())
    {
        DERROR("peek from empty, return garbage");
        return false;
    }
   
    if(i > inBuffer()-1)
    {
        DERROR("peek to long, return garbage");
        return false;
    }
   
    ret = buffer[(tail+i)%b_len]; //peek more than inBuffer is undefined!
    return true;
}


template<class TYPE>
uint RingBuffer<TYPE>::inBuffer()
{
    return (head >= tail) ? head-tail : b_len-tail+head;
}

template<class TYPE>
void RingBuffer<TYPE>::jumpFwd(uint i)
{
   
    int min = minOf(i, inBuffer());
   
    tail=(tail+min)%b_len;
}

/*
  template<class TYPE>
  TYPE& RingBuffer<TYPE>::operator[](uint i)
  {
  DASSERT(i < len);
  return buffer[i];
  }
  * 
  //TYPE* getBuffer();
  template<class TYPE>
  uint RingBuffer<TYPE>::headIndex()
  {
  return head;
  }
  template<class TYPE>
  uint RingBuffer<TYPE>::tailIndex() 
  {
  return tail;
  }
*/

template<class TYPE>
uint RingBuffer<TYPE>::size()
{
    return len;
}

}//end namespace
