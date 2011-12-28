#ifndef SIGNAL_MATH_H
#define SIGNAL_MATH_H

#include <vector>
#include <algorithm>

#include "signal.h"

/*input0 is left operand and input1 right for binop*/

namespace spl{

class GateToPulse : public Value
{
  public:
    GateToPulse();
   
    void setInput(Value** in);
   
    //def: 1
    void setPulseLenSmp(Value** in);
   
    smp_t nextValue();
   
  private:
    Value** in;
    Value** len;
    bool retrig;
    uint run;
};

//outputs 1 if input has changed since last nextValue()
class ChangePulse : public Value
{
  public:
    ChangePulse();
   
    void setInput(Value** in);
   
    //def: 1
    void setPulseLenSmp(Value** in);
   
    smp_t nextValue();
   
  private:
    Value** in;
    Value** len;
    bool retrig;
    uint run;
    smp_t cval;
};


//NOTICE: this must be drained
class Sort : public Value
{
  public:
    Sort(uint size);
   
    //slots with weight inputs as NULL are unused
    //values are sorted by weight, but the ouput is actually the corresponding value
    //if addInputValue(slot, NULL) the weight is also the output value
    void addInputWeight(uint slot, Value** val);
    void addInputValue(uint slot, Value** val);
   
    //output is [0 num_added] whichever input slots are used
    //returns values sorted
    Value** getSortedOutput(uint n);
   
    //returns what input the ouput value at 'n'  corresponds to: [0 num added-1]
    Value** getSortedId(uint n);
   
    smp_t nextValue(); //drain
   
  public:
   
    class Cmp
    {
      public:
        bool operator()(const pair<smp_t, uint>& a, const pair<smp_t, uint>& b); //a < b
    }cmp;
   
    uint size;
   
    //all same: size
    std::vector<Value**> w_in;
    std::vector<Value**> v_in;
   
    std::vector<std::pair<smp_t, uint> > inputs;
   
    std::vector<Const*> v_out;
    std::vector<Const*> slot_out;
};

class Integrate : public Value
{
  public:
    //sums all inputs from last reset until now
    Integrate();
   
    //def=0.0
    void setInput(Value** in);
    void setResetInput(Value** in); //restarts
 
    smp_t nextValue();
   
  private:
    smp_t sum;
    Value** in;
    Value** re_in;
    bool reset;
};

class Sin : public Value
{
  public:
    Sin();
   
    //def=0.0
    void setInput(Value** in);
 
    smp_t nextValue();
   
  private:
    Value** in;
};

class Sign : public Value
{
  public:
    Sign();
   
    //def=0
    void setInput(Value** in);
 
    smp_t nextValue();
   
  private:
    Value** in;
};

//truncates to integer
class Trunc : public Value
{
  public:
    Trunc();
   
    //def=0
    void setInput(Value** in);
 
    smp_t nextValue();
   
  private:
    Value** in;
};

/*
  class GreaterThan : public Value
  {
  public:
  GreaterThan();
   
  //in0 > in1 => output 1, else 0
  //both def=0
  void setInput0(Value** in);
  void setInput1(Value** in);
 
  smp_t nextValue();
   
  private:
  Value** in0;
  Value** in1;
  };*/

//i.e greatest integer not greater than input
class Floor : public Value
{
  public:
    Floor();
   
    //def=0
    void setInput(Value** in);
   
    //addInput(uint id, Value**);
   
    smp_t nextValue();
   
  private:
    Value** in;
};

//i.e smallest integer not less than input
class Ceil : public Value
{
  public:
    Ceil();
   
    //def=0
    void setInput(Value** in);
   
   
    smp_t nextValue();
   
  private:
    Value** in;
};


class Min : public Value
{
  public:
    Min();
   
    //outputs min input
    //both def=0
    void setInput0(Value** in);
    void setInput1(Value** in);
 
    smp_t nextValue();
   
  private:
    Value** in0;
    Value** in1;
};

class Max : public Value
{
  public:
    Max();
   
    //outputs min input
    //both def=0
    void setInput0(Value** in);
    void setInput1(Value** in);
 
    smp_t nextValue();
   
  private:
    Value** in0;
    Value** in1;
};

class LessThan : public Value
{
  public:
    LessThan();
    //input0 < input1
   
    //both def=0
    void setInput0(Value** in);
    void setInput1(Value** in);
 
    //ouputs 1/1
    smp_t nextValue();
   
  private:
    Value** in0;
    Value** in1;
};

class LessThanEqual : public Value
{
  public:
    LessThanEqual();
    //input0 <= input1
   
    //both def=0
    void setInput0(Value** in);
    void setInput1(Value** in);
   
    //ouputs 1/0
    smp_t nextValue();
   
  private:
    Value** in0;
    Value** in1;
};

//uses fmod()
class Mod : public Value
{
  public:
    Mod();
   
    //both def=1.0
    void setInput(Value** in);
    void setMod(Value** in);
 
    smp_t nextValue();
   
  private:
    Value** in;
    Value** mod;
};


//FIXME: add setSafe() to Div and Log
/*math ops are "proteced": does not return inf, nan etc but 0 instead*/

class Mult : public Value
{
  public:
    Mult();
   
    //both def=1
    void setInput0(Value** in);
    void setInput1(Value** in);
 
    smp_t nextValue();
   
  private:
    Value** in0;
    Value** in1;
};

class Plus : public Value
{
  public:
    Plus();
   
    //both def=0
    void setInput0(Value** in);
    void setInput1(Value** in);
 
    smp_t nextValue();
   
  private:
    Value** in0;
    Value** in1;
};

class Minus : public Value
{
  public:
    Minus();
   
    //both def=0
    void setInput0(Value** in);
    void setInput1(Value** in);
 
    smp_t nextValue();
   
  private:
    Value** in0;
    Value** in1;
};

class Divide : public Value
{
  public:
    Divide();
   
    //def: 1
    void setNumerator(Value** in);
   
    //if this becomes 0 out is=0, and isNan()= true
    //def: 1
    void setDenominator(Value** in);
   
    bool isNan();
   
    smp_t nextValue();
 
  private:
    bool nan;
   
    Value** num;
    Value** denom;
};

//WARNING: dont use these if precision is vital

class Exponent : public Value
{
  public:
	
    Exponent();
   
    //FIXME: unchecked:
    //WARNING: watch out for a negative base and a non integer negative exponent
    //==>img number
   
    //for ease of use this is the default base,
    //used everytime setBase(NULL)
    //def: M_E
    void setBase(smp_t b);
   
    //def: NULL
    //e.g 2, M_E etc
    void setBaseIn(Value** b);
   
    //def 1
    void setExponent(Value** in);
   
    smp_t nextValue();

  private:
    smp_t base;
    Value** v_base;
    Value** v_exponent;
};


class Logarithm : public Value
{
  public:
	
    Logarithm();

    //BASE: no negative values
   
    //for ease of use this is the default base,
    //used everytime setBase(NULL)
    //def: M_E
    void setBase(smp_t b);
   
    //def: NULL
    //e.g 2, M_E etc
    void setBaseIn(Value** b);
   
    //ARGUMENT >= 0
    //def: 1
    //if arg=0 returns 0 and sets negInf()
    void setArgument(Value** in);
   
    smp_t nextValue();

    bool negInf();
   
  private:
    bool neg_inf;
   
    smp_t base;
    Value** v_base;
    Value** v_arg;
};


class Threshold : public Value
{
  public:
    Threshold();
   
    void setInput(Value** in);
   
    //in==NULL => 1.0
    void setOnVal(Value** in);
   
    //in==NULL => 0.0
    void setOffVal(Value** in);
   
    //in==NULL => 0.5
    //if(in>=th)return on;else return off
    void setThreshold(Value** in);
   
    smp_t nextValue();
   
    const bool* isOnPtr();
   
  private:
    Value** v_in;
    Value** v_on;
    Value** v_off;
    Value** v_th;
   
    bool ison;
};


//returns x(n+1) = r*x(n)*(1-x(n))

class LogisticMap : public Value
{
  public:
    //because seed starts everything off a reset is needed after seed changes, 
    //unless setContSeed(true) : map is restarted everytime seed changes though
   
    LogisticMap();
   
    smp_t nextValue();
   
    //should be 0-4 otherwise goes beyond [0.0 1.0] -> inf
    //therefore clipped to 0-4
    //default: 0
    void setR(Value** in);
   
    //0-1
    //x(0)
    //default: 1.0
    void setSeed(Value** in);
   
    void setContSeed(bool val);
    bool getContSeed();
   
    //restarts map
    void reset();
   
  private:
    Value** in_r;
    Value** in_seed;
    smp_t   c_val;
   
    bool cont_seed;
    smp_t c_seed;
};

}

#endif
