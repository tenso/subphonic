#include "fpskeep.h"

namespace spl
{

using namespace std;

Fps::Fps(int fps)
{
    frames=1;
    this->fps=fps;
    fps_k=fps*1000.0;
   
    show_fps=0;
    show_fps_nokeep=0;
    wanted = 1000.0/(double)fps;
    error=0;
    ierror=0;
   
    fps_nokeep_e=0;
    fps_nokeep_s = SDL_GetTicks();
   
    fps_nokeep_e=0;
    show_fps_s=SDL_GetTicks();
}


void Fps::startFrame()
{
    tick=SDL_GetTicks();
}

void Fps::setFPSLimit(int fps)
{
    this->fps=fps;
    fps_k=fps*1000.0;
   
    wanted = 1000.0/(double)fps;
}

int Fps::getFPSLimit()
{
    return fps;
}

double Fps::getFps()
{
    return show_fps;
}

double Fps::getPossibleFps()
{
    return show_fps_nokeep;
}

//FIXME: this should be re-written
void Fps::endFrame()
{
    if(frames%fps==0)//calculate actual fps, about once per second
    {
        show_fps_e = SDL_GetTicks();
        show_fps = fps_k/(show_fps_e-show_fps_s);
        show_fps_s = SDL_GetTicks();
	
        fps_nokeep_e = SDL_GetTicks();
        show_fps_nokeep=fps_k/(fps_nokeep_e-fps_nokeep_s); //done every 50fps
        fps_nokeep_s = SDL_GetTicks(); //for next
	
    }
   
    frames++;
   
    tack = SDL_GetTicks();
   
   
    if(fps>0)
    {
        //take integral part of error and add it to delay, let fractional part remain untill next
        //but only in granularity of 10: SDL(10ms)
        ierror=(int)(error/10)*10;
        error-=ierror;
	
        //wanted delay
        del=(1000-(tack-tick)*fps)/(double)fps + ierror;
	
        //if delay is neg it means things are going to slow, atleast no delay needed...
        if(del<0)
        {
            del=0;
            error=0;
        }
        else
        {
            int idel=(int)(del/10)*10; //granularity of SDL_Delay is 10ms, so remove first digit in delay
	     
            if(idel>0)SDL_Delay(idel);
        }
	
    }
      
   
    Uint32 tack2 = SDL_GetTicks();
    Uint32 real_delay= tack2 - tack;
   
    fps_nokeep_s+=real_delay;
   
    //don't add to error if there is no delay
    if(fps>0 && del>0)
        error+= (del - real_delay);
   
}

double Fps::getMilliToSpare()
{
    return del;
}

}
