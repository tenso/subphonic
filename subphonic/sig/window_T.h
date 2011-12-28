#include "window.h"

template<class T>
Window<T>::Window(TYPE type, uint len)
{
    this->len=len;
    data=NULL;
    data = new T[len];
   
    switch(type)
    {
        case BLACKMAN:
            win_blackman(data,len);
            break;
	
        case HAMMING:
            win_hamming(data,len);
            break;
	
        case HANN:
            win_hann(data,len);
            break;
	
        case RECTANGLE:
            win_rectangle(data,len);
            break;
	
        case TRIANGLE:
            win_triangle(data,len);
            break;
	
        default:
            DERROR("no such type, making triangle");
            win_triangle(data,len);
            break;
    }
   
}

template<class T>
Window<T>::~Window()
{
    delete data;
}

template<class T>
Window<T>::Window(const Window& f)
{
    len=f.len;
    data = new T[len];
    memcpy(data, f.data, sizeof(T)*len);
}


template<class T>
Window<T>& Window<T>::operator=(const Window& r)
{
    if(this == &r)return *this;
   
    delete[] data;
   
    len=r.len;
    data = new T[len];
    memcpy(data, r.data, sizeof(T)*len);
   
    return *this;
}


template<class T>
uint Window<T>::getLen() const
{
    return len;
}

template<class T>
T& Window<T>::operator[](uint i)
{
    DASSERT(i<len);
    return data[i];
}

template<class T>
T Window<T>::operator[](uint i) const
{
    DASSERT(i<len);
    return data[i];
}

template<class T>
T& Window<T>::get(uint i)
{
    return data[i];
}

template<class T>
T Window<T>::get(uint i) const
{
    return data[i];
}

template<class T>
void win_blackman(T* data, uint len)
{
    for(uint i=0;i<len;i++)
    {
        data[i] = 0.42-0.5*cos(2*M_PI*i/(T)(len-1))+0.08*cos(4*M_PI*i/(T)(len-1));
    }
}
template<class T>
void win_hamming(T* data, uint len)
{
    for(uint i=0;i<len;i++)
    {
        data[i] = 0.53836-0.46164*cos(2*M_PI*i/(T)(len-1));
    }
}

template<class T>
void win_hann(T* data, uint len)
{
    for(uint i=0;i<len;i++)
    {
        data[i] = 0.5*(1.0-cos(2*M_PI*i/(T)(len-1)));
    }
}


template<class T>
void win_rectangle(T* data, uint len)
{
    for(uint i=0;i<len;i++)
    {
        data[i] = 1.0;
    }
}

template<class T>
void win_triangle(T* data, uint len)
{
    T d = 2.0/(T)(len);
    uint NH = len/2;
    T x=0;
   
    for(uint i=0;i<len;i++)
    {
        data[i] = x;
	
        if(i<NH)x+=d;
        else x-=d;
    }
}
