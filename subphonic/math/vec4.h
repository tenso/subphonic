//FIRST TODO: make template as in Vec3T

#ifndef SPL_VEC4_H
#define SPL_VEC4_H

#include <ostream>

#include "vec3.h"

namespace spl{

using namespace std;

class Vec4
{
    friend ostream& operator<<(ostream& out, const Vec4& v);
    friend Vec4 operator*(const float& rh, const Vec4& lh);
   
  private:
    float data[4];
   
  public:
    //Vec4(); //0,0,0,0
    Vec4(const float* d);
    explicit Vec4(float x=0, float y=0, float z=0, float w=0);
    Vec4(const Vec3& vec, float w=0);
   
    void set(float x=0, float y=0, float z=0, float w=0);
   
    bool isNan();
   
    float* ptr();
    const float* ptr() const;
    float& operator[](unsigned int i);
    float operator[](unsigned int i) const;
    float& x();
    float& y();
    float& z();
    float& w();
    float x() const;
    float y() const;
    float z() const;
    float w() const;
    float& r();
    float& g();
    float& b();
    float& a();
    float r() const;
    float g() const;
    float b() const;
    float a() const;
   
    float length() const;
    float length_sqr() const;
    void  normalize();
    Vec4 asNormalized() const;
    Vec4 abs() const;
   
    Vec4 operator-() const;                //negate
    float operator*(const Vec4& rh) const;
    Vec4 operator*(const float& rh) const;
    Vec4 operator/(const float& rh) const;
    Vec4 operator+(const Vec4& rh) const;
    Vec4 operator-(const Vec4& rh) const;
    void operator*=(const float& rh);
    void operator/=(const float& rh);
    void operator+=(const Vec4& rh);   
    void operator-=(const Vec4& rh);
   
    //EXTRA
    static Vec4 rand(float min=0.0f, float max=1.0f);
};

ostream& operator<<(ostream& out, const Vec4& v);
Vec4 operator*(const float& rh, const Vec4& lh);

}

#endif
