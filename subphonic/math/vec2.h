#ifndef SPL_VEC2_H
#define SPL_VEC2_H

#include <ostream>

namespace spl{

using namespace std;

class Vec2
{
    friend ostream& operator<<(ostream& out, const Vec2& v);
    friend Vec2 operator*(const float& rh, const Vec2& lh);
   
  private:
    float data[2];
   
  public:
    //Vec2(); //0,0
    Vec2(const float* d);
    explicit Vec2(float x=0, float y=0); //no coercion
   
    void set(float x=0, float y=0);
   
    bool isNan();
   
    float* ptr();
    const float* ptr() const;
    float& operator[](unsigned int i);
    const float operator[](unsigned int i) const;
    float& x();
    float& y();
    float x() const;
    float y() const;
   
    float length() const;
    float length_sqr() const;
    void  normalize();
    Vec2 asNormalized() const;
   
    Vec2 abs() const;
   
    Vec2 operator-() const;               //negate
    float operator*(const Vec2& rh) const;
    Vec2 operator*(const float& rh) const;
    Vec2 operator /(const float& rh) const;
    Vec2 operator +(const Vec2& rh) const;
    Vec2 operator-(const Vec2& rh) const;
    void operator *=(const float& rh);
    void operator/=(const float& rh);
    void operator +=(const Vec2& rh);
    void operator-=(const Vec2& rh);
   
    //EXTRA
    Vec2 rand(float min, float max);
};

ostream& operator<<(ostream& out, const Vec2& v);
Vec2 operator*(const float& rh, const Vec2& lh);

}

#endif
