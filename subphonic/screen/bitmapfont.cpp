#include "bitmapfont.h"
#include <string>

namespace spl {

using namespace std;

BitmapFont::BitmapFont(Screen* s)
{
    chars=NULL;
    bitmap=NULL;
    nchars=0;
    screen = s;
   
    tmp_chars_len = NUM_TMPCHARS_INITIAL;
    tmp_chars = new char[tmp_chars_len];
   
    setWrap(false);
    err_str.assign("none");
    err=false;
}


BitmapFont::~BitmapFont()
{
    freeFont();
    delete[] tmp_chars;
}
/*
void BitmapFont::setColorRanch(Uint8 r0, Uint8 r1, Uint8 g0, Uint8 g1, Uint8 b0, Uint8 b1)
{
    DASSERT(bitmap!=NULL);
   
    int bpp = bitmap->format->BytesPerPixel;
   
    if(bpp==2)
    {
        Uint16* data = (Uint16*)bitmap->pixels;
	
        int sr = (r1-r0)/char_w;
        int sg = (g1-g0)/char_w;
        int sb = (b1-b0)/(char_w+char_h);
	
        for(int c=0; c<nchars;c++)
        {
            for(int y=0; y<char_h;y++)
            {
                for(int x=0;x<char_w;x++)
                {
                    int index = c*char_w+x+y*bitmap->w;
                    Uint16 tdata = data[index];
		       
                    if(tdata!=mask)
                    {
                        data[index] = 
                            SDL_MapRGB(bitmap->format,  y*sr+r0,  y*sg+g0,  (x+y)*sb+b0);
                    }
                }
            }
        }
    }
   
    if(bpp==4)
    {
        Uint32* data = (Uint32*)bitmap->pixels;
	
        int sr = (r1-r0)/char_w;
        int sg = (g1-g0)/char_w;
        int sb = (b1-b0)/(char_w+char_h);
	
        for(int c=0; c<nchars;c++)
        {
            for(int y=0; y<char_h;y++)
            {
                for(int x=0;x<char_w;x++)
                {
                    int index = c*char_w+x+y*bitmap->w;
                    Uint32 tdata = data[index];
		       
                    if(tdata!=mask)
                    {
                        data[index] = 
                            SDL_MapRGB(bitmap->format,  y*sr+r0,  y*sg+g0,  (x+y)*sb+b0);
                    }
                }
            }
        }
    }
}
*/

void BitmapFont::setColor(const SDL_Color& color)
{
    setColor(color.r, color.g, color.b);
}

SDL_Color BitmapFont::getColor()
{
    return color;
}



void BitmapFont::setColor(Uint8 r, Uint8 g, Uint8 b)
{
    if(color.r==r && color.g==g && color.b==b)return;
   
    color.r=r;
    color.g=g;
    color.b=b;
   
    DASSERT(bitmap!=NULL);
   
    int bpp = bitmap->format->BytesPerPixel;
    Uint32 color = SDL_MapRGBA(bitmap->format,r,g,b, 0);
   
    int roff = bitmap->format->Rshift/8;
    int goff = bitmap->format->Gshift/8;
    int boff = bitmap->format->Bshift/8;
    int aoff = bitmap->format->Ashift/8;

    if(bpp==2)
    {
        Uint16* data = (Uint16*)bitmap->pixels;
	
        for(int y=0; y<bitmap->h;y++)
        {
            for(int x=0;x<bitmap->w;x++)
            {
                int index = x+y*bitmap->w;
                Uint16 tdata = data[index];
		  
                if(tdata!=mask)
                {
                    data[index] = color;
                }
            }
        }
    }
    if(bpp==3)
    {
        Uint8* data = (Uint8*)bitmap->pixels;
	
        for(int y=0; y<bitmap->h;y++)
        {
            for(int x=0;x<bitmap->w;x++)
            {
                int index = x*3+y*bitmap->w*3;
                
                //this might not be correct: byte ordering etc: dont have strength now to fix it
                if(data[index+roff] != maskRGB[0] && data[index+goff] != maskRGB[1] && data[index+boff] != maskRGB[2])
                {
                    data[index+roff] = r;
                    data[index+goff] = g;
                    data[index+boff] = b;
                }
            }
        }
    }
    if(bpp==4)
    {
        Uint8* data = (Uint8*)bitmap->pixels;
	
        for(int y=0; y<bitmap->h;y++)
        {
            for(int x=0;x<bitmap->w;x++)
            {
                int index = x*4+y*bitmap->w*4;
                Uint32 tdata = data[index];
		  
                if(data[index+roff] != maskRGB[0] && data[index+goff] != maskRGB[1] && data[index+boff] != maskRGB[2] 
                /*&& data[index+aoff] == 0xff*/)
                {
                    data[index+roff] = r;
                    data[index+goff] = g;
                    data[index+boff] = b;
                    //data[index+aoff] = 0xff;
                }
            }
        }
    }
   
}


int BitmapFont::setMask(Uint8 r, Uint8 g, Uint8 b)
{
    DASSERT(bitmap!=NULL);
   
    maskRGB[0] = r;
    maskRGB[1] = g;
    maskRGB[2] = b;

    mask = SDL_MapRGB(bitmap->format,r,g,b);
   
    return SDL_SetColorKey(bitmap,SDL_SRCCOLORKEY, mask);
}


bool BitmapFont::loadFont(const char* bmap,const char* fontmap, MATCH_MODE mode)
{
    if(bitmap!=NULL)
    {
        ERROR("already loaded");
        return false;
    }
   
    this->mode=mode;
   
    //LOAD FONTFILE, READ CHARS
    ifstream fin(fontmap, ios::in | ios::binary);
    if(fin == NULL)
    {
        setError(string("no such fontfile: ") + string(fontmap));
        err=true;
        return false;
    }
   
    fin.seekg(0,ios::end);
    nchars = fin.tellg();
   
    fin.seekg(0,ios::beg);
    chars = new char[nchars];
    fin.read(chars,nchars);
   
    fin.close();
   
    //LOAD BITMAP
    bitmap = SDL_LoadBMP(bmap);
    if(bitmap == NULL)
    {
        err=true;
        setError("no such bitmap file: " + string(bmap));
        return false;
    }
      
    char_w = bitmap->w/nchars;
    char_h = bitmap->h;
   
    //convert
    /*SDL_Surface* tmp = SDL_DisplayFormat(bitmap);
    SDL_FreeSurface(bitmap);
    bitmap = tmp;*/
   
    return true;
}

void BitmapFont::freeFont()
{
    if(bitmap==NULL)
    {
        ERROR("not loaded");
        return;
    }
      
    delete[] chars;
    SDL_FreeSurface(bitmap);
}



void BitmapFont::setError(const string& s)
{
    err_str=s;
}

string BitmapFont::getError()
{
    return err_str;
}

bool BitmapFont::error()
{
    return err;
}


bool BitmapFont::drawCh(int x, int y, char ch)
{
    SDL_Rect boff,soff;
    bool set=false;
    soff.x=x;
    soff.y=y;
    boff.y=0;
    boff.w=char_w;
    boff.h=char_h;
   
    if(mode==ASCII)
    {
        boff.x = ch*boff.w;
        set=true;
    }
    else if(mode==SEEK)
    {
        for(int i=0;i<nchars;i++)
        {
            if(ch == chars[i])
            {
                boff.x = i*boff.w;
                set=true;
                break;
            }
        }
    }
    else ERROR("no such mode");
   
    if(!set)return false;
   
    screen->blit(bitmap,&soff,&boff);
   
    return true;
}


void BitmapFont::printF(int x, int y, const char* fmt, ...)
{
    va_list ap;
   
    va_start(ap,fmt);
    int want_len = vsnprintf(tmp_chars, tmp_chars_len, fmt, ap); //want_len not inc '\0'
    va_end(ap);
   
    if(want_len >= tmp_chars_len)
    {
        delete[] tmp_chars;
        tmp_chars_len = want_len*2;
        tmp_chars = new char[tmp_chars_len];
	
        va_start(ap,fmt);
        want_len = vsnprintf(tmp_chars, tmp_chars_len, fmt, ap); 
        va_end(ap);
	
        DASSERT(want_len < tmp_chars_len);
    }
   
      
    printStr(x, y, tmp_chars, want_len);
   
   
}

void BitmapFont::printStr(int x, int y, const char* str, int len)
{
    int xoff=x;
    int yoff=y;
   
    for(int i=0;i<len;i++)
    {
        switch(*(str+i))
        {
            case '\r':
                xoff=x;
                break;
            case '\n':
                yoff+=char_h;
                xoff=x;
                break;
	     
            default:
                drawCh(xoff,yoff,*(str+i));
                xoff+=char_w;
	     
                if(xoff+char_w>screen->W())
                {
                    if(wrap)
                    {
                        xoff=x;
                        yoff+=char_h;
                    }
                    else return;
                }
                break;
        }
    }
}

}