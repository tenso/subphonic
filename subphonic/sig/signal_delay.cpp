#include "signal_delay.h"

namespace spl{ 

/*MK2*/

/*******/
/*DELAY*/
/*******/

Delay_MK2::Delay_MK2(int max_delay_smp)
{
    t_mult=1.0;
   
    if(max_delay_smp>=0)
    {
        this->max_delay_smp = (unsigned)max_delay_smp;
        inf_delay_size=false;
        buff = new RingBuffer<smp_t>(max_delay_smp);
    }
    else
    {
        inf_delay_size=true;
        this->max_delay_smp = INITIAL_DELAY_SZ;
        buff = new RingBuffer<smp_t>(max_delay_smp);
    }
   
   
    delay=NULL;
    input=NULL;
   
    curr_d=0;
    curr_d_frac=0;
   
    peek_i=0;
    cross=0;
    cross_step=0.01; //75.0f/(44100.0f); //FIXME: what is good?
    do_cross=NONE;
    warned=false;
    first_done=false;
   
    cross_d=0;
    cross_d_frac=0;
   
    to_add=0;
   
    delay_len=0;
   
    mode = FRAC_INTERPOLATE_LINEAR;
   
    curr_read = 1;
   
    d_chg_sleep_val=0; //not so good param: just diff noices
    d_chg_sleep=d_chg_sleep_val;
   
    peek=NULL;
}


void Delay_MK2::setDelay(Value** d)
{
    delay=d;
}

void Delay_MK2::setInput(Value** in)
{
    input=in;
}


/*NEED to do crossfade between old and new delay length if they are several samples long
 * or unwanted artifacts will be present*/

smp_t Delay_MK2::getValue()
{
    static bool warned=false;
   
    smp_t inval = 0;
    if(input != NULL)inval = (*input)->nextValue();
   
    if(!isfinite(inval))
    {
        if(!warned)
        {
            warned=true;
            DERROR("input is not finite! (only one warning)");
        }
        inval=0;
    }
   
    return inval;
}

void Delay_MK2::setFracInterpolateMode(F_INTER_MODE m)
{
    mode=m;
}

void Delay_MK2::setPeekInput(Value** in)
{
    peek=in;
}


smp_t Delay_MK2::nextValue()
{
    smp_t delval=0;
    if(delay!=NULL)delval = (*delay)->nextValue();
   
    //peek is clipped between [0 curr_d] so it is ok to add all the time
    uint peekval=0;

    smp_t p;
    if(peek!=NULL)
    {
        p = (*peek)->nextValue();
        if(p<0)p=0;
	
        if(p>curr_d)p=curr_d;
        peekval=(uint)p;
	
    }
   
   
    smp_t w_real_d = fabs(t_mult * delval);
   
    uint wanted_d = (uint)( w_real_d);
    smp_t wanted_d_frac = w_real_d - wanted_d;
    //wanted_d_frac=0;
   
    //cout << wanted_d << endl;
   
    if(wanted_d >= max_delay_smp)
    {
        if(inf_delay_size)
        {
            //*2 to prevent so many resize()
            max_delay_smp = wanted_d*2;
            buff->resize(max_delay_smp); //keeps all data(only grow)
        }
        else
        {
            DERROR("delay to big truncating");
            wanted_d=max_delay_smp-1;
        }
    }
   
    //INITIAL FILL
    if(buff->inBuffer() < curr_d+1 )
    {
        //DERROR("first");
        buff->add(getValue());
        delay_len++;
	
        return 0;  //nothing more to be done
    }
   
    if(do_cross==NONE)
    {
        //CHANGE DELAY LEN?
        if(wanted_d < curr_d)
        {
            //curr_d=wanted_d; //just changing delay "hard" like this produces a lot of noice;try it
	     
            if(d_chg_sleep==0)
            {
                d_chg_sleep=d_chg_sleep_val;
		  
                do_cross=FORWARD;
		  
                curr_read = delay_len - curr_d;
		  
                cross=0;
		  
                cross_d=wanted_d;
                cross_d_frac=wanted_d_frac;
		  
                peek_i=curr_d-wanted_d;
		  
            }
            else d_chg_sleep--;
        }
        else if(wanted_d > curr_d) 
        {
            if(d_chg_sleep==0)
            {
                d_chg_sleep=d_chg_sleep_val;
		  
                curr_read = delay_len - curr_d;
		  
                if(wanted_d >= delay_len)
                {
                    do_cross=GROW;
                    peek_i=0;
                    cross_d=wanted_d;
                    cross_d_frac=wanted_d_frac;
                    to_add = wanted_d-delay_len+1;
                }
                else
                {
                    do_cross=BACKWARD;
                    cross=0;
                    cross_d=wanted_d;
                    cross_d_frac=wanted_d_frac;
                    peek_i=wanted_d-curr_d;
                }
            }
            else d_chg_sleep--;
	     
        }
	
        if(do_cross==NONE) 	//NORMAL unless some above change do_cross
        {
            //cout << "norm" << endl;
	     
            //wanted integer delay same: frac may vary
            //however dont let frac part just vary as it wants here: produces alot of noice better keep it set untill delay 
            //changes proper(fwd/bwd) (still want frac delyay though(but fixed))
	     
            //curr_d_frac = wanted_d_frac;
	     
            buff->add(getValue());
	     
            curr_read = delay_len - curr_d;
            DASSERT(curr_read>0);
            if(curr_read<1)
            {
                cout << "curr_read:" << curr_read << endl;
                curr_read=1;
            }
	     
            /*static int debug=0;
              debug++;
              if(debug%1000==0)
              {
              cout << "rb size:" << buff->inBuffer() << "  read: " << curr_read << "  curr_d:" << curr_d << "  frac:" << curr_d_frac <<endl;
              }*/
	     
	     
            smp_t val=0;
	     
            if(mode==FRAC_INTERPOLATE_NONE)
            {
                val = buff->peek(curr_read+peekval);
            }
            else if(mode==FRAC_INTERPOLATE_LINEAR)
            {
                val = buff->peek(curr_read+peekval)*(1.0-curr_d_frac) + buff->peek(curr_read-1+peekval)*curr_d_frac;
            }
            else
            {
                DERROR("no such mode");
            }
	     
            buff->get();
	     
            return val;
        } //END NORMAL
	
    }//end do_cross==NONE
   
   
    //SHRINK DELAY 
    /*if(wanted_d < curr_d && do_cross==NONE)
      {
      curr_read = delay_len - curr_d;
      DASSERT(curr_read>0);
      * 
      peek_i=curr_d-wanted_d;
      * 
      do_cross=FORWARD;
      cross_d=wanted_d;
      cross_d_frac = wanted_d_frac;
      cross=0;
      }
      * 
      //GROW delay
      if(wanted_d>curr_d && do_cross==NONE)
      {
      curr_read = delay_len - curr_d;
      DASSERT(curr_read>0);
      * 
      if(wanted_d < delay_len)
      {
      do_cross=BACKWARD;
      peek_i = wanted_d-curr_d;
      }
      else
      {
      peek_i=0;
      do_cross=GROW;
      cross_d=wanted_d;
      cross_d_frac = wanted_d_frac;
      }
      * 
      cross=0;
      * 
      //DASSERT(peek_i==0);
      }
    */
    //FORWARD: when shrinking delay i.e moving read index forward in ringbuffer
    //BACKWARD when growning delay i,e moving read index back in ringbuffer
   
    if(do_cross==FORWARD)
    {
        //beacause line will shrink now and interpolate to new delaytime(less than current)
        //peekval may be invalidated: it was still set with curr_d fixit:
        if(peekval>cross_d)peekval=cross_d;
	
        //cout << "fwd" << endl;
        buff->add(getValue());
	
        cross+=cross_step;
	
        smp_t retval=0;
	
        if(mode==FRAC_INTERPOLATE_NONE)
        {
            smp_t to_val = buff->peek(curr_read+peek_i+peekval);
            smp_t from_val = buff->peek(curr_read+peekval);
	     
            retval = (smp_t)((1.0f-cross)*from_val + (cross)*to_val);
        }
        else if(mode==FRAC_INTERPOLATE_LINEAR)
        {
            smp_t to_val = (1.0-cross_d_frac)*buff->peek(curr_read+peek_i+peekval) 
                + cross_d_frac*buff->peek(curr_read+peek_i-1+peekval);
	     
            smp_t from_val = (1.0-curr_d_frac)*buff->peek(curr_read+peekval)  
                + curr_d_frac*buff->peek(curr_read-1+peekval);
	     
            retval = (smp_t)((1.0f-cross)*from_val + (cross)*to_val);
        }
        else
        {
            DERROR("no such mode");
        }
	
	
        if(cross>=1.0f)
        {
            do_cross=NONE;
	     
            curr_d=cross_d;
            curr_d_frac= cross_d_frac;
            curr_read = delay_len - curr_d;
	     
            //peek_i=0;
	     
            //DASSERT(buff->inBuffer()==curr_d+1);
        }
	
        buff->get();
	
        return retval;
    }
   
    if(do_cross==BACKWARD)
    {
        //cout << "bwd" << endl;
	
        buff->add(getValue()); 
	
        cross+=cross_step;
	
        smp_t retval=0;
	
        if(mode==FRAC_INTERPOLATE_NONE)
        {
            //delay line 1 bigger
            smp_t from_val = buff->peek(curr_read+peekval);
            smp_t to_val = buff->peek(curr_read-peek_i+peekval);
	     
            retval = (smp_t)(cross*to_val + (1.0f-cross)*from_val);
        }
        else if(mode==FRAC_INTERPOLATE_LINEAR)
        {
            smp_t from_val = (1.0-curr_d_frac) * buff->peek(curr_read+peekval) 
                + curr_d_frac*buff->peek(curr_read-1+peekval);
	     
            smp_t to_val = (1.0-cross_d_frac)*buff->peek(curr_read-peek_i+peekval)  
                + cross_d_frac*buff->peek(curr_read-peek_i-1+peekval);
	     
            retval = (smp_t)(cross*to_val + (1.0f-cross)*from_val);
        }
        else
        {
            DERROR("no such mode");
        }
	
	
        if(cross>=1.0f)
        {
            do_cross=NONE;
	     
            curr_d=cross_d;
            curr_d_frac= cross_d_frac;
            curr_read = delay_len - curr_d;
	     
            //DASSERT(buff->inBuffer()==curr_d+1);
        }
	
        buff->get();
	
        return retval;
    }
   
    if(do_cross==GROW)
    {
        //cout << "grow" << endl;
        //first let delay grow to new size while using old
        //peek_i will be old line read, cant use get() whilst it removes
        //when done tail of buffer will be new delay
	
        buff->add(getValue()); //always add
        delay_len++;
	
        //cout << "in:" << buff->inBuffer() << " pv:" << peekval << " cr:" << curr_read << " pi:" << peek_i << endl;
	
        //read value that was supposed to be returned if not growing
	
        smp_t retval = 0;
	
        if(mode==FRAC_INTERPOLATE_NONE)
        {
            //delay line 1 bigger
            retval = buff->peek(curr_read+peek_i+peekval);
	     
        }
        else if(mode==FRAC_INTERPOLATE_LINEAR)
        {
            retval = buff->peek(curr_read+peek_i+peekval)*(1.0-curr_d_frac) + buff->peek(curr_read+peek_i-1+peekval)*curr_d_frac;
        }
        else
        {
            DERROR("no such mode");
        }
	
        peek_i++;
	
        if(peek_i == to_add)
        {
            //ready to do the crossfade
	     
            //always grows to wanted_d+1:
            DASSERT(buff->inBuffer()==cross_d+1);
	     
            do_cross=BACKWARD;
            curr_read += peek_i;
            cross=0;
            peek_i=cross_d-curr_d;
	     
            /*do_cross=NONE;
              curr_d = cross_d;
              curr_d_frac = cross_d_frac;
              curr_read = delay_len - curr_d;*/
        }
	
        return retval;
    }
   
    ERROR("NEVER HERE");
    return 0; 
}

void Delay_MK2::setTimeMult(smp_t m)
{
    t_mult=m;
}








/*MK1*/

/*******/
/*DELAY*/
/*******/

Delay::Delay(uint max_delay_smp)
{
    t_mult=1.0;
   
    this->max_delay_smp = max_delay_smp;
   
    buff = new RingBuffer<smp_t>(max_delay_smp);
   
    delay=NULL;
    input=NULL;
   
    curr_d=-1;
    curr_d_frac=0;
   
    peek_i=0;
    cross=0;
    cross_step=0.05; //75.0f/(44100.0f); //FIXME: what is good?
    do_cross=NONE;
    warned=false;
    first_done=false;
   
    cross_d=0;
    cross_d_frac=0;
   
    mode = FRAC_INTERPOLATE_LINEAR;
}


void Delay::setDelay(Value** d)
{
    delay=d;
}

void Delay::setInput(Value** in)
{
    input=in;
}

void Delay::setTimeMult(smp_t m)
{
    t_mult=m;
}


/*NEED to do crossfade between old and new delay length if they are several samples long
 * or unwanted artifacts will be present*/

smp_t Delay::getValue()
{
    static bool warned=false;
   
    smp_t inval = 0;
    if(input != NULL)inval = (*input)->nextValue();
   
    if(!isfinite(inval))
    {
        if(!warned)
        {
            warned=true;
            DERROR("input is not finite! (only one warning)");
        }
        inval=0;
    }
   
    return inval;
}

void Delay::setFracInterpolateMode(F_INTER_MODE m)
{
    mode=m;
}


smp_t Delay::nextValue()
{
   
    /*DELAY LINE WILL ALWAYS BE +1 FROM WANTED INTEGER DELAY
     *this is so that fractional part is ok, o if wanted delay is
     * 10.4 samples delay line will be 11 saples long
     * 
     * remember: peek(+x) means delaytime-x
     * there are two interpolations happening: fractional and between delay times
     */
   
    smp_t delval=0;
    if(delay!=NULL)delval = (*delay)->nextValue();
   
    smp_t w_real_d = fabs(t_mult * delval);
   
    int wanted_d = (int)( w_real_d);
    smp_t wanted_d_frac = w_real_d - wanted_d;
   
    if(wanted_d > max_delay_smp)
    {
        if(!warned)
        {
            DERROR("(W) trying to set to long delay...only one warning");
            warned=true;
        }
	
        wanted_d=max_delay_smp;
        wanted_d_frac=0;
    }
   
   
    //INITIAL
    if(curr_d==-1)
    {
        curr_d=wanted_d;
        curr_d_frac=wanted_d_frac;
    }
   
    if(buff->inBuffer()< curr_d+1 )
    {
        DASSERT(!first_done); //<-remove me, DEBUG
	
        curr_d=wanted_d;
        curr_d_frac=wanted_d_frac;
	
        buff->add(getValue());
	
        return 0;  //nothing more to be done
    }
   
    first_done=true;  //<-remove me, DEBUG
   
    //NORMAL
    //if doing crossfade: let it finish
    if(wanted_d==curr_d && do_cross==NONE) 
    {
        curr_d_frac=wanted_d_frac;
	
        buff->add(getValue());
	
        smp_t val=0;
	
        if(mode==FRAC_INTERPOLATE_NONE)
        {
            //delay line 1 bigger
            val = buff->peek(1);
        }
        else if(mode==FRAC_INTERPOLATE_LINEAR)
        {
            val = buff->peek(1)*(1.0-curr_d_frac) + buff->peek(0)*curr_d_frac;
        }
        else
        {
            DERROR("no such mode");
        }
	
        buff->get();
	
        return val;
    }
   
    //CROSSFADE only operates on integer delay, i.e frac can change without crossfade happening
   
    //inBuffer() >= curr_d
   
    //inbuffer and curr_d stays the same untill crossfade is done
   
    //no new delay time while crossfading
   
   
    //SHRINK DELAY 
    if(wanted_d < curr_d && do_cross==NONE)
    {
        peek_i=curr_d-wanted_d;
        do_cross=FORWARD;
        cross_d=wanted_d;
        cross_d_frac = wanted_d_frac;
	
        /////
        /*do_cross=NONE;
          cross=0;
          * 
          curr_d=cross_d;
          * 
          buff->jumpFwd(peek_i); //only discards; buffer still +1
          peek_i=0;
          DASSERT(buff->inBuffer()==curr_d+1);
          * 
          return 0;*/
    }
   
    //GROW delay
    if(wanted_d>curr_d && do_cross==NONE)
    {
        do_cross=GROW;
        cross_d=wanted_d;
        cross_d_frac = wanted_d_frac;
	
        DASSERT(peek_i==0);
	
	
        ///
        /*	do_cross=NONE;
         * 
         for(int i=0;i<cross_d-curr_d;i++)buff->add(getValue()); //always add
         curr_d=cross_d;
         * 
         DASSERT(buff->inBuffer()==curr_d+1);
         * 
         return 0;*/
    }
   
    //FORWARD: when shrinking delay i.e moving read index forward in ringbuffer
    //BACKWARD when growning delay i,e moving read index back in ringbuffer
   
    if(do_cross==FORWARD)
    {
        buff->add(getValue());
	
        cross+=cross_step;
	
        smp_t retval=0;
	
        if(mode==FRAC_INTERPOLATE_NONE)
        {
            smp_t to_val = buff->peek(peek_i+1);
            smp_t from_val = buff->peek(1);
	     
            retval = (smp_t)((1.0f-cross)*from_val + (cross)*to_val);
        }
        else if(mode==FRAC_INTERPOLATE_LINEAR)
        {
            smp_t to_val = (1.0-cross_d_frac)*buff->peek(peek_i+1) + cross_d_frac*buff->peek(peek_i);
            smp_t from_val = (1.0-curr_d_frac)*buff->peek(1)  + curr_d_frac*buff->peek(0);
	     
            retval = (smp_t)((1.0f-cross)*from_val + (cross)*to_val);
        }
        else
        {
            DERROR("no such mode");
        }
	
	
	
        buff->get();
	
        if(cross>=1.0f)
        {
            do_cross=NONE;
            cross=0;
	     
            curr_d=cross_d;
            curr_d_frac= cross_d_frac;
	     
            buff->jumpFwd(peek_i); //only discards; buffer still +1
            peek_i=0;
	     
	     
            DASSERT(buff->inBuffer()==curr_d+1);
        }
	
        return retval;
    }
   
   
    if(do_cross==BACKWARD)
    {
        //DERROR("je");
	
        buff->add(getValue()); 
	
        cross+=cross_step;
	
        smp_t retval=0;
	
        if(mode==FRAC_INTERPOLATE_NONE)
        {
            //delay line 1 bigger
            smp_t from_val = buff->peek(peek_i+1);
            smp_t to_val = buff->peek(1);
	     
            retval = (smp_t)(cross*to_val + (1.0f-cross)*from_val);
        }
        else if(mode==FRAC_INTERPOLATE_LINEAR)
        {
            smp_t from_val = (1.0-curr_d_frac)*buff->peek(peek_i+1) + curr_d_frac*buff->peek(peek_i);
            smp_t to_val = (1.0-cross_d_frac)*buff->peek(1)  + cross_d_frac*buff->peek(0);
	     
            retval = (smp_t)(cross*to_val + (1.0f-cross)*from_val);
        }
        else
        {
            DERROR("no such mode");
        }
	
	
        buff->get();
	
        if(cross>=1.0f)
        {
            do_cross=NONE;
            cross=0;
            peek_i=0;
	     
            curr_d=cross_d;
            curr_d_frac= cross_d_frac;
	     
            DASSERT(buff->inBuffer()==curr_d+1);
        }
	
        return retval;
    }
   
    if(do_cross==GROW)
    {
	
        //first let delay grow to new size while using old
        //peek_i will be old line read, cant use get() whilst it removes
        //when done tail of buffer will be new delay
	
	
        buff->add(getValue()); //always add
	
        //read value that was supposed to be returned if not growing
	
        smp_t retval = 0; //buff->peek(peek_i);
	
        if(mode==FRAC_INTERPOLATE_NONE)
        {
            //delay line 1 bigger
            retval = buff->peek(peek_i+1);
	     
        }
        else if(mode==FRAC_INTERPOLATE_LINEAR)
        {
            retval = buff->peek(peek_i+1)*(1.0-curr_d_frac) + buff->peek(peek_i)*curr_d_frac;
        }
        else
        {
            DERROR("no such mode");
        }
	
	
        peek_i++;
	
        if(peek_i == cross_d-curr_d)
        {
            //ready to do the crossfade
	     
            do_cross=BACKWARD;
	     
            DASSERT(buff->inBuffer()==cross_d+1);
	     
        }
	
        return retval;
    }
   
    return 0; //NEVER HERE
}

}