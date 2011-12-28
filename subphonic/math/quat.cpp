#include "quat.h"

namespace spl{

Quat operator*(const Vec4& rh, const Quat& q)
{
    Quat ret;
    ret[0]=rh[3]*q.data[0]+rh[0]*q.data[3]+rh[1]*q.data[2]-rh[2]*q.data[1];
    ret[1]=rh[3]*q.data[1]-rh[0]*q.data[2]+rh[1]*q.data[3]+rh[2]*q.data[0];
    ret[2]=rh[3]*q.data[2]+rh[0]*q.data[1]-rh[1]*q.data[0]+rh[2]*q.data[3];
    ret[3]=rh[3]*q.data[3]-rh[0]*q.data[0]-rh[1]*q.data[1]-rh[2]*q.data[2];
    return ret;
}

Quat operator*(const Vec3& rh, const Quat& q)
{
    Quat ret;
    ret[0]=0.0f*q.data[0]+rh[0]*q.data[3]+rh[1]*q.data[2]-rh[2]*q.data[1];
    ret[1]=0.0f*q.data[1]-rh[0]*q.data[2]+rh[1]*q.data[3]+rh[2]*q.data[0];
    ret[2]=0.0f*q.data[2]+rh[0]*q.data[1]-rh[1]*q.data[0]+rh[2]*q.data[3];
    ret[3]=0.0f*q.data[3]-rh[0]*q.data[0]-rh[1]*q.data[1]-rh[2]*q.data[2];
    return ret;
}

Quat operator*(const float& lh, const Quat& q) 
{
    Quat ret;
    ret[0]=q.data[0]*lh;
    ret[1]=q.data[1]*lh;
    ret[2]=q.data[2]*lh;
    ret[3]=q.data[3]*lh;
   
    return ret;
}

ostream& operator<<(ostream& out, const Quat& v)
{
    return out << v.data[0] << " " << v.data[1] << " " << v.data[2] << " " << v.data[3];
}

Quat::Quat()
{
    set(0,0,0,0);
}


Quat::Quat(float x, float y, float z, float w)
{
    data[0]=x;
    data[1]=y;
    data[2]=z;
    data[3]=w;
}

Quat::Quat(const Vec3& vec, float w)
{
    data[0]=vec[0];
    data[1]=vec[1];
    data[2]=vec[2];
    data[3]=w;
}

Quat::Quat(const Vec4& vec)
{
    data[0]=vec[0];
    data[1]=vec[1];
    data[2]=vec[2];
    data[3]=vec[3];
}

void Quat::set(float x, float y, float z, float w)
{
    data[0]=x;
    data[1]=y;
    data[2]=z;
    data[3]=w;
}
void Quat::set(const Vec3& vec, float w)
{
    data[0]=vec[0];
    data[1]=vec[1];
    data[2]=vec[2];
    data[3]=w;
}
void Quat::set(const Vec4& vec)
{
    data[0]=vec[0];
    data[1]=vec[1];
    data[2]=vec[2];
    data[3]=vec[3];
}

float& Quat::operator[](int index)
{
    return data[index];
}
float Quat::operator[](int index) const
{
    return data[index];
}

float& Quat::x() { return data[0]; }
float& Quat::y() { return data[1]; }
float& Quat::z() { return data[2]; }
float& Quat::w() { return data[3]; }
float Quat::x() const { return data[0]; } 
float Quat::y() const { return data[1]; }
float Quat::z() const { return data[2]; }
float Quat::w() const { return data[3]; }


Quat Quat::operator*(const float& rh) const
{
    Quat ret;
    ret[0]=data[0]*rh;
    ret[1]=data[1]*rh;
    ret[2]=data[2]*rh;
    ret[3]=data[3]*rh;
   
    return ret;
}

Quat Quat::operator/(const float& rh) const
{
    Quat ret;
    float inv=1.0f/rh;
   
    ret[0]=data[0]*inv;
    ret[1]=data[1]*inv;
    ret[2]=data[2]*inv;
    ret[3]=data[3]*inv;
   
    return ret;
}

Quat Quat::operator+(const Quat& rh) const
{
    Quat ret;
    ret[0]=data[0]+rh[0];
    ret[1]=data[1]+rh[1];
    ret[2]=data[2]+rh[2];
    ret[3]=data[3]+rh[3];
   
    return ret;
}

void Quat::operator+=(const Quat& rh)
{
   
    data[0]+=rh[0];
    data[1]+=rh[1];
    data[2]+=rh[2];
    data[3]+=rh[3];
}

void Quat::operator-=(const Quat& rh)
{
   
    data[0]-=rh[0];
    data[1]-=rh[1];
    data[2]-=rh[2];
    data[3]-=rh[3];
}

Quat Quat::operator-(const Quat& rh) const
{
    Quat ret;
    ret[0]=data[0]-rh[0];
    ret[1]=data[1]-rh[1];
    ret[2]=data[2]-rh[2];
    ret[3]=data[3]-rh[3];
   
    return ret;
}


Quat Quat::operator*(const Vec4& rh) const 
{
    Quat ret;
    ret[0]=data[3]*rh[0]+data[0]*rh[3]+data[1]*rh[2]-data[2]*rh[1];
    ret[1]=data[3]*rh[1]-data[0]*rh[2]+data[1]*rh[3]+data[2]*rh[0];
    ret[2]=data[3]*rh[2]+data[0]*rh[1]-data[1]*rh[0]+data[2]*rh[3];
    ret[3]=data[3]*rh[3]-data[0]*rh[0]-data[1]*rh[1]-data[2]*rh[2];
    return ret;
}


Quat Quat::operator*(const Vec3& rh) const
{
    Quat ret;
    ret[0]=data[3]*rh[0]+data[0]*0.0f+data[1]*rh[2]-data[2]*rh[1];
    ret[1]=data[3]*rh[1]-data[0]*rh[2]+data[1]*0.0f+data[2]*rh[0];
    ret[2]=data[3]*rh[2]+data[0]*rh[1]-data[1]*rh[0]+data[2]*0.0f;
    ret[3]=data[3]*0.0f-data[0]*rh[0]-data[1]*rh[1]-data[2]*rh[2];
    return ret;
}

//if rotation and q2*q1 => rotate by q1 then q2
Quat Quat::operator*(const Quat& rh) const
{
    Quat ret;
    ret[0]=data[3]*rh.data[0]+data[0]*rh.data[3]+data[1]*rh.data[2]-data[2]*rh.data[1];
    ret[1]=data[3]*rh.data[1]-data[0]*rh.data[2]+data[1]*rh.data[3]+data[2]*rh.data[0];
    ret[2]=data[3]*rh.data[2]+data[0]*rh.data[1]-data[1]*rh.data[0]+data[2]*rh.data[3];
    ret[3]=data[3]*rh.data[3]-data[0]*rh.data[0]-data[1]*rh.data[1]-data[2]*rh.data[2];
    return ret;
}

void Quat::operator*=(const Quat& rh)
{
    Quat ret;
    ret[0]=data[3]*rh.data[0]+data[0]*rh.data[3]+data[1]*rh.data[2]-data[2]*rh.data[1];
    ret[1]=data[3]*rh.data[1]-data[0]*rh.data[2]+data[1]*rh.data[3]+data[2]*rh.data[0];
    ret[2]=data[3]*rh.data[2]+data[0]*rh.data[1]-data[1]*rh.data[0]+data[2]*rh.data[3];
    ret[3]=data[3]*rh.data[3]-data[0]*rh.data[0]-data[1]*rh.data[1]-data[2]*rh.data[2];
   
    data[0]=ret[0];
    data[1]=ret[1];
    data[2]=ret[2];
    data[3]=ret[3];
   
   
}

void Quat::setRotate(float x, float y, float z, float rad)
{
    float len = 1.0/sqrt(x*x+y*y+z*z);
   
    float s_ang = sin(rad*0.5f);
    float c_ang = cos(rad*0.5f);
   
    data[0]=x*s_ang*len;
    data[1]=y*s_ang*len;
    data[2]=z*s_ang*len;
    data[3]=c_ang;
}

void Quat::setRotate(const Vec3& axis, float rad)
{
    Vec3 ax=axis;
    ax.normalize(); //FIXME: do this?
   
    float s_ang = sin(rad*0.5f);
    float c_ang = cos(rad*0.5f);
   
    data[0]=ax[0]*s_ang;
    data[1]=ax[1]*s_ang;
    data[2]=ax[2]*s_ang;
    data[3]=c_ang;
}

Quat Quat::rotate(const Vec3& axis, float rad)
{
    Quat ret;
    ret.setRotate(axis, rad);
    return ret;
}

Quat Quat::rotate(float x, float y, float z, float rad)
{
    Quat ret;
    ret.setRotate(x,y,z, rad);
    return ret;
}

Matrix Quat::asMatrix()
{
    Matrix ret;
    ret.setIdentity();
   
   
    ret(0,0) = 1.0f - 2.0f*data[1]*data[1] - 2.0f*data[2]*data[2];
    ret(0,1) = 2.0f*data[0]*data[1] - 2.0f*data[3]*data[2];
    ret(0,2) = 2.0f*data[0]*data[2] + 2.0f*data[3]*data[1];
   
    ret(1,0) = 2.0f*data[0]*data[1] + 2.0f*data[3]*data[2];
    ret(1,1) = 1.0f - 2.0f*data[0]*data[0] - 2.0f*data[2]*data[2];
    ret(1,2) = 2.0f*data[1]*data[2]-2.0f*data[3]*data[0];
   
    ret(2,0) = 2.0f*data[0]*data[2] - 2.0f*data[3]*data[1];
    ret(2,1) = 2.0f*data[1]*data[2]+2.0f*data[3]*data[0];
    ret(2,2) = 1.0f - 2.0f*data[0]*data[0] - 2.0f*data[1]*data[1];
   
    return ret;
}

Vec3 Quat::rotateVec(const Vec3& vec)
{
    Quat v(vec, 0);
    Quat ans=(*this)*v*(*this).inverse();
   
    return Vec3(ans[0],ans[1],ans[2]);
}

void Quat::normalize()
{
    float len = 1.0/sqrt(data[0]*data[0]+data[1]*data[1]+data[2]*data[2]+data[3]*data[3]);
   
    data[0]*=len;
    data[1]*=len;
    data[2]*=len;
    data[3]*=len;
   
}


float Quat::length() const
{
    return sqrt(data[0]*data[0]+data[1]*data[1]+data[2]*data[2]+data[3]*data[3]);
}

float Quat::length_sqr() const
{
    return data[0]*data[0]+data[1]*data[1]+data[2]*data[2]+data[3]*data[3];
}
Quat Quat::inverse() const
{
    return conjugate()/length_sqr();
}

Quat Quat::conjugate() const
{
    Quat ret;
   
    ret[0] =-data[0];
    ret[1] =-data[1];
    ret[2] =-data[2];
    ret[3] = data[3];
   
    return ret;
}

Quat Quat::slerp(float t, const Quat& to)
{
    float theta = acos(data[0]*to[0]+data[1]*to[1]+data[2]*to[2]+data[3]*to[3]);
    float s_ang = 1.0/sin(theta);
   
    return sin(theta*(1-t))*s_ang*(*this)+sin(theta*t)*s_ang*to;
}

}
