#include "binfont.h"

namespace spl{

/*************/
/*BINFONTDATA*/
/*************/

BinFontData::BinFontData()
{
    data=NULL;
    ok=false;
}

bool BinFontData::load(const char* file)
{
    ifstream in(file, ios::in|ios::binary);
    if(in.fail())
    {
        cout << "load error: " << file << endl;
        return false;
    }
   
    in.read((char*)(&head), sizeof(Head));
    data = new unsigned char[head.data_bytes];
    in.read((char*)data, head.data_bytes);
   
    if(in.bad())
    {
        cout << "read error: " << file << endl;
        in.close();
        return false;
    }
   
    in.close();
    ok=true;
    return true;
}

bool BinFontData::save(const char* file)
{
    ofstream out(file, ios::out|ios::binary);
   
    if(out.fail())
    {
        cout << "save error: " << file << endl;
        return false;
    }
   
    out.write((char*)(&head), sizeof(Head));
    out.write((char*)data, head.data_bytes);
   
    if(out.bad())
    {
        cout << "write error: " << file << endl;
        out.close();
        return false;
    }
   
    out.close();
    return true;
}

void BinFontData::purge()
{
    delete data;
}


/********/
/*GLFONT*/
/********/


GLFont::GLFont()
{
    loadedfont=false;
   
   
    state.lightingOn(false);
    state.depthTestOn(false);
    state.blendOn(false);
    state.textureOn(false);
    //state.multiTexUnitOn(0,false);
    //state.multiTexUnitOn(1,false);
    //state.multiTexUnitOn(2,false);
        
    entered=false;
}

GLFont::~GLFont()
{
}

void GLFont::enter()
{
    D_ASSERT_M(!entered,"enter upon enter");
   
    state.apply();
   
    glPushAttrib(GL_TRANSFORM_BIT);
   
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    proj.loadGL();
   
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    view.loadGL();
   
    entered=true;
}

void GLFont::leave()
{
    D_ASSERT_M(entered,"leave without enter");
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
   
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
   
    glPopAttrib();
   
    entered=false;

    state.unApply();
}

void GLFont::pos(GLuint x, GLuint y)
{
    p_pos[0]=x;
    p_pos[1]=y;
}

void GLFont::color(float r, float g,float b, float a)
{
    p_col[0]=r;
    p_col[1]=g;
    p_col[2]=b;
    p_col[3]=a;
}


bool GLFont::load(const char* file)
{
    //first setup screnn, do this here so GLFont can be decalared before screen::setup()
    SDL_GL_Screen& s = SDL_GL_Screen::instance();
    D_ASSERT(s.isOk());
   
    proj = Matrix::ortho2D(0, s.getW(), 0, s.getH());
    view = Matrix::identity();

    BinFontData fnt;
   
    D_ASSERT_M(!loadedfont, "purge(), not called! ");
   
    if(!fnt.load(file))return false;
   
   
    char_w=fnt.head.w;
    char_h=fnt.head.h;
    num=fnt.head.num;
   
    makeFont(fnt.data);
    fnt.purge(); //loaded into gl lists
   
    loadedfont=true;
    return true;
}

bool GLFont::purge()
{
    D_ASSERT_M(loadedfont, "loadFont(), not called! ");
   
    glDeleteLists(fontoff, num);
   
    loadedfont=false;
   
    return true;
}


void GLFont::makeFont(const unsigned char* fontdata)
{
   
#warning glNewList() + glBitmap() "seems" to copy data, better look it up
   
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    fontoff = glGenLists(num);
   
    if(char_w==8)
    {
        GLubyte tmp[char_h];
	
        for(int i=0;i<num;i++)
        {
            //get one char
            memcpy(tmp,&fontdata[i*char_h], char_h);
	     
            //reverse y-axis for font
            for(int j=0;j<char_h/2;j++)
            {
                GLubyte t = tmp[j];
                tmp[j]=tmp[char_h-1-j];
                tmp[char_h-1-j]=t;
            }
            glNewList(fontoff+i, GL_COMPILE);
            glBitmap(char_w,char_h,0/*xoff*/,0/*yoff*/,char_w+1/*xinc*/,0/*yinc*/, tmp);
            glEndList();
	     
        }
	
        return;
    }
    if(char_w==16)
    {
        GLushort tmp[char_h]; 
	
        for(int i=0;i<num;i++)
        {
            //get one char
            memcpy(tmp,&fontdata[i*char_h*2], char_h*2);
	     
            //reverse y-axis for font
            for(int j=0;j<char_h/2;j++)
            {
                GLushort t = tmp[j];
                tmp[j]=tmp[char_h-1-j];
                tmp[char_h-1-j]=t;
            }
	     
            glNewList(fontoff+i, GL_COMPILE);
            glBitmap(char_w,char_h,0/*xoff*/,0/*yoff*/,char_w+1/*xinc*/,0/*yinc*/,(GLubyte*)(tmp));
            glEndList();
        }
        return;
    }
  
}


void GLFont::printF(const char* fmt, ...)
{     
    //PRINT
    va_list ap;
   
    va_start(ap,fmt); //remember va_start efter each v_command
    int len = vsnprintf(NULL,0,fmt,ap)+1; //get len
    va_end(ap);
   
    char* str=new char[len]; //FIXME: alloc each printF? = good?
   
    va_start(ap,fmt);
    int tmp = vsnprintf(str,len,fmt,ap);
    va_end(ap);
   
   
    D_ASSERT(len-1==tmp);
    str[len-1]='\0';
   
   
    /*APPLY pos+color*/
    glPushAttrib(GL_CURRENT_BIT);

    glColor4fv(p_col);
    glRasterPos2i(p_pos[0],p_pos[1]);
   
    /*print*/
    glPushAttrib(GL_LIST_BIT);
   
    glListBase(fontoff);
    glCallLists(len, GL_UNSIGNED_BYTE, (GLubyte*)str);
   
    glPopAttrib();
    glPopAttrib();
   
    delete str;
   
}


/********/
/*SWFont*/
/********/


SWFont::SWFont()
{
    bitmap=NULL;
    setWrap(true);
    loaded=false;
}


SWFont::~SWFont()
{
    if(loaded)purge();
}



void SWFont::color(Uint8 r, Uint8 g, Uint8 b)
{
    D_ASSERT_M(loaded, "no font loaded");
    //FIXME: WHAA!
    int bpp = bitmap->format->BytesPerPixel;
   
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
                    data[index] = SDL_MapRGB(bitmap->format,r,g,b);
                }
            }
        }
    }
    if(bpp==4)
    {
        Uint32* data = (Uint32*)bitmap->pixels;
	
        for(int y=0; y<bitmap->h;y++)
        {
            for(int x=0;x<bitmap->w;x++)
            {
                int index = x+y*bitmap->w;
                Uint32 tdata = data[index];
		  
                if(tdata!=mask)
                {
                    data[index] = SDL_MapRGB(bitmap->format,r,g,b);
                }
            }
        }
    }
   
}

int SWFont::setMask(Uint8 r, Uint8 g, Uint8 b)
{
    D_ASSERT_M(loaded, "no font loaded");
   
    mask = SDL_MapRGB(bitmap->format,r,g,b);
   
    return SDL_SetColorKey(bitmap,SDL_SRCCOLORKEY, mask);
}

bool SWFont::load(const char* file)
{
    D_ASSERT_M(!loaded, "font loaded!");
   
    BinFontData fnt;
    fnt.load(file);
    char_w=fnt.head.w;
    char_h=fnt.head.h;
    nchars=fnt.head.num;
   
    SDL_SW_Screen& screen = SDL_SW_Screen::instance();
   
    bitmap = buildBitmap(fnt, screen.getPixelFormat());
   
    fnt.purge();
   
    return true;
}

SDL_Surface* SWFont::buildBitmap(BinFontData& fnt, const SDL_PixelFormat* fmt)
{
    int w = fnt.head.w;
    int h = fnt.head.h;
    int num = fnt.head.num;
   
    //created black
    SDL_Surface* map = SDL_CreateRGBSurface(SDL_SWSURFACE, w*num, h,
    fmt->BitsPerPixel,
    fmt->Rmask,fmt->Gmask, fmt->Bmask,fmt->Amask);
   

   
    unsigned char* pixels = (unsigned char*)map->pixels;
    //Uint32 color = SDL_MapRGB(fmt, 0xff,0xff,0xff);
    SDL_PixelFormat fmt2 = *fmt;
   
    //set initital mask to bg color, not activeated though
    mask = SDL_MapRGB(&fmt2,0,0,0);
   
    Uint32 color = SDL_MapRGB(&fmt2, 0xff,0xff,0xff);
   
    unsigned int data_shift=0x1<<7;
   
    int dindex=0;
    for(int c=0;c<num;c++)
    {
        for(int y=0;y<h;y++)
        {
            unsigned char data = fnt.data[dindex];
            int shift=0;
	     
            for(int x=0;x<w;x++,shift++)
            {
                if(x==8)
                {
                    shift=0;
                    dindex++;
                    data = fnt.data[dindex];
                }
		  
		  
                if(! (data& (data_shift>>shift) ))continue;
		  
                int index=x+y*w*num+c*w;
		  
                pixels[index*fmt->BytesPerPixel+0]=((color&fmt->Rmask)>>fmt->Rshift)&0xff;
                pixels[index*fmt->BytesPerPixel+1]=((color&fmt->Gmask)>>fmt->Gshift)&0xff;
                pixels[index*fmt->BytesPerPixel+2]=((color&fmt->Bmask)>>fmt->Bshift)&0xff;
		  
            }
	     
            dindex++;
        }
    }
   
   
    return map;
}


void SWFont::purge()
{
    D_ASSERT_M(loaded, "no font loaded");
   
    SDL_FreeSurface(bitmap);
    bitmap=NULL;
    loaded=false;
}

void SWFont::printF(int x, int y, const char* fmt, ...)
{
    D_ASSERT_M(loaded, "no font loaded");
   
    va_list ap;
   
    va_start(ap,fmt); //remember va_start efter each v_command
    int len = vsnprintf(NULL,0,fmt,ap)+1; //get len
    va_end(ap);
   
    char* str=new char[len]; //FIXME: alloc each printF? = good?
   
    va_start(ap,fmt);
    /*int tmp =*/ vsnprintf(str,len,fmt,ap);
    va_end(ap);
   
    //D_ASSERT(tmp==len);
   
    printStr(x, y, str, len);

}

bool SWFont::drawCh(int x, int y, unsigned char ch)
{
    SDL_SW_Screen& screen = SDL_SW_Screen::instance();
    SDL_Rect boff,soff;
    bool set=false;
    soff.x=x;
    soff.y=y;
    boff.y=0;
    boff.w=char_w;
    boff.h=char_h;
   
    //if(mode==BITMAPFONT_ASCII)
    {
        boff.x = ch*boff.w;
        set=true;
    }
    /*else
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
      * 
      }*/
    if(!set)return false;
    screen.blit(bitmap,&soff,&boff);
    return true;
}


void SWFont::printStr(int x, int y, const char* str, int len)
{
    SDL_SW_Screen& screen = SDL_SW_Screen::instance();
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
	     
                if(xoff+char_w>screen.getW())
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
