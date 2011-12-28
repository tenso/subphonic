#ifndef BG_MAKE_H
#define BG_MAKE_H

#include <SDL.h>

#include "../screen/surf_hold.h"
#include "../util/debug.h"
#include "../util/types.h"

namespace spl {

SDL_Surface* assembleBG(SDL_Surface* pices, SDL_Rect* wanted,
SDL_Rect& topleft,
SDL_Rect& left,
SDL_Rect& bottomleft,
SDL_Rect& bottom,
SDL_Rect& bottomright,
SDL_Rect& right,
SDL_Rect& topright,
SDL_Rect& top, Uint32 bg_color);


//if pixmap is not found and key string is of format YxY it will be built and returned
//if build_name does not exist in Hold nothing will be built
//
class SurfaceHoldAutoBuild : public SurfaceHold
{
  public:
   
    static SurfaceHoldAutoBuild& instance();
   
    void setBuildName(const string build_name);
   
    SDL_Surface* operator[](const string k);
    SDL_Surface* getSurface(const string name, bool strict=false);
    SDL_Surface* getBG(uint w, uint h); //same as ["WxH"]
     
    //default:150
    void setBuildColor(const SDL_Color& c);
      
  private:
   
    static SurfaceHoldAutoBuild me;
   
    SurfaceHoldAutoBuild(const SurfaceHoldAutoBuild&);
    SurfaceHoldAutoBuild& operator=(const SurfaceHoldAutoBuild&);
      
    SurfaceHoldAutoBuild();
   
    bool makeSize(const string& name, SDL_Rect& ret);
    bool makeName(const SDL_Rect& size, string& ret);
   
    //make sure surface does not exist before this!
    //
    //ret = SurfaceHold::getSurface(name, true);
    //DASSERT(ret==NULL);
   
    SDL_Surface* buildBG(const SDL_Rect& size);
   
    string build;
    SDL_Color c;
};
}
#endif
