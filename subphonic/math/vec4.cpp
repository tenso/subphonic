#include "vec4.h"

#include "mathutil.h"
#include "../util/debug.h"
#include "../util/rand.h"

namespace spl{

ostream& operator<<(ostream& out, const Vec4& v)
{
    return out << v.data[0] << " " << v.data[1] << " " << v.data[2] << " " << v.data[3];
}

Vec4 operator*(const float& rh, const Vec4& lh)
{
    return Vec4(lh.data[0]*rh,
    lh.data[1]*rh,
    lh.data[2]*rh,
    lh.data[3]*rh);
}

bool Vec4::isNan()
{
    if(isnan(data[0]) || isnan(data[1]) || isnan(data[2]) || isnan(data[3]))return true;
    return false;
}

/*Vec4::Vec4()
  {
  data[0]=0;
  data[1]=0;
  data[2]=0;
  data[3]=0;
  }*/

Vec4::Vec4(const float* d)
{
    data[0]=d[0];
    data[1]=d[1];
    data[2]=d[2];
    data[3]=d[3];
}

Vec4::Vec4(float x, float y, float z, float w)
{
    data[0]=x;data[1]=y;data[2]=z;data[3]=w;
}

Vec4::Vec4(const Vec3& vec, float w)
{
    data[0]=vec[0];data[1]=vec[1];data[2]=vec[2];data[3]=w;
}

void Vec4::set(float x, float y, float z, float w)
{
    data[0]=x;data[1]=y;data[2]=z;data[3]=w;
}

float Vec4::length() const
{
    return sqrt(data[0]*data[0]+data[1]*data[1]+data[2]*data[2]+data[3]*data[3]);
}

float Vec4::length_sqr() const
{
    return data[0]*data[0]+data[1]*data[1]+data[2]*data[2]+data[3]*data[3];
}

void Vec4::normalize()
{
    float l = length();
   
    D_ASSERT_M(l!=0, "normalizing vector of length 0");
    if(l==0)return;
   
    float d = 1/l;
   
    data[0]*=d;
    data[1]*=d;
    data[2]*=d;
    data[3]*=d;
}

Vec4 Vec4::asNormalized() const
{
    float l = length();
    D_ASSERT(l!=0);
    if(l==0)return Vec4(0,0,0,0);
   
    float d = 1.0f/length();
   
    return Vec4(data[0]*d, data[1]*d, data[2]*d, data[3]*d);
}

Vec4 Vec4::abs() const
{
    return Vec4(fabs(data[0]),fabs(data[1]),fabs(data[2]),fabs(data[3]));
}

float* Vec4::ptr() 
{ 
    return data; 
}

const float* Vec4::ptr() const
{ 
    return data; 
}


float& Vec4::operator[](unsigned int i)
{
    D_ASSERT(i<4);
    return data[i];
}

float Vec4::operator[](unsigned int i) const
{
    D_ASSERT(i<4);
    return data[i];
}

float& Vec4::x() 
{
    return data[0]; 
}
float& Vec4::y()
{
    return data[1]; 
}
float& Vec4::z() 
{ 
    return data[2]; 
}
float& Vec4::w()
{
    return data[3]; 
}
float Vec4::x() const 
{
    return data[0]; 
}
float Vec4::y() const
{
    return data[1]; 
}
float Vec4::z() const 
{
    return data[2]; 
}
float Vec4::w() const
{
    return data[3]; 
}
float& Vec4::r() 
{ 
    return data[0]; 
}
float& Vec4::g()
{
    return data[1]; 
}
float& Vec4::b()
{
    return data[2]; 
}
float& Vec4::a()
{ 
    return data[3]; 
}
float Vec4::r() const 
{
    return data[0]; 
}
float Vec4::g() const
{ 
    return data[1]; 
}
float Vec4::b() const
{
    return data[2]; 
}
float Vec4::a() const 
{
    return data[3]; 
}


//negate
Vec4 Vec4::operator-() const
{
    return Vec4(-data[0],-data[1],-data[2],-data[3]);
}


//dot
float Vec4::operator*(const Vec4& rh) const
{
    return data[0]*rh.data[0]+ data[1]*rh.data[1]+ data[2]*rh.data[2]+ data[3]*rh.data[3];
}

Vec4 Vec4::operator*(const float& rh) const
{
    return Vec4(data[0]*rh,
    data[1]*rh,
    data[2]*rh,
    data[3]*rh);
}

Vec4 Vec4::operator /(const float& rh) const
{
    float rht=1/rh;
   
    return Vec4(data[0]*rht,
    data[1]*rht,
    data[2]*rht,
    data[3]*rht);
}

Vec4 Vec4::operator+(const Vec4& rh) const
{
    return Vec4(data[0]+rh.data[0],
    data[1]+rh.data[1],
    data[2]+rh.data[2],
    data[3]+rh.data[3]);
}

Vec4 Vec4::operator-(const Vec4& rh) const
{
    return Vec4(data[0]-rh.data[0],
    data[1]-rh.data[1],
    data[2]-rh.data[2],
    data[3]-rh.data[3]);
}

void Vec4::operator*=(const float& rh)
{
    data[0]*=rh;
    data[1]*=rh;
    data[2]*=rh;
    data[3]*=rh;
}

void Vec4::operator/=(const float& rh)
{	
    float rht = 1/rh;
    data[0]*=rht;
    data[1]*=rht;
    data[2]*=rht;
    data[3]*=rht;
}

void Vec4::operator+=(const Vec4& rh)
{	
    data[0]+=rh.data[0];
    data[1]+=rh.data[1];
    data[2]+=rh.data[2];
    data[3]+=rh.data[3];
}

void Vec4::operator-=(const Vec4& rh)
{
    data[0]-=rh.data[0];
    data[1]-=rh.data[1];
    data[2]-=rh.data[2];
    data[3]-=rh.data[3];
}


//EXTRA
Vec4 Vec4::rand(float min, float max)
{
    Vec4 ret;
    ret.set(min+(max-min)*Rand::next(),min+(max-min)*Rand::next(),
    min+(max-min)*Rand::next(),min+(max-min)*Rand::next());
    return ret;
}

}
