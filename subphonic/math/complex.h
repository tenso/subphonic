#ifndef SPL_COMPLEX_H
# define SPL_COMPLEX_H

# include <iostream>

//FIXME: make template

namespace spl
{

class Complex
{
    friend std::ostream& operator<<(std::ostream& l, const Complex& c);
     
  public:
    Complex();
    Complex(double r, double i);
    void set(double r=0, double i=0);
    void setRe(double v);
    void setIm(double v);
    double getRe() const;
    double getIm() const;
    Complex operator+(const Complex& r) const;
    void operator+=(const Complex& r);
    Complex operator-(const Complex& r) const;
    void operator-=(Complex& r);
    Complex operator*(const Complex& r) const;
    void operator*=(Complex& r);
    Complex operator*(double r) const;
    void operator*=(double r);
    friend Complex operator*(double r, const Complex& c);
    Complex operator/(const Complex& r) const;
    void operator /=(const Complex& r);
    Complex operator/(double r) const;
    void operator/=(double r);
    double length() const;
    double arg() const; //[-pi pi]
    Complex conj() const;
   
  private:
    double re;
    double im;
};

Complex operator*(double r, const Complex& c);


class EImgPow
{
   
    friend EImgPow operator*(EImgPow& left, double right);
    friend std::ostream& operator<<(std::ostream& l, EImgPow& r);
   
  public:
    //only complex exp because:
    //C*e^(ai+b) = (e^b * C) *e^ai
   
    //represents C*e^(i*k)
    EImgPow(double k=1.0, double C=1.0);
   
    Complex getComplex();
   
    //(1):
    double getConst(); //'C'
    double getPower(); //'k'
   
    //only diff. names than (1)
    /*double getMag(); //'C'
      double getPhase();//'k'*/
      
  private:
    double power;
    double c;
    int sgn;
};

EImgPow operator*(EImgPow& left, double right);
std::ostream& operator<<(std::ostream& l, EImgPow& r);

}

#endif
