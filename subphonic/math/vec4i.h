#ifndef SPL_VEC4I_H
#define SPL_VEC4I_H

#include <ostream>

#include "vec3.h"

namespace spl{

using namespace std;

class Vec4i
{
    friend ostream& operator<<(ostream& out, const Vec4i& v);
    friend Vec4i operator*(const int& rh, const Vec4i& lh);
   
  private:
    int data[4];
   
  public:
    //Vec4i(); //0,0,0,0
    Vec4i(const int* d);
    explicit Vec4i(int x=0, int y=0, int z=0, int w=0);
    Vec4i(const Vec3i& vec, int w=0);
    void set(int x=0, int y=0, int z=0, int w=0);
   
    bool isNan();
   
    int* ptr();
    const int* ptr() const;
    int& operator[](unsigned int i);
    int operator[](unsigned int i) const;
    int& x();
    int& y();
    int& z();
    int& w();
    int x() const;
    int y() const;
    int z() const;
    int w() const;
    int& r();
    int& g();
    int& b();
    int& a();
    int r() const;
    int g() const;
    int b() const;
    int a() const;
   
    float length() const;
    int length_sqr() const;
    void  normalize(); //don't make sense
    Vec4i asNormalized() const;
    Vec4i abs() const;
   
    Vec4i operator-() const;                //negate
    int operator*(const Vec4i& rh) const;
    Vec4i operator*(const int& rh) const;
    Vec4i operator/(const int& rh) const;
    Vec4i operator+(const Vec4i& rh) const;
    Vec4i operator-(const Vec4i& rh) const;
    void operator*=(const int& rh);
    void operator/=(const int& rh);
    void operator+=(const Vec4i& rh);   
    void operator-=(const Vec4i& rh);
   
    //EXTRA
    Vec4i rand(int min, int max);
};

ostream& operator<<(ostream& out, const Vec4i& v);
Vec4i operator*(const int& rh, const Vec4i& lh);

}

#endif
