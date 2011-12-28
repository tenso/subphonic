#include "vec4i.h"

#include "mathutil.h"
#include "../util/debug.h"
#include "../util/rand.h"

namespace spl{

ostream& operator<<(ostream& out, const Vec4i& v)
{
    return out << v.data[0] << " " << v.data[1] << " " << v.data[2] << " " << v.data[3];
}

Vec4i operator*(const int& rh, const Vec4i& lh)
{
    return Vec4i(lh.data[0]*rh,
    lh.data[1]*rh,
    lh.data[2]*rh,
    lh.data[3]*rh);
}

bool Vec4i::isNan()
{
    if(isnan(data[0]) || isnan(data[1]) || isnan(data[2]) || isnan(data[3]))return true;
    return false;
}

/*Vec4i::Vec4i()
  {
  data[0]=0;
  data[1]=0;
  data[2]=0;
  data[3]=0;
  }*/

Vec4i::Vec4i(const int* d)
{
    data[0]=d[0];
    data[1]=d[1];
    data[2]=d[2];
    data[3]=d[3];
}

Vec4i::Vec4i(int x, int y, int z, int w)
{
    data[0]=x;data[1]=y;data[2]=z;data[3]=w;
}

Vec4i::Vec4i(const Vec3i& vec, int w)
{
    data[0]=vec[0];data[1]=vec[1];data[2]=vec[2];data[3]=w;
}

void Vec4i::set(int x, int y, int z, int w)
{
    data[0]=x;data[1]=y;data[2]=z;data[3]=w;
}

float Vec4i::length() const
{
    return sqrt((float)(data[0]*data[0]+data[1]*data[1]+data[2]*data[2]+data[3]*data[3]));
}

int Vec4i::length_sqr() const
{
    return data[0]*data[0]+data[1]*data[1]+data[2]*data[2]+data[3]*data[3];
}

void Vec4i::normalize()
{
    float l = length();
   
    D_ASSERT_M(l!=0, "normalizing vector of length 0");
    if(l==0)return;
   
    float d = 1/l;
   
    data[0]=(int)(d*data[0]);
    data[1]=(int)(d*data[0]);
    data[2]=(int)(d*data[0]);
    data[3]=(int)(d*data[0]);

}

Vec4i Vec4i::asNormalized() const
{
    float l = length();
    D_ASSERT(l!=0);
    if(l==0)return Vec4i(0,0,0,0);
   
    float d = 1.0f/length();
   
    return Vec4i((int)(data[0]*d), (int)(data[1]*d), (int)(data[2]*d), (int)(data[3]*d));
}

Vec4i Vec4i::abs() const
{
    return Vec4i(::abs(data[0]),::abs(data[1]),::abs(data[2]),::abs(data[3]));
}

int* Vec4i::ptr() 
{ 
    return data; 
}

const int* Vec4i::ptr() const
{ 
    return data; 
}


int& Vec4i::operator[](unsigned int i)
{
    D_ASSERT(i<4);
    return data[i];
}

int Vec4i::operator[](unsigned int i) const
{
    D_ASSERT(i<4);
    return data[i];
}

int& Vec4i::x() 
{
    return data[0]; 
}
int& Vec4i::y()
{
    return data[1]; 
}
int& Vec4i::z() 
{ 
    return data[2]; 
}
int& Vec4i::w()
{
    return data[3]; 
}
int Vec4i::x() const 
{
    return data[0]; 
}
int Vec4i::y() const
{
    return data[1]; 
}
int Vec4i::z() const 
{
    return data[2]; 
}
int Vec4i::w() const
{
    return data[3]; 
}
int& Vec4i::r() 
{ 
    return data[0]; 
}
int& Vec4i::g()
{
    return data[1]; 
}
int& Vec4i::b()
{
    return data[2]; 
}
int& Vec4i::a()
{ 
    return data[3]; 
}
int Vec4i::r() const 
{
    return data[0]; 
}
int Vec4i::g() const
{ 
    return data[1]; 
}
int Vec4i::b() const
{
    return data[2]; 
}
int Vec4i::a() const 
{
    return data[3]; 
}


//negate
Vec4i Vec4i::operator-() const
{
    return Vec4i(-data[0],-data[1],-data[2],-data[3]);
}


//dot
int Vec4i::operator*(const Vec4i& rh) const
{
    return data[0]*rh.data[0]+ data[1]*rh.data[1]+ data[2]*rh.data[2]+ data[3]*rh.data[3];
}

Vec4i Vec4i::operator*(const int& rh) const
{
    return Vec4i(data[0]*rh,
    data[1]*rh,
    data[2]*rh,
    data[3]*rh);
}

Vec4i Vec4i::operator /(const int& rh) const
{
    int rht=1/rh;
   
    return Vec4i(data[0]*rht,
    data[1]*rht,
    data[2]*rht,
    data[3]*rht);
}

Vec4i Vec4i::operator+(const Vec4i& rh) const
{
    return Vec4i(data[0]+rh.data[0],
    data[1]+rh.data[1],
    data[2]+rh.data[2],
    data[3]+rh.data[3]);
}

Vec4i Vec4i::operator-(const Vec4i& rh) const
{
    return Vec4i(data[0]-rh.data[0],
    data[1]-rh.data[1],
    data[2]-rh.data[2],
    data[3]-rh.data[3]);
}

void Vec4i::operator*=(const int& rh)
{
    data[0]*=rh;
    data[1]*=rh;
    data[2]*=rh;
    data[3]*=rh;
}

void Vec4i::operator/=(const int& rh)
{	
    int rht = 1/rh;
    data[0]*=rht;
    data[1]*=rht;
    data[2]*=rht;
    data[3]*=rht;
}

void Vec4i::operator+=(const Vec4i& rh)
{	
    data[0]+=rh.data[0];
    data[1]+=rh.data[1];
    data[2]+=rh.data[2];
    data[3]+=rh.data[3];
}

void Vec4i::operator-=(const Vec4i& rh)
{
    data[0]-=rh.data[0];
    data[1]-=rh.data[1];
    data[2]-=rh.data[2];
    data[3]-=rh.data[3];
}


//EXTRA
Vec4i Vec4i::rand(int min, int max)
{
    Vec4i ret;
    ret.set(Rand::nextInt(min,max),Rand::nextInt(min,max),Rand::nextInt(min,max),Rand::nextInt(min,max));
    return ret;
}

}
