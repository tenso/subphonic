#ifndef SPL_BINFONT_H
# define SPL_BINFONT_H

# include <iostream>
# include <fstream>

# include <stdarg.h>
# include <string.h>

# include <SDL.h>

#include <GL/glew.h>
# include <GL/gl.h>
//# include <GL/glu.h>
//# include <GL/glut.h>

# include "screen.h"

# include <util/debug.h>

# include <math/matrix.h>

# include <scene/scene_state.h>

namespace spl
{

using namespace std;


class BinFontData
{
  private:
    class Head
    {
      public:
        unsigned char w;
        unsigned char h;
        unsigned int num;
        unsigned int data_bytes;
    };
   
    bool ok;
   
  public:
    Head head;
    unsigned char* data;
   
    BinFontData();
    bool load(const char* file);   
    bool save(const char* file);
    void purge();
   
};
/*
  class Font
  {
  public:
  virtual void printF(int x, int y, char* fmt, ...) = 0;
  virtual bool load(char* file) = 0;
  virtual bool purge() = 0;
  virtual int charH() = 0;
  virtual int charW() = 0;   
  virtual void color(Uint8 r, Uint8 g, Uint8 b) = 0;
  * 
  };*/

//SET good opengl state before printing (use SceneState)
class GLFont //: public Font
{
  public:
    GLFont();
    ~GLFont();   
   
    //does nothing more than print, uses last font loaded
    //use glRasterPos(nt)() to position
    //glOrtho2d, glColor() etc...
    void printF(const char* fmt, ...);

    //some "simple" highlevel for a fast print
    //make sure to just do: enter(); N x printF(); leave();
    void enter();
    void leave();
    void pos(GLuint x, GLuint y);
    void color(float r, float g,float b, float a);
   
    bool load(const char* file);
    bool purge();
   
    int charH()
    {
        return char_h;
    }
    int charW()
    {
        return char_w;
    }
   
   
  private:
    SceneState state;
    Matrix proj;
    Matrix view;
    GLuint  p_pos[2];
    GLfloat p_col[4];
    bool entered;
   
    void makeFont(const unsigned char* fontdata);
    int char_h;
    int char_w;
    int num;
   
    GLuint fontoff;
    bool  loadedfont;
   
};

class SWFont // : public Font
{
  public:
    SWFont();   
    ~SWFont();
   
    void printF(int x, int y, const char* fmt, ...);
    bool load(const char* file);
    void purge();
   
    int setMask(Uint8 r, Uint8 g, Uint8 b);
    void color(Uint8 r, Uint8 g, Uint8 b);
   
    void setWrap(bool val)
    {
        wrap=val;
    }
   
    int charH()
    {
        return char_h;
    }
    int charW()
    {
        return char_w;
    }
   
   
  private:
   
    SDL_Surface* buildBitmap(BinFontData& fnt, const SDL_PixelFormat* fmt);
    void printStr(int x, int y, const char* str, int len);
    bool drawCh(int x, int y, unsigned char ch);
   
    SDL_Surface* bitmap;
    int char_w;
    int char_h;
    int nchars;
    bool wrap;
    Uint32 mask;
    bool loaded;
};

}

#endif
