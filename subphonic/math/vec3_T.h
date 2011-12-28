#include "../util/debug.h"

namespace spl{

template<class T>
ostream& operator<<(ostream& out, const Vec3T<T>& v)
{
    return out << v.data[0] << " " << v.data[1] << " " << v.data[2];
}

template<class T, class TT>
Vec3T<T> operator*(const TT& rh, const Vec3T<T>& lh)
{
    return Vec3T<T>(lh.data[0]*rh,
    lh.data[1]*rh,
    lh.data[2]*rh);
}

template<class T>
bool Vec3T<T>::isNan()
{
    if(isnan(data[0]) || isnan(data[1]) || isnan(data[2]))return true;
    return false;
}

/*Vec3T<T>::Vec3T()
  {
  data[0]=0;data[1]=0;data[2]=0;
  }*/

template<class T>
Vec3T<T>::Vec3T(const T* d)
{
    data[0]=d[0];
    data[1]=d[1];
    data[2]=d[2];
}

template<class T>
Vec3T<T>::Vec3T(T x, T y, T z)
{
    data[0]=x;data[1]=y;data[2]=z;
}

template<class T>
void Vec3T<T>::set(T x, T y, T z)
{
    data[0]=x;data[1]=y;data[2]=z;
}

template<class T>
T Vec3T<T>::length() const
{
    return sqrt(data[0]*data[0]+data[1]*data[1]+data[2]*data[2]);
}

template<class T>
T Vec3T<T>::length_sqr() const
{
    return data[0]*data[0]+data[1]*data[1]+data[2]*data[2];
}

template<class T>
void Vec3T<T>::normalize()
{
    T l = length();
   
    D_ASSERT_M(l!=0, "normalizing vector of length 0");
    if(l==0)return;
   
    T d = 1/l;
    data[0]*=d;
    data[1]*=d;
    data[2]*=d;
}

template<class T>
Vec3T<T> Vec3T<T>::asNormalized() const
{
    T l = length();
    D_ASSERT(l!=0);
    if(l==0)return Vec3T(0,0,0);
   
    T d = 1/length();
   
    return Vec3T<T>(data[0]*d, data[1]*d, data[2]*d);
}

template<class T>
T* Vec3T<T>::ptr()
{ 
    return data; 
}

template<class T>
const T* Vec3T<T>::ptr() const
{ 
    return data; 
}

template<class T>
T& Vec3T<T>::operator[](unsigned int i)
{
    D_ASSERT(i<3);
    return data[i];
}

template<class T>
const T Vec3T<T>::operator[](unsigned int i) const
{
    D_ASSERT(i<3);
    return data[i];
}
template<class T>
T& Vec3T<T>::x() { return data[0]; }

template<class T>
T& Vec3T<T>::y() {	return data[1]; }

template<class T>
T& Vec3T<T>::z() { return data[2]; }

template<class T>
T Vec3T<T>::x() const { return data[0]; }

template<class T>
T Vec3T<T>::y() const { return data[1]; }

template<class T>
T Vec3T<T>::z() const { return data[2]; }

template<class T>
Vec3T<T> Vec3T<T>::abs() const
{
    return Vec3T<T>(fabs(data[0]),fabs(data[1]),fabs(data[2]));
}


//cross
template<class T>
Vec3T<T> Vec3T<T>::operator^(const Vec3T<T>& rh) const
{
    return Vec3T<T>(data[1]*rh.data[2]-data[2]*rh.data[1],
    data[2]*rh.data[0]-data[0]*rh.data[2],
    data[0]*rh.data[1]-data[1]*rh.data[0]);
}

//negate
template<class T>
Vec3T<T> Vec3T<T>::operator-() const
{
    return Vec3T<T>(-data[0],-data[1],-data[2]);
}


//dot
template<class T>
T Vec3T<T>::operator*(const Vec3T<T>& rh) const
{
    return data[0]*rh.data[0]+ data[1]*rh.data[1]+ data[2]*rh.data[2];
}

template<class T>
Vec3T<T> Vec3T<T>::operator*(const T& rh) const
{
    return Vec3T<T>(data[0]*rh,
    data[1]*rh,
    data[2]*rh);
}

template<class T>
Vec3T<T> Vec3T<T>::operator/(const T& rh) const
{
    T rht=1/rh;
   
    return Vec3T<T>(data[0]*rht,
    data[1]*rht,
    data[2]*rht);
}

template<class T>
Vec3T<T> Vec3T<T>::operator+(const Vec3T<T>& rh) const
{
    return Vec3T<T>(data[0]+rh.data[0],
    data[1]+rh.data[1],
    data[2]+rh.data[2]);
}

template<class T>
Vec3T<T> Vec3T<T>::operator-(const Vec3T<T>& rh) const
{
    return Vec3T<T>(data[0]-rh.data[0],
    data[1]-rh.data[1],
    data[2]-rh.data[2]);
}

template<class T>
void Vec3T<T>::operator^=(const Vec3T<T>& rh)
{
    Vec3T<T> tmp(data[1]*rh.data[2]-data[2]*rh.data[1],
    data[2]*rh.data[0]-data[0]*rh.data[2],
    data[0]*rh.data[1]-data[1]*rh.data[0]);
   
    this->data[0]=tmp.data[0];
    this->data[1]=tmp.data[1];
    this->data[2]=tmp.data[2];
}

template<class T>
void Vec3T<T>::operator*=(const T& rh)
{
    data[0]*=rh;
    data[1]*=rh;
    data[2]*=rh;
}

template<class T>
void Vec3T<T>::operator/=(const T& rh)
{	
    T rht = 1/rh;
    data[0]*=rht;
    data[1]*=rht;
    data[2]*=rht;
}

template<class T>
void Vec3T<T>::operator+=(const Vec3T<T>& rh)
{	
    data[0]+=rh.data[0];
    data[1]+=rh.data[1];
    data[2]+=rh.data[2];
}

template<class T>
void Vec3T<T>::operator-=(const Vec3T<T>& rh)
{
    data[0]-=rh.data[0];
    data[1]-=rh.data[1];
    data[2]-=rh.data[2];
}

//EXTRA
template<class T>
Vec3T<T> Vec3T<T>::rand(T min, T max)
{
    Vec3T<T> ret;
    ret.set(min+(max-min)*Rand::next(),min+(max-min)*Rand::next(),min+(max-min)*Rand::next());
    return ret;
}

}
