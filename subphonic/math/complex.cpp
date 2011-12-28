#include "complex.h"

namespace spl
{

using namespace std;

ostream& operator<<(ostream& l, const Complex& c)
{
    if(c.im>=0)l << c.re << " + " << c.im << "j";
    else l << c.re << " - " << fabs(c.im) << "j";
    return l;
}

Complex operator*(double r, const Complex& c)
{
    return Complex(c.re*r, c.im*r);
}

Complex::Complex()
{
    re=0;
    im=0;
}

Complex::Complex(double r, double i)
{
    re=r;
    im=i;
}

void Complex::set(double r, double i)
{
    re=r;
    im=i;
}

void Complex::setRe(double v)
{
    re=v;
}
void Complex::setIm(double v)
{
    im=v;
}
double Complex::getRe() const
{
    return re;
}
double Complex::getIm() const
{
    return im;
}

Complex Complex::operator+(const Complex& r) const
{
    return Complex(re+r.re, im+r.im);
}
void Complex::operator+=(const Complex& r)
{
    re+=r.re;
    im+=r.im;
}
Complex Complex::operator-(const Complex& r) const
{
    return Complex(re-r.re, im-r.im);
}
void Complex::operator-=(Complex& r)
{
    re-=r.re;
    im-=r.im;
}
Complex Complex::operator*(const Complex& r) const
{
    return Complex(re*r.re - im*r.im, re*r.im + im*r.re);
}
void Complex::operator*=(Complex& r)
{
    double tmp=re;
    re = re*r.re - im*r.im;
    im = tmp*r.im + im*r.re;
}
Complex Complex::operator*(double r) const
{
    return Complex(re*r, im*r);
}
void Complex::operator*=(double r)
{
    re*=r;
    im*=r;
}


Complex Complex::operator/(const Complex& r) const
{
    double nr=re*r.re+im*r.im;
    double ni=im*r.re-re*r.im;
   
    double nd=1.0/(r.re*r.re+r.im*r.im);
   
    return Complex(nr * nd, ni * nd);
}
void Complex::operator/=(const Complex& r)
{
    double nr=re*r.re+im*r.im;
    double ni=im*r.re-re*r.im;
    double nd=1.0/(r.re*r.re+r.im*r.im);
   
    re=nr*nd;
    im=ni*nd;
}
Complex Complex::operator/(double r) const
{
    return Complex(re/r, im/r);
}
void Complex::operator/=(double r)
{
    double t=1.0/r;
    re*=t;
    im*=t;
}

double Complex::length() const
{
    return sqrt(re*re+im*im);
}

double Complex::arg() const
{
    return atan2(im, re);
}


Complex Complex::conj() const
{
    return Complex(re,-im);
}

/*EIMGPOW*/

EImgPow::EImgPow(double k, double C)
{
    c = C;
    sgn = (k>0) ? 1 : -1;
    power = fabs(k);
}

Complex EImgPow::getComplex()
{
    return Complex(c*cos(power),c*sgn*sin(power));
}

double EImgPow::getConst()
{
    return c;
}

double EImgPow::getPower()
{
    return power;
}

/*friends*/
EImgPow operator*(EImgPow& left, double right)
{
    return EImgPow(left.power, left.c*right);
}
   
ostream& operator<<(ostream& l, EImgPow& r)
{
    return ( l << r.c << "e^(" << r.power << "j)");
}

}//end namespace
