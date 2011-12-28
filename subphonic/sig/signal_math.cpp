#include "signal_math.h"

namespace spl{

using namespace std;

GateToPulse::GateToPulse()
{
    in=NULL;
    len=NULL;
    retrig=true;
    run=0;
}

void GateToPulse::setInput(Value** in)
{
    this->in=in;
}


//def: 1
void GateToPulse::setPulseLenSmp(Value** in)
{
    this->len=in;
}


smp_t GateToPulse::nextValue()
{
    smp_t ival=0;
    if(in != NULL)ival = (*in)->nextValue();
   
    smp_t lval=1;
    if(len != NULL)lval = fabs((*len)->nextValue());
   
    if(ival>GATE_ZERO_LVL)
    {
        if(retrig)
        {
            run = (uint)lval;
            retrig=false;
        }
    }
    else
    {
        run=0;
        retrig=true;
    }
   
    smp_t ret=0;
    if(run)ret=1;
   
    if(run>0)run--;
   
    return ret;
}


ChangePulse::ChangePulse()
{
    in=NULL;
    len=NULL;
    retrig=true;
    run=0;
    cval=0;
}

void ChangePulse::setInput(Value** in)
{
    this->in=in;
}


//def: 1
void ChangePulse::setPulseLenSmp(Value** in)
{
    this->len=in;
}


smp_t ChangePulse::nextValue()
{
    smp_t ival=0;
    if(in != NULL)ival = (*in)->nextValue();
   
    smp_t lval=1;
    if(len != NULL)lval = fabs((*len)->nextValue());
   
    if(!feq(ival, cval))
    {
        run = (uint)lval;
        cval=ival;
    }
    else
    {
        run=0;
    }
   
    smp_t ret=0.0;
    if(run)ret=1.0;
   
    if(run>0)run--;
   
    return ret;
}




/******/
/*SORT*/
/******/

Sort::Sort(uint size) :
    w_in(size), v_in(size), inputs(size), v_out(size), slot_out(size)
{
    this->size=size;
   
    for(uint i=0;i<size;i++)
    {
        v_out[i] = new Const(0);
        slot_out[i] = new Const(0);
    }
   
}


void Sort::addInputWeight(uint slot, Value** val)
{
    DASSERT(slot < size);
    w_in[slot] = val;
}

void Sort::addInputValue(uint slot, Value** val)
{
    DASSERT(slot < size);
    v_in[slot] = val;
}


Value** Sort::getSortedOutput(uint n)
{
    DASSERT(n < size);
    return (Value**)&v_out[n];
}



Value** Sort::getSortedId(uint n)
{
    DASSERT(n < size);
    return (Value**)&slot_out[n];
}

//dummy
smp_t Sort::nextValue()
{
    //get input
    uint done=0;
    for(uint i=0;i<size;i++)
    {
        if(w_in[i]==NULL)continue;
	
        smp_t wval = (*w_in[i])->nextValue();
	
        inputs[done] = make_pair(wval, i);
        done++;
    }
   
    //sort by weight
    partial_sort(inputs.begin(), inputs.begin()+done, inputs.end(), cmp);
   
    //set outputs
    for(uint i=0;i<done;i++)
    {
        uint id = inputs[i].second;
	
        smp_t out_val=0;
        if(v_in[id]!=NULL)out_val = (*v_in[id])->nextValue();
        else out_val = inputs[i].first;
	
        v_out[i]->setValue(out_val);
        slot_out[i]->setValue(id);
    }
   
    return 0.0;
}

//a < b
bool Sort::Cmp::operator()(const pair<smp_t, uint>& a, const pair<smp_t, uint>& b)
{
    if(a.first < b.first)return true;
    return false;
}


/***********/
/*INTEGRATE*/
/***********/

Integrate::Integrate()
{
    in=NULL;
    re_in=NULL;
    sum=0;
    reset=true;
}

void Integrate::setInput(Value** in)
{
    this->in=in;
}

void Integrate::setResetInput(Value** in)
{
    this->re_in=in;
}

smp_t Integrate::nextValue()
{
    smp_t i=0;
    if(in != NULL)i = (*in)->nextValue();
   
    sum+=i;
   
    smp_t ri=0;
    if(re_in != NULL)ri = (*re_in)->nextValue();
   
    if(ri>GATE_ZERO_LVL)
    {
        if(reset)
        {
            sum=0;
            reset=false;
        }
    }
    else reset=true;
   
   
    return sum;
}

/*****/
/*SIN*/
/*****/

Sin::Sin()
{
    in=NULL;
}

void Sin::setInput(Value** in)
{
    this->in=in;
}

smp_t Sin::nextValue()
{
    smp_t i=0.0;
    if(in != NULL)i = (*in)->nextValue();
   
    return sin(i);
}



/******/
/*SIGN*/
/******/

Sign::Sign()
{
    in=NULL;
}

void Sign::setInput(Value** in)
{
    this->in=in;
}

smp_t Sign::nextValue()
{
    smp_t i=0;
    if(in != NULL)i = (*in)->nextValue();
   
    return i>=0 ? 1.0 : -1.0;
}


/*******/
/*TRUNC*/
/*******/

Trunc::Trunc()
{
    in=NULL;
}

void Trunc::setInput(Value** in)
{
    this->in=in;
}

smp_t Trunc::nextValue()
{
    smp_t i=0;
    if(in != NULL)i = (*in)->nextValue();
   
    return (smp_t)((int)i);
}


/************/
/*FLOOR/CEIL*/
/************/

Floor::Floor()
{
    in=NULL;
}

void Floor::setInput(Value** in)
{
    this->in=in;
}

smp_t Floor::nextValue()
{
    smp_t i=0;
    if(in != NULL)i = (*in)->nextValue();
   
    return floor(i);
}


Ceil::Ceil()
{
    in=NULL;
}

void Ceil::setInput(Value** in)
{
    this->in=in;
}

smp_t Ceil::nextValue()
{
    smp_t i=0;
    if(in != NULL)i = (*in)->nextValue();
   
    return ceil(i);
}

/*********/
/*MIN/MAX*/
/*********/

Min::Min()
{
    in0=NULL;
    in1=NULL;
}

void Min::setInput0(Value** in)
{
    in0=in;
}
void Min::setInput1(Value** in)
{
    in1=in;
}

smp_t Min::nextValue()
{
    smp_t i0=0;
    if(in0 != NULL)i0 = (*in0)->nextValue();
   
    smp_t i1=0;
    if(in1 != NULL)i1 = (*in1)->nextValue();
   
    return i0 <= i1 ? i0 : i1;
}

Max::Max()
{
    in0=NULL;
    in1=NULL;
}

void Max::setInput0(Value** in)
{
    in0=in;
}
void Max::setInput1(Value** in)
{
    in1=in;
}

smp_t Max::nextValue()
{
    smp_t i0=0;
    if(in0 != NULL)i0 = (*in0)->nextValue();
   
    smp_t i1=0;
    if(in1 != NULL)i1 = (*in1)->nextValue();
   
    return i0 >= i1 ? i0 : i1;
}

/*LESSTHAN*/

LessThan::LessThan()
{
    in0=NULL;
    in1=NULL;
}

void LessThan::setInput0(Value** in)
{
    in0=in;
}
void LessThan::setInput1(Value** in)
{
    in1=in;
}

smp_t LessThan::nextValue()
{
    smp_t i0=0;
    if(in0 != NULL)i0 = (*in0)->nextValue();
   
    smp_t i1=0;
    if(in1 != NULL)i1 = (*in1)->nextValue();
   
    return i0 < i1;
}


LessThanEqual::LessThanEqual()
{
    in0=NULL;
    in1=NULL;
}

void LessThanEqual::setInput0(Value** in)
{
    in0=in;
}
void LessThanEqual::setInput1(Value** in)
{
    in1=in;
}

smp_t LessThanEqual::nextValue()
{
    smp_t i0=0;
    if(in0 != NULL)i0 = (*in0)->nextValue();
   
    smp_t i1=0;
    if(in1 != NULL)i1 = (*in1)->nextValue();
   
    return i0 <= i1;
}

/*****/
/*MOD*/
/*****/

Mod::Mod()
{
    in=NULL;
    mod=NULL;
}

void Mod::setInput(Value** in)
{
    this->in=in;
}
void Mod::setMod(Value** mod)
{
    this->mod=mod;
}

smp_t Mod::nextValue()
{
    smp_t i=1.0;
    if(in != NULL)i = (*in)->nextValue();
   
    smp_t m=1.0;
    if(mod != NULL)m = (*mod)->nextValue();
   
    return fmod(i, m);
}


/******/
/*MULT*/
/******/

Mult::Mult()
{
    in0=NULL;
    in1=NULL;
}

void Mult::setInput0(Value** in)
{
    in0=in;
}
void Mult::setInput1(Value** in)
{
    in1=in;
}

smp_t Mult::nextValue()
{
    smp_t i0=1;
    if(in0 != NULL)i0 = (*in0)->nextValue();
   
    smp_t i1=1;
    if(in1 != NULL)i1 = (*in1)->nextValue();
   
    return i0*i1;
}

/************/
/*PLUS/MINUS*/
/************/

Plus::Plus()
{
    in0=NULL;
    in1=NULL;
}

void Plus::setInput0(Value** in)
{
    in0=in;
}
void Plus::setInput1(Value** in)
{
    in1=in;
}

smp_t Plus::nextValue()
{
    smp_t i0=0;
    if(in0 != NULL)i0 = (*in0)->nextValue();
   
    smp_t i1=0;
    if(in1 != NULL)i1 = (*in1)->nextValue();
   
    return i0+i1;
}

Minus::Minus()
{
    in0=NULL;
    in1=NULL;
}

void Minus::setInput0(Value** in)
{
    in0=in;
}
void Minus::setInput1(Value** in)
{
    in1=in;
}

smp_t Minus::nextValue()
{
    smp_t i0=0;
    if(in0 != NULL)i0 = (*in0)->nextValue();
   
    smp_t i1=0;
    if(in1 != NULL)i1 = (*in1)->nextValue();
   
    return i0-i1;
}

/********/
/*DIVIDE*/
/********/

Divide::Divide()
{
    nan=false;
   
    num=NULL;
    denom=NULL;
}


void Divide::setNumerator(Value** in)
{
    num=in;
}


void Divide::setDenominator(Value** in)
{
    denom=in;
}


bool Divide::isNan()
{
    return nan;
}


smp_t Divide::nextValue()
{
    smp_t n=1;
    if(num!=NULL)n = (*num)->nextValue();
   
    smp_t d=1;
    if(denom!=NULL)d = (*denom)->nextValue();
   
    if(feq<smp_t>(d,0))
    {
        nan=true;
        return 0;
    }
   
    return n/d;
}




/**********/
/*Exponent*/
/**********/

Exponent::Exponent()
{
    v_base=NULL;
    v_exponent=NULL;
    base=M_E;
}

void Exponent::setBase(smp_t b)
{
    base=b;
}

void Exponent::setBaseIn(Value** b)
{
    v_base=b;
}

void Exponent::setExponent(Value** in)
{
    v_exponent=in;
}


smp_t Exponent::nextValue()
{
    smp_t ex = 1.0;
    if(v_exponent != NULL)ex = (*v_exponent)->nextValue();
   
    smp_t ba = base;
    if(v_base != NULL)ba = (*v_base)->nextValue();
   
   
    smp_t val = pow(ba, ex);
   
    static bool warned=false;
    if(!isfinite(val))
    {
        if(!warned)
        {
            warned=true;
            DERROR("output is not finite! (only one warning)");
        }
        val=0;
    }
   
    return val;
}


/***********/
/*LOGARITHM*/
/***********/

Logarithm::Logarithm()
{
    v_base=NULL;
    v_arg=NULL;
    neg_inf=false;
   
    setBase(M_E);
}



void Logarithm::setBase(smp_t b)
{
    base=b;
}


void Logarithm::setBaseIn(Value** b)
{
    v_base=b;
}


void Logarithm::setArgument(Value** in)
{
    v_arg=in;
}


smp_t Logarithm::nextValue()
{
    smp_t a = 1.0;
    if(v_arg != NULL)a = fabs((*v_arg)->nextValue());
   
    smp_t b = base;
    if(v_base != NULL)b = fabs((*v_base)->nextValue());
   
    if(a==0) //feq ?
    {
        neg_inf=0;
        return 0;
    }
    else neg_inf=false;
   
   
    smp_t val = log(a)/log(b);
   
    static bool warned=false;
    if(!isfinite(val))
    {
        if(!warned)
        {
            warned=true;
            DERROR("output is not finite! (only one warning)");
        }
        val=0;
    }
   
    return val;
}

bool Logarithm::negInf()
{
    return neg_inf;
}



/***********/
/*THRESHOLD*/
/***********/

Threshold::Threshold()
{
    v_in=NULL;
    v_th=NULL;
    v_on=NULL;
    v_off=NULL;
   
    ison=false;
}


void Threshold::setInput(Value** in)
{
    v_in=in;
}


void Threshold::setOnVal(Value** in)
{
    v_on=in;
}

void Threshold::setOffVal(Value** in)
{
    v_off=in;
}


void Threshold::setThreshold(Value** in)
{
    v_th=in;
}


smp_t Threshold::nextValue()
{
    smp_t in=0.0;
    if(v_in!=NULL)in = (*v_in)->nextValue();
   
    smp_t on=1.0;
    if(v_on!=NULL)on = (*v_on)->nextValue();
   
    smp_t off=0.0;
    if(v_off!=NULL)off = (*v_off)->nextValue();
   
    smp_t th=0.5;
    if(v_th!=NULL)th = (*v_th)->nextValue();
   
    if(in>=th)
    {
        ison=true;
        return on;
    }
    ison=false;
   
    return off;
}

const bool* Threshold::isOnPtr()
{
    return &ison;
}


/*LOGMAP*/

LogisticMap::LogisticMap()
{
    in_seed=NULL;
    in_r=NULL;
    c_val=0;
   
    cont_seed=false;
    c_seed=0;
}


smp_t LogisticMap::nextValue()
{
    smp_t seed=1.0;
    if(in_seed != NULL)
    {
        seed=(*in_seed)->nextValue();
	
        if(cont_seed)
        {
            if(!feq(c_seed, seed))
            {
                c_seed = seed;
                c_val = seed;
            }
        }
    }
   
    smp_t r=0;
    if(in_r !=NULL)r=(*in_r)->nextValue();
   
    if(r<0)r=0;
    if(r>4)r=4;
   
    smp_t n_val = r*c_val*(1.0-c_val);
    c_val=n_val;
   
    return c_val;
}

void LogisticMap::setR(Value** in)
{
    in_r=in;
}


void LogisticMap::setSeed(Value** in)
{
    in_seed=in;
}

void LogisticMap::setContSeed(bool val)
{
    cont_seed=val;
}

bool LogisticMap::getContSeed()
{
    return cont_seed;
}


void LogisticMap::reset()
{
    smp_t s = 0;
   
    if(in_seed != NULL)s=(*in_seed)->nextValue();
   
    c_val = s;
}

}