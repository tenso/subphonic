#ifndef SPL_VEC3_H
#define SPL_VEC3_H

#include <ostream>

#include "../util/rand.h"

namespace spl{

using namespace std;

//WARNING: Vec3T<int>::normalize, length etc does not make sence

template<class T>
class Vec3T
{
    template<class TT, class TTT>
    friend Vec3T<TT> operator*(const TTT& rh, const Vec3T<TT>& lh);
   
    template<class TT>
    friend ostream& operator<<(ostream& out, const Vec3T<TT>& v);
   
  private:
    T data[3];

  public:
    //Vec3T(); //0,0,0
    Vec3T(const T* d);
    //explicit - must prevent automatic coercions as Vec3 a=1.0f; is BAD
    explicit Vec3T(T x=0, T y=0, T z=0); 
   
    void set(T x=0, T y=0, T z=0);

    bool isNan();
      
    T* ptr();
    const T* ptr() const;
    T& operator[](unsigned int i);
    const T operator[](unsigned int i) const;
    T& x();
    T& y();
    T& z();
    T x() const;
    T y() const;
    T z() const;
   
    T length() const;
    T length_sqr() const;
    void  normalize();
    Vec3T<T> asNormalized() const;
    Vec3T<T> abs() const;
   
    /*NOTE:remember right hand rule:
     * if right hand finger point in P dir and palm faces Q then thumb is dir of product in PxQ*/
    Vec3T<T> operator^(const Vec3T<T>& rh) const;    //cross
    Vec3T<T> operator-() const;                  //negate
    T operator*(const Vec3T& rh) const;   //dot
    Vec3T<T> operator*(const T& rh) const;
    Vec3T<T> operator/(const T& rh) const;
    Vec3T<T> operator+(const Vec3T<T>& rh) const;
    Vec3T<T> operator-(const Vec3T<T>& rh) const;
    void operator^=(const Vec3T<T>& rh); //cross
    void operator*=(const T& rh);
    void operator/=(const T& rh);
    void operator+=(const Vec3T<T>& rh);
    void operator-=(const Vec3T<T>& rh);   
   
    //EXTRA
    static Vec3T rand(T min=0.0f, T max=1.0f);
};

template<class T, class TT>
Vec3T<T> operator*(const TT& rh, const Vec3T<T>& lh);

template<class T>
ostream& operator<<(ostream& out, const Vec3T<T>& v);

typedef Vec3T<int> Vec3i;
typedef Vec3T<float> Vec3f;
typedef Vec3T<double> Vec3d;

typedef Vec3f Vec3;

}

#include "vec3_T.h"

#endif
