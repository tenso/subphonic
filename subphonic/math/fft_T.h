//#include "fft.h"
using namespace spl;

template<class T>
void fft(T* in, Complex* out, int len) 
{
    //runs in O(n*lg(n))
    //len power of 2
    int ll=(int)log2(len);
   
    //permute array
    for(int i=0;i<len;i++)
    {
        out[i].set(in[bitReverse(i,ll)], 0.0);
    }
   
    for(int k=0;k<ll;k++)
    {
        int under_len=1<<k;
	
        //~300
        //omega+=inc; ==> order 10 more error in fft->ifft process
	
        for(int j=0;j<len;j+=under_len*2)
        {
            for(int i=0;i<under_len;i++)
            {
		  
                T omega = -i*M_PI/(T)under_len;
		  
                //not so much speed ~500, but same result so...
                //cos, sin is primary bottleneck ~3200/5000 (total g_comp inc)
                Complex e = Complex(cos(omega), sin(omega));
		  
                //VS: Complex e = EImgPow(-i*M_PI/(T)under_len).getComplex();
		  
		  
                Complex W = out[j+i+under_len]*e;
		  
                out[j+i+under_len] = out[j+i] - W;
                out[j+i] = out[j+i] + W;
		  
            }
        }
    } 
}

template<class T>
void ifft(Complex* in, T* out, int len)
{
    //runs in O(n*lg(n))
    //len power of 2
    int ll=(int)log2(len);
   
    Complex* t_out = new Complex[len]; //this is unnecessary?
   
    //permute array
    for(int i=0;i<len;i++)
    {
        t_out[i] = in[bitReverse(i,ll)]/len; //FIXME: (T?)
    }
   
    for(int k=0;k<ll;k++)
    {
        int under_len=1<<k;
	
        for(int j=0;j<len;j+=under_len*2)
        {
            for(int i=0;i<under_len;i++)
            {
                //~500
                T omega = i*M_PI/(T)under_len;
                Complex e = Complex(cos(omega),sin(omega));
		  
                //VS: Complex e = EImgPow(i*M_PI/(T)under_len).getComplex();
		  
		  
                Complex W = t_out[j+i+under_len]*e;
		  
                t_out[j+i+under_len] = t_out[j+i] - W;
                t_out[j+i] = t_out[j+i] + W;
		  
            }
        }
    }
   
    for(int i=0;i<len;i++)
    {
        out[i] = t_out[i].getRe(); //is this is good(i.e throw img)?
    }
   
    delete[] t_out;
}



/**********/
/*FFTFIXED*/
/**********/

template<class T>
FFTfixed<T>::FFTfixed()
{
    len=0;
    mem_usage=0;
}

template<class T>
FFTfixed<T>::~FFTfixed()
{
    free();
}

template<class T>
void FFTfixed<T>::setup(uint len)
{
    if(len<2)
    {
        ERROR("len must be > 1");
        return;
    }
   
   
    this->len = len;
    this->loglen = (uint)log2(len);
   
    mem_usage = 2*loglen*len*sizeof(T);
   
    neg_e = new Complex*[loglen];
    pos_e = new Complex*[loglen];
   
    for(uint k=0;k<loglen;k++)
    {
        neg_e[k] = new Complex[len];
        pos_e[k] = new Complex[len];
    }
      
    for(uint k=0;k<loglen;k++)
    {
        uint under_len=1<<k;
	
        for(uint i=0;i<under_len;i++)
        {
            T omega = i*M_PI/(T)under_len;
            pos_e[k][i] = Complex(cos(omega), sin(omega));
	     
	     
            //recalcing omega makes things worse: higher fft->ifft error
            omega = -omega;
            neg_e[k][i] = Complex(cos(omega), sin(omega));;
        }
	
    } 
}

template<class T>
void FFTfixed<T>::free()
{
   
    for(uint k=0;k<loglen;k++)
    {
        delete[] neg_e[k];
        delete[] pos_e[k];
    }
   
    delete[] neg_e;
    delete[] pos_e;
   
    len=0;
    loglen=0;
    mem_usage=0;
}

template<class T>
uint FFTfixed<T>::getBytesUsed() const
{
    return mem_usage;
}

template<class T>
void FFTfixed<T>::fft(T* in, Complex* out) const
{
    if(len==0)
    {
        ERROR("fft fail: not setup");
        return;
    }
   
    //permute array
    for(uint i=0;i<len;i++)
    {
        out[i].set(in[bitReverse(i,loglen)], 0.0);
    }
   
    for(uint k=0;k<loglen;k++)
    {
        uint under_len=1<<k;
	
        for(uint j=0;j<len;j+=under_len*2)
        {
            for(uint i=0;i<under_len;i++)
            {
                //VS: Complex e = EImgPow(-i*M_PI/(T)under_len).getComplex();
                Complex W = out[j+i+under_len]*neg_e[k][i];
		  
                out[j+i+under_len] = out[j+i] - W;
                out[j+i] = out[j+i] + W;
		  
            }
        }
    } 
}

template<class T>
void FFTfixed<T>::ifft(Complex* in, T* out) const
{
    if(len==0)
    {
        ERROR("ifft fail: not setup");
        return;
    }
   
    Complex* t_out = new Complex[len]; //this is unnecessary?
   
    //permute array
    for(uint i=0;i<len;i++)
    {
        t_out[i] = in[bitReverse(i,loglen)]/len; //FIXME: (T?)
    }
   
    for(uint k=0;k<loglen;k++)
    {
        uint under_len=1<<k;
	
        for(uint j=0;j<len;j+=under_len*2)
        {
            for(uint i=0;i<under_len;i++)
            {
                //VS: Complex e = EImgPow(i*M_PI/(T)under_len).getComplex();
                Complex W = t_out[j+i+under_len]*pos_e[k][i];
		  
                t_out[j+i+under_len] = t_out[j+i] - W;
                t_out[j+i] = t_out[j+i] + W;
		  
            }
        }
    }
   
    for(uint i=0;i<len;i++)
    {
        out[i] = t_out[i].getRe(); //is this is good(i.e throw img)?
    }
   
    delete[] t_out;
}
