#ifndef _BITMAPFONT_H
#define _BITMAPFONT_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdarg.h>

#include "screen.h"
#include "../util/debug.h"
#include "../util/types.h"

namespace spl {

class BitmapFont
{
    //initial, grows by wanted_len*2 every time buffer underruns
    static const uint NUM_TMPCHARS_INITIAL=100;
   
  public:
   
    //how is offset in font bitmap found corresponding to char:
    //ascii: offset = ascii_val*wdith
    //seek: use fontmap to find correspondence
    enum MATCH_MODE {UNUSED=0, ASCII, SEEK};
   
    BitmapFont(Screen* s);
    ~BitmapFont();
   
    bool setWrap(bool val)
    {
        bool w = wrap;
        wrap=val;
        return w;
    }
   
    bool loadFont(const char* bmap, const char* fontmap, MATCH_MODE mode);
    int setMask(Uint8 r, Uint8 g, Uint8 b);
    void printF(int x, int y, const char* fmt, ...);
    void printStr(int x, int y, const char* str, int len);
    bool drawCh(int x, int y, char ch);
   
    //FIXME: make a color cache with multiple bitmaps, if this is to be used
    //beacuse it is bitmap color is TO SLOW to do often
    void setColor(const SDL_Color& color);
    SDL_Color getColor();
    void setColor(Uint8 r, Uint8 g, Uint8 b);
   
    //FIXME: not good
    //interpolates r0->r1
    //void setColorRanch(Uint8 r0, Uint8 r1, Uint8 g0, Uint8 g1, Uint8 b0, Uint8 b1);
   
    int charH()
    {
        return char_h;
    }
    int charW()
    {
        return char_w;
    }
   
   
    void freeFont();
   
    bool error();
    string getError();
   
  private:
   
    void setError(const string& s);
   
    SDL_Color color;
   
    string  err_str;
    bool err;
   
    char* tmp_chars;
    uint tmp_chars_len;
   
    int char_w;
    int char_h;
    Screen* screen;
    SDL_Surface* bitmap;
    int nchars;
    char* chars;
    bool wrap;

    Uint32 mask;
    Uint8 maskRGB[3];

    MATCH_MODE mode;
};

}

#endif
