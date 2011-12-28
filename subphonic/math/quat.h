#ifndef SPL_QUAT_H
#define SPL_QUAT_H

#include "matrix.h"
#include "vec3.h"
#include "vec4.h"

namespace spl {

class Quat;

ostream& operator<<(ostream& out, const Quat& v);
Quat operator*(const float& lh, const Quat& q);
Quat operator*(const Vec4& rh, const Quat& q);
Quat operator*(const Vec3& rh, const Quat& q);

class Quat
{
  public:
    Quat();
    Quat(float x, float y, float z, float w);
    Quat(const Vec3& vec, float w);
    Quat(const Vec4& vec);
   
    void set(float x=0, float y=0, float z=0, float w=0);
    void set(const Vec3& vec, float w=0);
    void set(const Vec4& vec);
   
    inline float& operator[](int index);
    inline float operator[](int index) const;
   
    inline float& x();
    inline float& y();
    inline float& z();
    inline float& w();
    inline float x() const;
    inline float y() const;
    inline float z() const;
    inline float w() const;
      
    Quat operator*(const float& rh) const;
    Quat operator/(const float& rh) const;
    Quat operator+(const Quat& rh) const;
    void operator+=(const Quat& rh);
    Quat operator-(const Quat& rh) const;
    void operator-=(const Quat& rh);
      
    //if rotation and q2*q1 => rotate by q1 then q2
    Quat operator*(const Quat& rh) const;
    void operator*=(const Quat& rh);
    Quat operator*(const Vec4& rh) const; //treat Vec4 as a quat
    Quat operator*(const Vec3& rh) const; //treat as [x,y,z,0]
   
   
    //set this to a rotation by 'rad' radians about'axis'
    void setRotate(float x, float y, float z, float rad);
    void setRotate(const Vec3& axis, float rad);
   
    //same as above but return a new quat
    static Quat rotate(const Vec3& axis, float rad);
    static Quat rotate(float x, float y, float z, float rad);
   
   
    Matrix asMatrix();
   
   
    //rotates 'vec' by this quaternion 'q', i.e performes: q*quat(vec,0)*(q.inverse())
    Vec3 rotateVec(const Vec3& vec);
   
    void normalize();   
    inline float length() const;
    inline float length_sqr() const;

    Quat inverse() const;
    Quat conjugate() const;
   
    Quat slerp(float t, const Quat& to); //returns quat 't' slerps  between this and 'to'
    
  private:
    float data[4]; //saved as [i j k w]
   
    friend ostream& spl::operator<<(ostream& out, const Quat& v);
    friend Quat spl::operator*(const float& lh, const Quat& q);
    friend Quat spl::operator*(const Vec4& rh, const Quat& q); //should these exist?
    friend Quat spl::operator*(const Vec3& rh, const Quat& q);
};

}

#endif
