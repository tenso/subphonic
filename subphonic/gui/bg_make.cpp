#include "bg_make.h"

#include <sstream>

namespace spl {
using namespace std;

SDL_Surface* assembleBG(SDL_Surface* pices, SDL_Rect* wanted,
SDL_Rect& topleft,  SDL_Rect& left,  SDL_Rect& bottomleft,
SDL_Rect& bottom,	 SDL_Rect& bottomright,  SDL_Rect& right,
SDL_Rect& topright,  SDL_Rect& top,  Uint32 bg_color)
{
   
    SDL_Surface* bg = SDL_CreateRGBSurface(SDL_SWSURFACE, wanted->w, wanted->h,
    pices->format->BitsPerPixel,
    pices->format->Rmask,
    pices->format->Gmask,
    pices->format->Bmask,
    pices->format->Amask);
   
    if(bg==NULL)
    {
        DERROR("surface create failed");
        return NULL;
    }
   
    int err; //-1:error 0:ok
   
    //BG COLOR
    err = SDL_FillRect(bg, NULL, bg_color);
    DASSERT(err==0);
   
    SDL_Rect dst;
   
    //DO CORNERS
   
    //TOPLEFT
    dst.x=0;
    dst.y=0;
   
    err = SDL_BlitSurface(pices, &topleft, bg, &dst);
    DASSERT(err==0);
   
    //BOTTOMLEFT
    dst.x=0;
    dst.y=wanted->h-bottomleft.h;
    err = SDL_BlitSurface(pices, &bottomleft, bg, &dst);
    DASSERT(err==0);
   
    //TOPRIGHT
    dst.x=wanted->w-topright.w;
    dst.y=0;
    err = SDL_BlitSurface(pices, &topright, bg, &dst);
    DASSERT(err==0);
   
    //BOTTOMRIGHT
    dst.x=wanted->w-bottomright.w;
    dst.y=wanted->h-bottomright.h;
    err = SDL_BlitSurface(pices, &bottomright, bg, &dst);
    DASSERT(err==0);
   
    //DO BORDER
   
    //TOP
    int want=wanted->w-topleft.w-topright.w;
    int done=0;
   
    while(want>0)
    {
        if(want>top.w) //take all from pices
        {
            dst.x=topleft.w+done;
            dst.y=0;
            err = SDL_BlitSurface(pices, &top, bg, &dst);
            DASSERT(err==0);
	     
            done+=top.w;
            want-=top.w;
        }
        else //take what is neeeded
        {
            dst.x=topleft.w+done;
            dst.y=0;
            SDL_Rect take=top;
            take.w=want;
	     
            err = SDL_BlitSurface(pices, &take, bg, &dst);
            DASSERT(err==0);
	     
            done+=top.w;
            want-=top.w;
        }
    }
   
    //bottom
    want=wanted->w-bottomleft.w-bottomright.w;
    done=0;
   
    while(want>0)
    {
        if(want>bottom.w) //take all from pices
        {
            dst.x=bottomleft.w+done;
            dst.y=wanted->h-bottom.h;
            err = SDL_BlitSurface(pices, &bottom, bg, &dst);
            DASSERT(err==0);
	     
            done+=bottom.w;
            want-=bottom.w;
        }
        else //take what is neeeded
        {
            dst.x=bottomleft.w+done;
            dst.y=wanted->h-bottom.h;
            SDL_Rect take=bottom;
            take.w=want;
	     
            err = SDL_BlitSurface(pices, &take, bg, &dst);
            DASSERT(err==0);
	     
            done+=bottom.w;
            want-=bottom.w;
        }
    }
   
    //left
    want=wanted->h-topleft.h-bottomleft.h;
    done=0;
   
    while(want>0)
    {
        if(want>left.h) //take all from pices
        {
            dst.x=0;
            dst.y=topleft.h+done;
            err = SDL_BlitSurface(pices, &left, bg, &dst);
            DASSERT(err==0);
	     
            done+=left.h;
            want-=left.h;
        }
        else //take what is neeeded
        {
            dst.x=0;
            dst.y=topleft.h+done;
            SDL_Rect take=left;
            take.h=want;
	     
            err = SDL_BlitSurface(pices, &take, bg, &dst);
            DASSERT(err==0);
	     
            done+=left.h;
            want-=left.h;
        }
    }
   
   
    //RIGHT
    want=wanted->h-topright.h-bottomright.h;
    done=0;
   
    while(want>0)
    {
        if(want>right.h) //take all from pices
        {
            dst.x=wanted->w-right.w;
            dst.y=topright.h+done;
            err = SDL_BlitSurface(pices, &right, bg, &dst);
            DASSERT(err==0);
	     
            done+=right.h;
            want-=right.h;
        }
        else //take what is neeeded
        {
            dst.x=wanted->w-right.w;
            dst.y=topright.h+done;
            SDL_Rect take=right;
            take.h=want;
	     
            err = SDL_BlitSurface(pices, &take, bg, &dst);
            DASSERT(err==0);
	     
            done+=right.h;
            want-=right.h;
        }
    }
   
    //BOTTOM
   
   
    return bg;
}



/**********************/
/*SurfaceHoldAutoBuild*/
/**********************/

SurfaceHoldAutoBuild SurfaceHoldAutoBuild::me;

SurfaceHoldAutoBuild::SurfaceHoldAutoBuild()
{
    //build=build_name;
   
    //build.assign("");
   
    c.r=150;
    c.g=150;
    c.b=150;
}


SurfaceHoldAutoBuild& SurfaceHoldAutoBuild::instance()
{
    return me;
}

SDL_Surface* SurfaceHoldAutoBuild::operator[](const string k)
{
    return getSurface(k);
}

SDL_Surface* SurfaceHoldAutoBuild::getBG(uint w, uint h)
{
    SDL_Rect size;
    size.w=w;
    size.h=h;
    size.x=size.y=0;
   
    string name;
    bool ok = makeName(size, name);
    DASSERT(ok);
   
    return getSurface(name);
}


SDL_Surface* SurfaceHoldAutoBuild::getSurface(const string name, bool strict)
{
   
    SDL_Surface* ret = SurfaceHold::getSurface(name, true);
   
    if(ret==NULL) //build a new one
    {	
        SDL_Rect want;
        bool bok = makeSize(name, want);
	
        if(!bok)
        {
            cout << "bgmake, invalid arg: " << name << endl;
            if(strict)
            {
                return NULL;
            }
            return empty;
        }
	
        ret = buildBG(want);
        DASSERT(ret!=NULL);
	
        if(ret!=NULL)
        {
            bool ok = addSurface(name, ret, true);
            DASSERT(ok);
        }
    }
   
    return ret;
}

SDL_Surface* SurfaceHoldAutoBuild::buildBG(const SDL_Rect& size)
{
    SDL_Surface* ret;
      
    if(build.length()==0)
    {
        DERROR("build name not set");
        return NULL;
    }
      
    //FIXME: MOVE!
    /*c.r=150;
      c.g=150;
      c.b=150;*/
   
    SDL_Rect tl = {0,0,4,15+3};
    SDL_Rect bl = {0,28, 4,4};
    SDL_Rect br = {28,28, 4,4};
    SDL_Rect tr = {28,0,4,15+3};
    SDL_Rect bot = {4,29,24,3};
    SDL_Rect top = {4,0,24,14+3};
    SDL_Rect l = {0,15+3,3,13-3};
    SDL_Rect r = {29,15+3,3,13-3};
    //SDL_Rect back = {4,15,24,12};
   
    //end fixme
   
    VERBOSE2(cout << "building" << endl;);
   
    SDL_Surface* bpix = SurfaceHold::getSurface(build, true);
    DASSERT(bpix!=NULL);
    if(bpix==NULL)return NULL;
   
    SDL_Rect want = size;
   
    ret = assembleBG(bpix, &want,
    tl,l,bl, bot, br,r, tr, top, 
    SDL_MapRGB(bpix->format,c.r,c.g,c.b));
   
    DASSERT(ret!=NULL);
   
    return ret;
}


void SurfaceHoldAutoBuild::setBuildName(const string build_name)
{
    build=build_name;
}


void SurfaceHoldAutoBuild::setBuildColor(const SDL_Color& c)
{
    this->c=c;
}

bool SurfaceHoldAutoBuild::makeSize(const string& name, SDL_Rect& ret)
{
    ret.x=0;
    ret.y=0;
   
    istringstream ss(name);
    int w,h;
    char x;
   
    ss >> w;
    ss >> x;
    ss >> h;
   
    if(w<=0 || h<=0 || x!='x')
    {
        VERBOSE1(cout << "invalid format" << endl;);
        return false;
    }
   
    ret.h=h;
    ret.w=w;
   
    return true;
}

bool SurfaceHoldAutoBuild::makeName(const SDL_Rect& size, string& ret)
{
    ostringstream ss;
    ss << size.w << "x" << size.h;
   
    ret = ss.str();
   
    return true;
}
}