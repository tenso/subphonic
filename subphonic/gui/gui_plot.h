#ifndef GUI_PLOT_H
#define GUI_PLOT_H

#include <vector>
#include <sstream>
#include <string>

#include "gui.h"
#include "../screen/screen.h"
#include "../util/ringbuffer.h"

namespace spl {

using namespace std;

/*MOST THINGS here don't have a size pos.w, pos.h
 * this "seems" not to matter when they only draw something on screen, i.e no feedback
 * */

//draw a row of leds that light up one after another according to 'data'
template<class T>
class LedsLevel : public Component
{
  public:
    //default have threshold=0 and step = 1/n
    LedsLevel(SDL_Surface* on, SDL_Surface* off, uint n=1, bool vert=true, const T* data=NULL);
    ~LedsLevel();
   
    //reset show pixmap, use this to have different pixmaps per led
    void setPixmap(uint index, SDL_Surface* on, SDL_Surface* off);
   
    void draw(Screen* screen);
   
    void setData(const T* d);
   
    //first led will light up if data is over(>) this
    //default: 0
    void setThreshold(T lev);
    T getThreshold();
   
    //each consecutive led will light up if it is 'step' greater than the last
    void setLevelStep(T step);
   
  private:
    T lev;
    T step;
    const T* data;
    uint n;
    bool vert;
    SDL_Surface** on;
    SDL_Surface** off;
};

//FIXME: this is a little to specific
//draws a row of leds that light up one according to what index data is truncated to (int), neg = all off, 
//'ons' 'offs' is a periodic change of pixmap, speriod=-1 => period off
//i.e ons are drawin index%speriod | speriod!=-1
//space is the spaceing between consecutive leds, 
//WARNING: probably best if only non floating point 'T', i.e char, short, int etc(due to indexing)
//if data < 0 all are off
template<class T>
class LedsIndex : public Component
{
  public:
    LedsIndex(SDL_Surface* on, SDL_Surface* off, uint n=1, bool vert=true, int space=0,
    SDL_Surface* ons=NULL, SDL_Surface* offs=NULL, int speriod=-1, T* data=NULL);
   
    //FIXME: add    void setPixmap() as in Level;
   
    virtual void draw(Screen* screen);
   
    void setData(T* d)
    {
        data=d;
    }
   
  private:
   
    int space;
    T* data;
    uint n;
    bool vert;
    SDL_Surface* on;
    SDL_Surface* off;
   
    //led spacing
    SDL_Surface* ons;
    SDL_Surface* offs;
    int speriod;
};


//FIXME: name?

template<class T=coord_t>
class Histogram : public Component
{
  public:
    Histogram(uint w, uint h);
   
    //staples will have width=w/n
    //and height= h*data[i]*scale [and data will be:-1 +1 if clip on]
    //if use_sign=true : y=0 will be in middle of plot, else y=0 will be at bottom of plot(neg values clipped)
    //and data=1.0 will be whole range, i.e h*scale
    void setData(const T* data, uint len, bool use_sign=true, T scale=1.0);
   
    virtual void draw(Screen* screen);
   
    //default:255,255,255
    void setColor(const SDL_Color& color);
   
    //clips values over +- 1.0
    //default: on
    void setClip(bool v);
   
  private:
    bool use_sign;
    bool clip;
    SDL_Color color;
   
    coord_t staple_w;
   
    const T* data;
    uint len;
    T scale;
   
    uint w,h;
    uint half_h;
};

template<class T=coord_t>
class Plot : public Component
{
  public:
    Plot(uint w, uint h);
   
    //x=-1 y=1 will be at (0,0)
    //x=1 y=-1 will be at (w,h)
    //EX: set x_scale to 1/max(x) y_scale to 1/max(y) to make sure data fits in w/h
   
    //WARNING: x,y data better be atleast 'len' each in length!
    //it is possible to use setData(NULL, NULL, 0 ...) to disable draw
    void setData( const T* x_data, const T* y_data, uint len, T x_scale=1.0, T y_scale=1.0);
   
    //default:255,255,255
    void setColor(const SDL_Color& color);
   
    //default:on
    void connectPoints(bool v);
   
    virtual void draw(Screen* screen);
   
    //clips values over +- 1.0
    //default: on
    void setClip(bool v);
   
  private:
    const T* x_data;
    const T* y_data;
    T x_scale;
    T y_scale;
    uint len;
   
    bool clip;
   
    uint w;
    uint h;
    uint half_h;
    uint half_w;
   
    SDL_Color color;
    Uint32 c_color;
    bool have_c;
    bool con;
   
};



/*DataPlot is kind of special purpose because it only takes y-data as input(no x-coord)
 *one usage: it is good for showing a waveform in a "confined space(width)"*/

/*DataPlot(s) shows 'w' pixels with 'data' as y-comp indifferent how long 'data' is;
 * so that for example len=1 takes same space as len=99999 (w)
 * a 'data'-val of 'max' will be at y = 0 and -'max' at h
 * WARNING: only holds a pointer to data...(no free, dont free data and then use etc)
 * this is a speed issue: time changeing data would req a set/copy op every data change(might be very often)...
 * however it would be safer to have a set/copy data
 */

template<class T=coord_t>
class DataPlot : public Component
{
  public:
    DataPlot(uint w, uint h);
   
    void setData(const T* data, uint len, T max);
   
    //default:255,255,255
    void setColor(const SDL_Color& color);
   
    //default:on
    void connectPoints(bool v);
   
    virtual void draw(Screen* screen);
   
    //clips values over +- 'max' to max
    //default: on
    void setClip(bool v);
   
  private:
    const T* data;
    uint len;
   
    uint w,h;
   
    //calced
    float step;//by w
    float scale;//by h
    int half_h;
   
    SDL_Color color;
    Uint32 c_color;
    bool have_c;
    bool con;
   
    T max; 
   
    bool clip;
};

//same as DataPlot but takes a RingBuffer as input
template<class T>
class DataPlotRing : public Component
{
  public:
    DataPlotRing(uint w, uint h);
   
    void setData(spl::RingBuffer<T>* data, T max);
    void setColor(const SDL_Color& color);
    void connectPoints(bool v);
   
    virtual void draw(Screen* screen);
   
    void setClip(bool v);
   
    //default: false; plots newest values at right end(this reverses sound wave)
    void setReversePlot(bool v);
    bool getReversePlot();
   
  private:
    spl::RingBuffer<T>* data;
    uint len;
   
    uint w,h;
   
    //calced
    coord_t step;//by w
    coord_t scale;//by h
    int half_h;
   
    SDL_Color color;
    Uint32 c_color;
    bool have_c;
    bool con;
   
    T max;
   
    bool clip;
    bool reverse;
};

#include "gui_plot_T.h"
}
#endif
