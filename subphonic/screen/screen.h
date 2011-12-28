#ifndef _SCREEN_H
# define _SCREEN_H

#include <iostream>
#include <vector>
#include <map>
#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace spl {

using namespace std;

/*
 * IMPLEMENT: change ALL SDL_Surface* to ScreenSurface*
 * this is GOOD: makes it possible to change aplication to say OpenGL with just another Screen Type
 * i.e., with all the logic implemented here that is. ScreenSurface could then create a tecture from the SDL_Surface etc
 
 class ScreenSurface
 {
 public:
 ScreenSurface();
 ScreenSurface(filename);
 ~ScreenSurface();
 bool loadFile(string file);
 void free();
  
 private:
 SDL_Surface* surf;
 };
 
 class SW_Surface : pubic ScreenSurface
 {};
  
 class GL_Surface: public ScreenSurface
 {};
 
*/



//#include "util.h"

class Screen
{
  public:
   
    Screen(int w, int h);
   
    virtual ~Screen();
    virtual bool ok() = 0;
    virtual void plot(int x, int y, Uint32 color) = 0;
    virtual void show(int x=0, int y=0, int w=0, int h=0) = 0;
    virtual Uint32 makeColor(Uint8 r, Uint8 g, Uint8 b) = 0;
    virtual void toggleFullScreen() = 0;
   
    void blit(SDL_Surface* s, int x, int y)
    {
        SDL_Rect pos;
        pos.x=x;
        pos.y=y;
       
        blit(s, &pos, NULL);
    }
   
   
    //FIXME: maybe const &
    //pos: x,y destination
    //off:x,y is offset in 's';  w,h is size of blit
    virtual void blit(SDL_Surface* s, SDL_Rect* pos, SDL_Rect* off=NULL) = 0;
   
    Uint32 makeColor(SDL_Color c)
    {
        return makeColor(c.r,c.g,c.b);
    }
    int W()
    {
        return w;
    }
    int H()
    {
        return h;
    }
   
    virtual void fill(Uint32 color);
   
    //the boxes are contained within coords
    virtual void wirebox(int x0, int y0, int x1, int y1, Uint32 color, int size=1);
    virtual void box(int x0, int y0, int x1, int y1, Uint32 color);
    virtual void box(const SDL_Rect& r, Uint32 color)
    {
        box(r.x, r.y, r.x+r.w, r.y+r.h, color);
    }
    virtual void wirebox(const SDL_Rect& r, Uint32 color)
    {
        wirebox(r.x, r.y, r.x+r.w, r.y+r.h, color);
    }
   
   
    //if size != 1 line will expand to the right/down
    //so if line is horizontal it will start yo and end at y0+size
    virtual void line(int x0, int y0, int x1, int y1, Uint32 color, int size=1);
    virtual void line(const SDL_Rect& s, const SDL_Rect& e, Uint32 color, int size=1)
    {
        line(s.x, s.y, e.x, e.y, color, size);
    }
   
    //UGLY!
    virtual void roundedLine(int x0, int y0, int x1, int y1,
    const SDL_Color& s_color, const SDL_Color& e_color, int size);
   
  protected:
    int w;
    int h;
};


/**********/
/*SWSCREEN*/
/**********/

class SDL_SW_Screen : public Screen
{
  public:
   
    SDL_SW_Screen(int w, int h, int bpp, int flag=0);
    ~SDL_SW_Screen();
   
    bool ok();

    void plot(int x, int y, Uint32 color);
    void show(int x=0, int y=0, int w=0, int h=0);
      
    Uint32 makeColor(Uint8 r, Uint8 g, Uint8 b);

    void toggleFullScreen();
   
    void blit(SDL_Surface* s, SDL_Rect* pos, SDL_Rect* off=NULL);
   
    virtual void box(int x0, int y0, int x1, int y1, Uint32 color);
   
  private:
    SDL_Surface* screen; 
    Uint8* pixels;
    Uint8 bpp;
    int flags;
    Uint16 pitch;
   
};

/**********/
/*GLSCREEN*/
/**********/

//#include <spl/scene/scene_state_tex.h>
#include <map>

class SDL_GL_Screen : public Screen
{
  public:
   
    SDL_GL_Screen(int w, int h, int bpp, int flag=0);
    ~SDL_GL_Screen();
   
    bool ok();

    void plot(int x, int y, Uint32 color);
    void show(int x=0, int y=0, int w=0, int h=0);
      
    Uint32 makeColor(Uint8 r, Uint8 g, Uint8 b);

    void toggleFullScreen();
   
    void blit(SDL_Surface* s, SDL_Rect* pos, SDL_Rect* off=NULL);
   
    virtual void box(int x0, int y0, int x1, int y1, Uint32 color);
    void line(int x0, int y0, int x1, int y1, Uint32 color, int size);
  private:

    class RenderData
    {
      public:
        //spl::TextureState texState;
        SDL_Surface* surf;
        vector<GLfloat> verts;
        vector<int> texCoords;
    };
    
    map<SDL_Surface*, RenderData> renderData;
    map<SDL_Surface*, RenderData> renderDataAlpha;
    GLfloat blitLayer;

    SDL_Surface* screen; 
    //Uint8* pixels;
    Uint8 bpp;
    int flags;
    Uint16 pitch;
   
};

}

#endif
