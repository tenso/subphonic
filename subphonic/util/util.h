#ifndef SPL_UTIL_H
#define SPL_UTIL_H

#include <string> 
#include <sstream>
#include <ctype.h>

//#include "types.h"

namespace spl
{

template<class T>
void swap(T* x, T* y)
{
    T tmp=*x;
   
    *x=*y;
    *y=tmp;
}


template<class T> std::string toStr(T val)
{
    std::ostringstream ss;
    ss << val;
    return ss.str();
}

template<class T> T fromStr(std::string val)
{
    T ret;
   
    std::istringstream ss(val);
    ss >> ret;
   
    return ret;
}

std::string toLower(const std::string& s);
std::string toUpper(const std::string& s);

int strncasecmp(const char* s1, const char* s2, int n);

//remember fmod in math.h

}

#endif
