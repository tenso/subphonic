//#include "gui_plot.h"

/******/
/*Leds*/
/******/

template<class T>
LedsLevel<T>::LedsLevel(SDL_Surface* on, SDL_Surface* off, uint n, bool vert, const T* data)
{
    this->data=data;
    lev=0;
    step=1.0/(float)n;
   
    this->n=n;
    this->vert=vert;
   
    this->on=NULL;
    this->off=NULL;
   
    if(n>0)
    {
        this->on = new SDL_Surface*[n];
        this->off = new SDL_Surface*[n];
	
        for(uint i=0;i<n;i++)
        {
            this->on[i]=on;
            this->off[i]=off;
        }
    }
}

template<class T>
LedsLevel<T>::~LedsLevel()
{
    delete[] on;
    delete[] off;
}

template<class T>
void LedsLevel<T>::setPixmap(uint index, SDL_Surface* on, SDL_Surface* off)
{
    if(index>=n)
    {
        ERROR("index out of bounds");
        return;
    }
   
    this->on[index]=on;
    this->off[index]=off;
}


template<class T>
void LedsLevel<T>::draw(Screen* screen)
{
    if(n==0)return;
   
    T ulev=lev;
   
    SDL_Rect p = pos;
   
    if(vert)p.y+=on[0]->h*n;
   
    for(uint i=0;i<n;i++)
    {
        if(data!=NULL && *data>ulev)
        {
            screen->blit(on[i], &p);
        }
        else
        {
            screen->blit(off[i], &p);
        }
	
        ulev+=step;
	
        if(vert)p.y-=on[i]->h;
        else p.x+=on[i]->w;
    }
}

template<class T>
void LedsLevel<T>::setData(const T* d)
{
    data=d;
}

//first led will light up if data is over this

template<class T>
void LedsLevel<T>::setThreshold(T lev)
{
    this->lev=lev;
}

template<class T>
T LedsLevel<T>::getThreshold()
{
    return lev;
}


//each consecutive led will light up if it is 'step' greater than the last
template<class T>
void LedsLevel<T>::setLevelStep(T step)
{
    this->step=step;
}


template<class T>
LedsIndex<T>::LedsIndex(SDL_Surface* on, SDL_Surface* off, uint n, bool vert, int space,
SDL_Surface* ons, SDL_Surface* offs, int speriod, T* data)
{
    this->data=data;
    this->n=n;
    this->vert=vert;
    this->on=on;
    this->off=off;
    this->space=space;
    this->ons=ons;
    this->offs=offs;
    this->speriod=speriod;
   
    if(speriod!=-1)
    {
        DASSERT(ons!=NULL);
        DASSERT(offs!=NULL);
    }
   
    //pos.w=;
}

template<class T>
void LedsIndex<T>::draw(Screen* screen)
{
    DASSERT(data!=NULL);
    if(data==NULL)return;
   
    int lev=(int)(*data);
   
    SDL_Rect p = pos;
   
    for(uint i=0;i<n;i++)
    {
        if(lev==(int)i) //led on
        {
            if(speriod==0)screen->blit(ons, &p);
            else if(speriod>0)
            {
                if(i%speriod==0)screen->blit(ons, &p);
                else screen->blit(on, &p);
            }
            else screen->blit(on, &p);
        }
        else //led off
        {
            if(speriod==0)screen->blit(offs, &p);
            else if(speriod>0)
            {
                if(i%speriod==0)screen->blit(offs, &p);
                else screen->blit(off, &p);
            }
            else screen->blit(off, &p);
        }
	
        if(vert)p.y+=on->h+space;
        else p.x+=on->w+space;
    }
}

/***********/
/*HISTOGRAM*/
/***********/

template<class T>
Histogram<T>::Histogram(uint w, uint h)
{
    data=NULL;
    len=0;
    use_sign=true;
    staple_w=0;
   
    this->w=w;
    this->h=h;
    half_h=h/2;
   
    this->scale=1.0;
   
    color.r=255;
    color.g=255;
    color.b=255;
   
    setClip(true);
}

template<class T>
void Histogram<T>::setData(const T* data, uint len, bool use_sign, T scale)
{
    this->use_sign=use_sign;
    this->data=data;
    this->len=len;
    this->scale=scale;
   
    staple_w=w/(coord_t)len;
   
    if(staple_w*len>w)
    {
        DERROR("FIXME");
    }
}

template<class T>
void Histogram<T>::setColor(const SDL_Color& color)
{
    color=color;
}

template<class T>
void Histogram<T>::setClip(bool v)
{
    clip=v;
}

template<class T>
void Histogram<T>::draw(Screen* screen)
{
    if(data==NULL)return;
   
    Uint32 u_color = screen->makeColor(color);
   
    coord_t x = pos.x;
    int y = pos.y;
   
    if(use_sign)
        y += half_h;
    else
        y += h;
   
    for(uint i=0;i<len;i++)
    {
        T dval = data[i];
	
        if(!use_sign)
        {
            if(dval<0)dval=0;
	     
            if(clip)
            {
                if(dval>1.0)dval=1.0;
            }
	     
            dval*=2; //so that EQ(1) is *2*half_h
        }
        else
        {
            if(clip)
            {
                if(dval>1.0)dval=1.0;
                else if(dval<-1.0)dval=-1.0;
            }
        }
	
	
        int ydata = (int)(scale*dval*half_h); // EQ(1)
	
        screen->box((int)x, y, (int)(x+staple_w), y-ydata, u_color);
	
        x+=staple_w;
    }
}


/******/
/*PLOT*/
/******/

template<class T>
Plot<T>::Plot(uint w, uint h)
{
    this->w=w;
    this->h=h;
    half_h=h/2;
    half_w=w/2;
   
    x_data=NULL;
    y_data=NULL;
    len=0;
   
    color.r=255;
    color.g=255;
    color.b=255;
    have_c=false;
   
    con=true;
   
    clip=true;
   
}

template<class T>
void Plot<T>::setData( const T* x_data, const T* y_data, uint len, T x_scale, T y_scale)
{
    this->x_data=x_data;
    this->y_data=y_data;
    this->len=len;
   
    this->x_scale=x_scale;
    this->y_scale=y_scale;
   
}

template<class T>
void Plot<T>::setColor(const SDL_Color& color)
{
    this->color=color;
    have_c=false;
}

template<class T>
void Plot<T>::connectPoints(bool v)
{
    con=v;
}


template<class T>
void Plot<T>::draw(Screen* screen)
{
    if(x_data==NULL || y_data==NULL || len==0)return;
   
    if(!have_c)
    {
        c_color = screen->makeColor(color);
        have_c=true;
    }
   
    int oldx=0;
    int oldy=0;
   
    for(uint i=0;i<len;i++)
    {
        T xval = x_data[i]*x_scale;
        T yval = y_data[i]*y_scale;
	
        if(clip)
        {
            if(xval>1.0)
            {
                xval=1.0;
            }
            if(xval<-1.0)
            {
                xval=-1.0;
            }
            if(yval>1.0)
            {
                yval=1.0;
            }
            if(yval<-1.0)
            {
                yval=-1.0;
            }
        }
	
        int xs = (int)(pos.x + (xval+1.0)*(half_w-1));
        int ys = (int)(pos.y + (half_h-1) - yval*(half_h-1));
	
        if(con && len>1)
        {
            if(i>0)screen->line(oldx,oldy, xs, ys, c_color);
	     
            oldx = xs;
            oldy = ys;
        }
        else screen->plot(xs, ys, c_color);
	
    }
}

template<class T>
void Plot<T>::setClip(bool v)
{
    clip=v;
}


/**********/
/*DATAPLOT*/
/**********/

template<class T>
DataPlot<T>::DataPlot(uint w, uint h)
{
    this->w=w;
    this->h=h;
    half_h=h/2;
   
    data=NULL;
    len=0;
   
    have_c=false;
   
    step=0;
    scale=0;
   
    color.r=255;
    color.g=255;
    color.b=255;
   
    con=true;
   
    clip=true;
}

template<class T>
void DataPlot<T>::setData(const T* data, uint len, T max)
{
    this->data=data;
    this->len=len;
    this->max=max;
   
    step = len/(float)w;
   
    scale = half_h/(coord_t)max;
   
    if((uint)(step*w)>len)
    {
        DERROR("FIXME");
    }
   
    //cout << (int)(step*w) << " " << len << endl;
}

template<class T>
void DataPlot<T>::setColor(const SDL_Color& color)
{
    this->color=color;
    have_c=false;
}

template<class T>
void DataPlot<T>::connectPoints(bool v)
{
    con=v;
}


template<class T>
void DataPlot<T>::draw(Screen* screen)
{
    if(data==NULL || len==0)return;
   
    if(!have_c)
    {
        c_color = screen->makeColor(color);
        have_c=true;
    }
   
    int yoff=pos.y+half_h;
   
    int x=pos.x;
   
    T dval = data[0];
      
    if(clip)
    {
        if(dval>max)
        {
            dval=max;
        }
        if(dval<-max)
        {
            dval=-max;
        }
    }
    T olddval=dval;
   
    int oldy=yoff-(int)(scale*dval);
   
    for(uint i=1;i<w;i++)
    {
        dval = data[(int)(i*step)];
	
        bool dodraw=true;
        if(clip)
        {
            if(dval>max)
            {
                dval=max;
                if(olddval>=max)dodraw=false;
            }
            if(dval<-max)
            {
                dval=-max;
                if(olddval<=-max)dodraw=false;
            }
        }
        olddval=dval;
	
        int val = (int)(yoff-scale*dval);
	
        if(dodraw)
        {
            if(con)screen->line(x,oldy, x+1, val, c_color);
            else screen->plot(x, val, c_color);
        }
	
        x++;
        oldy=val;
    }
}

template<class T>
void DataPlot<T>::setClip(bool v)
{
    clip=v;
}


/**************/
/*DATAPLOTRING*/
/**************/

template<class T>
DataPlotRing<T>::DataPlotRing(uint w, uint h)
{
    this->w=w;
    this->h=h;
   
    data=NULL;
    len=0;
   
    step=0;
    scale=0;
   
    color.r=255;
    color.g=255;
    color.b=255;
    have_c=false;
   
    con=true;
   
    clip=true;
   
    reverse=false;
}

template<class T>
void DataPlotRing<T>::setData(spl::RingBuffer<T>* data, T max)
{
    this->data=data;
    len=data->size();
   
    this->max=max;
   
    step = len/(coord_t)w;
    half_h=h>>1;
    scale = half_h/(coord_t)max;
   
    if((uint)(step*w)>len)DERROR("FIXME");
}

template<class T>
void DataPlotRing<T>::setColor(const SDL_Color& color)
{
    this->color=color;
    have_c=false;
}

template<class T>
void DataPlotRing<T>::connectPoints(bool v)
{
    con=v;
}

template<class T>
void DataPlotRing<T>::setReversePlot(bool v)
{
    reverse=v;
}

template<class T>
bool DataPlotRing<T>::getReversePlot()
{
    return reverse;
}


template<class T>
void DataPlotRing<T>::draw(Screen* screen)
{
    //this function is REALLY ugly, uses headIndex() etc...
    //probably need to redesign RingBuffer altogether
    //draw from head downto tail, i.e reversed
   
    if(data==NULL)return;
   
    if(!have_c)
    {
        c_color = screen->makeColor(color);
        have_c=true;
    }
   
    int x=pos.x;
    int yoff=pos.y+half_h;
    int oldy=yoff;
   
    T olddval=0;
   
    if(!reverse)
    {
        uint read=data->inBuffer()-1;
	
        if(data->inBuffer()>0) //still plot: 'zero'
        {
            T dval = data->peek(read);
	     
            if(clip)
            {
                if(dval>max)
                {
                    dval=max;
                }
                if(dval<-max)
                {
                    dval=-max;
                }
            }
	     
            oldy-=(int)(scale*dval);
            olddval=dval;
        }
	
	
        for(uint i=1;i<w;i++)
        {
            uint read_off = (uint)(i*step);
	     
            int val=yoff;
	     
            bool dodraw=true;
            if(read-read_off>=0)
            {
                T dval = data->peek(read-read_off);
		  
                if(clip)
                {
                    if(dval>max)
                    {
                        dval=max;
                        if(olddval>=max)dodraw=false;
                    }
                    if(dval<-max)
                    {
                        dval=-max;
                        if(olddval<=-max)dodraw=false;
                    }
                }
		  
                val -= (int)(scale*dval);
                olddval=dval;
            }
            //else: still continue plot
	     
            if(dodraw)
            {
                if(con)screen->line(x,oldy, x+1, val, c_color);
                else screen->plot(x, val, c_color);
            }
	     
            x++;
            oldy=val;
        }
    }
    else
    {
        uint read=0;
	
        if(data->inBuffer()>0)
        {
            T dval = data->peek(read);
	     
            if(clip)
            {
		  
                if(dval>max)
                {
                    dval=max;
                }
                if(dval<-max)
                {
                    dval=-max;
                }
            }
	     
            oldy-=(int)(scale*dval);
            olddval=dval;
        }
	
	
        for(int i=1;i<w;i++)
        {
            int read_off = (int)(i*step);
	     
            int val=yoff;
            bool dodraw=true;
            if(read+read_off<data->inBuffer())
            {
                T dval = data->peek(read+read_off);
		  
                if(clip)
                {
                    if(dval>max)
                    {
                        dval=max;
                        if(olddval>=max)dodraw=false;
                    }
                    if(dval<-max)
                    {
                        dval=-max;
                        if(olddval<=-max)dodraw=false;
                    }
                }
                olddval=dval;
                val -= (int)(scale*dval);
            }
            //else: still continue plot
	     
            if(dodraw)
            {
                if(con)screen->line(x,oldy, x+1, val, c_color);
                else screen->plot(x, val, c_color);
            }
	     
            x++;
            oldy=val;
        }
    }
   
}

template<class T>
void DataPlotRing<T>::setClip(bool v)
{
    clip=v;
}
