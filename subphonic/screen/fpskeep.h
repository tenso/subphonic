//FIXME: seems to miss calculate in possibleFPS()

#ifndef SPL_FPS_KEEP_H
#define SPL_FPS_KEEP_H

#include <SDL.h>
#include <iostream>

namespace spl
{

#define FPS_DONT_KEEP -1

class Fps
{
  public:
   
    //set to <0 for no keep, still count possible fps though
    Fps(int fps=60);
   
    void startFrame();
    void endFrame();
   
    void setFPSLimit(int fps);
    int getFPSLimit();
   
    double getFps(); //update once per second
    double getPossibleFps();
   
    //returns num milli seconds remaining after endFram() 
    //if this is neg or zero, prog frametime is to great for comp to handle
    double getMilliToSpare();
  private:
    int tick,tack;
    int frames;
    int fps;
      
    double show_fps;
    double show_fps_s;
    double show_fps_e;
   
    double show_fps_nokeep;
    double fps_nokeep_s;
    double fps_nokeep_e;
   
    double del;
    double wanted;
    double error;
    double fps_k; //fps*1000.0
    int ierror;
   
};

}
#endif
