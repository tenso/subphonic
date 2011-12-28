#include "signal_note.h"

namespace spl{

KeyExtract::KeyExtract()
{
    in=NULL;
}


void KeyExtract::setInput(Value** in)
{
    this->in=in;
}

smp_t KeyExtract::nextValue()
{
    smp_t v_in = 0;
    if(in!=NULL)v_in = (*in)->nextValue();
   
    int in_key = hzNote(v_in);
   
    return (smp_t)in_key;
}



/**********/
/*NOTETRAN*/
/**********/

NoteTran::NoteTran()
{
    in = NULL;
    key = NULL;
    oct = NULL;
    cent=NULL;
    setQuantize(true);
}


void NoteTran::setInput(Value** in)
{
    this->in=in;
}

void NoteTran::setCentOff(Value** in)
{
    cent=in;
}


void NoteTran::setKeyOff(Value** in)
{
    this->key=in;
}

void NoteTran::setOctOff(Value** in)
{
    this->oct=in;
}

void NoteTran::setQuantize(bool on)
{
    quantize=on;
}

bool NoteTran::getQuantize()
{
    return quantize;
}


smp_t NoteTran::nextValue()
{
    smp_t v_in = 0;
    if(in!=NULL)v_in = (*in)->nextValue();
   
    if(feq(v_in, (smp_t)0.0)) //0 hz in always give 0 out
    {
        return 0;
    }
   
    smp_t v_cent = 0;
    if(cent!=NULL)v_cent = (*cent)->nextValue();
   
    smp_t v_key = 0;
    if(key!=NULL)v_key = (*key)->nextValue();
   
    smp_t v_oct = 0;
    if(oct!=NULL)v_oct = (*oct)->nextValue();
   
    if(quantize)
    {
        v_cent = floor(v_cent);
        v_key = floor(v_key);
        v_oct = floor(v_oct);
    }
   
   
    /*
      int in_key = hzNote(v_in);
      int out_key = (int)(12*v_oct+v_key+in_key);
   	
      if(v_cent!=0) //speed?
      return noteHz(out_key)*powf(2,v_cent/1200.0);
      else
      return noteHz(out_key);
    */
   
    smp_t off = v_cent/100.0 + v_key + v_oct*12.0;
   
    return v_in*powf(2.0, off/12.0);
   
}

}