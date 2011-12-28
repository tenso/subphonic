#include "surf_hold.h"

#include <SDL_image.h>
#include "../util/debug.h"

namespace spl {

SurfaceHold SurfaceHold::me;

SurfaceHold& SurfaceHold::instance()
{
    return me;
}

int SurfaceHold::getLoadedBytes() const
{
    return loaded_bytes;
}


SurfaceHold::SurfaceHold()
{
    loaded_bytes=0;
   
    setMaskColor(0xff,0xff,0xff);
   
/*   #if SDL_BYTEORDER == SDL_BIG_ENDIAN
     rmask = 0xff000000;
     gmask = 0x00ff0000;
     bmask = 0x0000ff00;
     amask = 0x000000ff;
     #else
     rmask = 0x000000ff;
     gmask = 0x0000ff00;
     bmask = 0x00ff0000;
     amask = 0xff000000;
     #endif*/
   
    empty = SDL_CreateRGBSurface(SDL_SWSURFACE, 10, 10, 16,
    0,0,0,0);
}

SurfaceHold::~SurfaceHold()
{
    SDL_FreeSurface(empty);
    clear();
}

SDL_Surface* SurfaceHold::getSurface(const string name, bool strict)
{
    Data::iterator it = surf.find(name);
   
    if(it==surf.end())
    {
        if(strict)
        {
            return NULL;
        }
        else return empty;
    }
   
    return it->second;
}

SDL_Surface* SurfaceHold::operator[](const string k)
{
    return getSurface(k);
}


bool SurfaceHold::loadSurface(const string filename, bool set_mask)
{
    Data::iterator it = surf.find(filename);
    if(it!=surf.end())
    {
        DERROR("surface with same name already exist");
        return false;
    }
         
    //SDL_Surface* s = SDL_LoadBMP(filename.c_str());
    SDL_Surface* s = IMG_Load(filename.c_str());
    if(s==NULL)
    {
        DERROR("file load failed");
        cout << filename << endl;
        return false;
    }
   
    /*SDL_Surface* tmp = SDL_DisplayFormat(s);
    SDL_FreeSurface(s);
    s = tmp;*/

    loaded_bytes+=s->pitch*s->h;
   
    if(set_mask)setMaskSurface(s);
   
    surf.insert(make_pair(filename, s));
   
    return true;
}


bool SurfaceHold::loadSurface(const string filename, const string key, bool set_mask)
{
    Data::iterator it = surf.find(filename);
    if(it!=surf.end())
    {
        DERROR("surface with same name already exist");
        return false;
    }
         
    //SDL_Surface* s = SDL_LoadBMP(filename.c_str());
    SDL_Surface* s = IMG_Load(filename.c_str());
    if(s==NULL)
    {
        DERROR("img load failed");
        cout << filename << endl;
        return false;
    }
    
    /*SDL_Surface* tmp = SDL_DisplayFormat(s);
    SDL_FreeSurface(s);
    s = tmp;*/

    loaded_bytes+=s->pitch*s->h;
   
    if(set_mask)setMaskSurface(s);
   
    surf.insert(make_pair(key, s));
   
    return true;
}


bool SurfaceHold::addSurface(const string name, SDL_Surface* s,bool set_mask)
{
    Data::iterator it = surf.find(name);
    if(it!=surf.end())
    {
        DERROR("surface with same name already exist");
        return false;
    }
   
    loaded_bytes+=s->pitch*s->h;
   
    if(set_mask)setMaskSurface(s);
    surf.insert(make_pair(name, s));
   
    return true;
}


void SurfaceHold::setMaskColor(Uint8 r, Uint8 g, Uint8 b)
{
    mask[0]=r;
    mask[1]=g;
    mask[2]=b;
}


void SurfaceHold::clear()
{
    Data::iterator it = surf.begin();
    while(it!=surf.end())
    {
        SDL_FreeSurface(it->second);
        it++;
    }
    surf.clear();
}


bool SurfaceHold::setMaskFor(const string name)
{
    Data::iterator it = surf.find(name);
    if(it==surf.end())
    {
        DERROR("surface does not exist");
        return false;
    }
   
    setMaskSurface(it->second);
    return true;
}


void SurfaceHold::setMaskSurface(SDL_Surface* s)
{
    SDL_SetColorKey(s, SDL_SRCCOLORKEY, SDL_MapRGB(s->format,mask[0],mask[1],mask[2]));
}

}