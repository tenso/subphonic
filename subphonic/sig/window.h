#ifndef WINDOW_H
# define WINDOW_H

#include "defines.h"

//FIXME: names w_ or something

namespace spl{

template<class T=smp_t>
class Window
{
  public:
    enum TYPE {BLACKMAN, HAMMING, HANN, RECTANGLE, TRIANGLE};
   
    Window(TYPE type, uint len);
    ~Window();
   
    Window(const Window& f);
    Window& operator=(const Window& r);
   
    uint getLen() const;
   
    //WARNING: unchecked op's
    T& operator[](uint i);
    T operator[](uint i) const;
   
    T& get(uint i);
    T get(uint i) const;
   
  private:
   
    T* data;
    uint len;
   
};

//bell from 0 to 1 to 0
template<class T>
void win_blackman(T* data, uint len);

//"hann & hamming"
//bell from 0 to 1 to 0
template<class T>
void win_hann(T* data, uint len);

//bell from about 0.1 to 1 to 0.1
template<class T>
void win_hamming(T* data, uint len);

template<class T>
void win_rectangle(T* data, uint len);

template<class T>
void win_triangle(T* data, uint len);

//}

# include "window_T.h"

}

#endif
