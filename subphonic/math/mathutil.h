#ifndef SPL_MATHUTIL_H
# define SPL_MATHUTIL_H

//# include "../util/util.h"

#define _USE_MATH_DEFINES
#include <math.h>

//#ifdef WIN32
#include <float.h>
#define isnan _isnan
#define FALSE 0
#define TRUE 1
//#endif

namespace spl{

bool isfinite(double num);
double log2(double num);
double round(double val);

template<class T>
bool feq(T x, T y, T th=1e-10) //FIXME: def th good?
{
    if( x >= y-th && x <= y+th)return true;
    return false;
}

template<class T>
T minOf(const T& x, const T& y)
{
    return (x<y) ? x : y;
}

template<class T>
T maxOf(const T& x, const T& y)
{
    return (x>y) ? x : y;
}

template<class T>
int sign(T v)
{
    if(v>=0)return 1;
    return -1;
}

}

#endif
