#ifndef SURF_HOLD_H
#define SURF_HOLD_H

#include <map>
#include <string>
#include <SDL.h>

namespace spl {

using namespace std;
//singleton

//make SurfaceHold take over ALL surfaces added
class SurfaceHold
{
  public:
   
    static SurfaceHold& instance();
   
    SDL_Surface* operator[](const string k);
      
    //if strict it will return NULL on not found, otherwise an usable surface(anything) will be returned
    SDL_Surface* getSurface(const string name, bool strict=false);

    //return true with success
    bool loadSurface(const string filename, bool set_mask=false);
    bool loadSurface(const string filename, const string key, bool set_mask=false);
    bool addSurface(const string name, SDL_Surface* s,bool set_mask=false);
   
    //mask used when load/addSurface, surfaces not changed if this is reset after load
    void setMaskColor(Uint8 r, Uint8 g, Uint8 b);
   
    void clear(); //deletes all content so dont use pointer after that!
    
    bool setMaskFor(const string name);
   
    int getLoadedBytes() const;
   
  protected:
   
    typedef map<string, SDL_Surface*> Data;
   
    static SurfaceHold me;
   
    SurfaceHold();
    ~SurfaceHold();
   
    SurfaceHold& operator=(const SurfaceHold&);
    SurfaceHold(const SurfaceHold&);
   
    void setMaskSurface(SDL_Surface* s);
   
    Uint8 mask[3];
    Data surf;
    SDL_Surface* empty;
    int loaded_bytes;
};

}

#endif
