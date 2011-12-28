#include "vec2.h"

#include "mathutil.h"
#include "../util/debug.h"
#include "../util/rand.h"

namespace spl{

ostream& operator<<(ostream& out, const Vec2& v)
{
    return out << v.data[0] << " " << v.data[1];
}

Vec2 operator*(const float& rh, const Vec2& lh)
{
    return Vec2(lh.data[0]*rh,
    lh.data[1]*rh);
}

bool Vec2::isNan()
{
    if(isnan(data[0]) || isnan(data[1]))return true;
    return false;
}

/*Vec2::Vec2()
  {
  data[0]=0;
  data[1]=0;
  }*/

Vec2::Vec2(const float* d)
{
    data[0]=d[0];
    data[1]=d[1];
}

Vec2::Vec2(float x, float y)
{
    data[0]=x;data[1]=y;
}

void Vec2::set(float x, float y)
{
    data[0]=x;data[1]=y;
}


float Vec2::length() const
{
    return sqrt((float)(data[0]*data[0]+data[1]*data[1]));
}

float Vec2::length_sqr() const
{
    return data[0]*data[0]+data[1]*data[1];
}

void Vec2::normalize()
{
    float l = length();
   
    D_ASSERT_M(l!=0, "normalizing vector of length 0");
    if(l==0)return;
   
    float d = 1/l;
   
    data[0]*=d;
    data[1]*=d;
    data[2]*=d;
}

Vec2 Vec2::asNormalized() const
{
    float l = length();
    D_ASSERT(l!=0);
    if(l==0)return Vec2(0,0);
   
    float d = 1/length();
   
    return Vec2(data[0]*d, data[1]*d);
}

Vec2 Vec2::abs() const
{
    return Vec2(fabs(data[0]),fabs(data[1]));
}

float* Vec2::ptr() 
{
    return data; 
}
const float* Vec2::ptr() const
{
    return data; 
}

float& Vec2::operator[](unsigned int i)
{
    D_ASSERT(i<2);
    return data[i];
}
const float Vec2::operator[](unsigned int i) const
{
    D_ASSERT(i<2);
    return data[i];
}

float& Vec2::x()
{ 
    return data[0]; 
}
float& Vec2::y()
{
    return data[1]; 
}
float Vec2::x() const
{
    return data[0]; 
}
float Vec2::y() const
{
    return data[1]; 
}

//negate
Vec2 Vec2::operator-() const
{
    return Vec2(-data[0],-data[1]);
}

//dot
float Vec2::operator*(const Vec2& rh) const
{
    return data[0]*rh.data[0]+ data[1]*rh.data[1];
}

Vec2 Vec2::operator*(const float& rh) const
{
    return Vec2(data[0]*rh,
    data[1]*rh);
}

Vec2 Vec2::operator/(const float& rh) const
{
    float rht=1/rh;
   
    return Vec2(data[0]*rht,
    data[1]*rht);
}

Vec2 Vec2::operator+(const Vec2& rh) const
{
    return Vec2(data[0]+rh.data[0],
    data[1]+rh.data[1]);
}

Vec2 Vec2::operator-(const Vec2& rh) const
{
    return Vec2(data[0]-rh.data[0],
    data[1]-rh.data[1]);
}

void Vec2::operator*=(const float& rh)
{
    data[0]*=rh;
    data[1]*=rh;
}

void Vec2::operator/=(const float& rh)
{	
    float rht = 1/rh;
    data[0]*=rht;
    data[1]*=rht;
}

void Vec2::operator+=(const Vec2& rh)
{	
    data[0]+=rh.data[0];
    data[1]+=rh.data[1];
}

void Vec2::operator-=(const Vec2& rh)
{
    data[0]-=rh.data[0];
    data[1]-=rh.data[1];
}


//EXTRA
Vec2 Vec2::rand(float min, float max)
{
    Vec2 ret;
    ret.set(min+(max-min)*Rand::next(),min+(max-min)*Rand::next());
    return ret;
}

}
