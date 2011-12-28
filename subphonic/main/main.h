#ifndef MAIN_H
# define MAIN_H

# include <stdlib.h>

# include <SDL_thread.h>
# include <sys/types.h>
//#include <dirent.h>

# include <string>

# include "../screen/all.h"
//# include <audio/device.h>

# include "../m_main/m_main.h"
# include "../m_main/m_bench.h"
# include "../m_main/progstate.h"

#include "../util/debug.h"
# include "../util/o_gcon.h"
# include "../gui/bg_make.h"

# include "../util/util.h"

#include "../audio/midi.h"
#include "../screen/fpskeep.h"

//ALL return 0 if no error, -1 if error, 1 if not an error, but should quit(want show help for parseArgs)

int parseArgs(int argc, char** argv);

//-1 on error
int loadPixmaps(const std::string prefix);
//int loadFonts();
void quit();
int initialCmds();

int initCon();
int freeCon();

//debug 
class Stats
{
  public:
    Stats()
    {
        count=0;
        num=50;
        dnum=num;
        c_ev=0;
        c_draw=0;
        c_sleep=0;
        c_gen=0;
        c_play=0;
        ev=0;
        draw=0;
        sleep=0;
        gen=0;
        play=0;
        total=0;
        load=0;
    }
   
    void tick()
    {
#ifdef UNIX_TIME
        gettimeofday(&ta_tim, NULL);
        ta_usecs= ta_tim.tv_sec*1000000 + ta_tim.tv_usec;
#endif
    }
   
    enum STAT {EVENT, GEN, SLEEP, DRAW, PLAY};
	
   
    void tack(STAT stat)
    {
#ifdef UNIX_TIME
        gettimeofday(&ta_tim, NULL);
        ta_usecs=ta_tim.tv_sec*1000000+ta_tim.tv_usec - ta_usecs;
	
        if(stat==EVENT)
        {
            c_ev+=ta_usecs;
        }
        else if(stat==DRAW)
        {
            c_draw+=ta_usecs;
        }
        else if(stat==GEN)
        {
            c_gen+=ta_usecs;
        }
        /*else if(stat==SLEEP)
          {
          c_sleep+=ta_usecs;
          }*/
        else if(stat==PLAY)
        {
            c_play+=ta_usecs;
        }
#endif
    }
   
    void tSleep(double sleep)
    {
        c_sleep+=sleep;
    }
   
   
    bool accum()
    {
        count++;
        if(count==num)
        {
            count=0;
            ev=c_ev/dnum;
            draw=c_draw/dnum;
            gen=c_gen/dnum;
            sleep=c_sleep/dnum;
            play=c_play/dnum;
	     
            total = sleep+gen+draw+ev+play;
            if(total!=0)load = 1.0-sleep/total;
	     
            c_ev=0;
            c_draw=0;
            c_gen=0;
            c_sleep=0;
            c_play=0;
	     
            return true;
        }
        return false;
    }
   
   
    double c_ev;
    double c_draw;
    double c_gen;
    double c_sleep;
    double c_play;
    double ev;
    double draw;
    double gen;
    double sleep;
    double play;
    double load;
    double total;
   
    int count;
    int num;
    double dnum;
   
#ifdef UNIX_TIME
    double ta_usecs;
    timeval ta_tim;
#endif
};

class StatsSum
{
  public:
    StatsSum() : stats(1)
    {
	
    }
   
    void update(uint th_id, const Stats& s)
    {
        if(th_id>=stats.size())stats.resize(th_id+1);
        DASSERT(th_id<stats.size());
	
        upd=true;
        stats[th_id]=s;
    }
   
    bool updatedSum()
    {
        return upd;
    }
   
   
    Stats getSum()
    {
        Stats ret;
        for(uint i=0; i<stats.size();i++)
        {
            ret.ev+=stats[i].ev;
            ret.draw+=stats[i].draw;
            ret.gen+=stats[i].gen;
            ret.sleep+=stats[i].sleep;
            ret.play+=stats[i].play;
	     
            ret.total =ret.sleep+ret.gen+ret.draw+ret.ev+ret.play;
            if(ret.total!=0)ret.load = 1.0-ret.sleep/ret.total;
        }
        upd=false;
        return ret;
    }
   
    Stats getSingle(uint th_id)
    {
        return stats[th_id];
    }
   
   
  private:
    bool upd;
    vector<Stats> stats;
};


/************/
/*WORKTHREAD*/
/************/

//i.e a workspace

//needs access to globals: shared_mess, shared_audio, master_res
//THREADS CAN only have one lock at a time! i.e either master_share_mut OR shared_work_mut
class WorkThread
{
  public:
    static const uint CHECK_MS_INTERVAL=1; //MIN:1 in frames
    static const uint IDLE_TIME_USEC=1; //micro sec, for idles between all different kinds of events
   
    WorkThread();
    ~WorkThread();
   
    void init(Screen* screen, BitmapFont* f, Console* c);
    void free();
   
    //only returns on SharedMessagePass::shutdown
    //int exec(void* data); //execs' one step, will lock both SharedMessagePass, and in turn SharedAudio
   
    //if running as thread
    void startThread();
   
    //FIXME: to private(somehow)
   
    void takeMaster();
    void yieldMaster();
    SDL_Thread* th;
   
    int generate();
    void sync(uint to, bool set_state=false, int state_to=0);
   
    uint th_id;
    static uint num_th;
   
    spl::Fps fps;
    spl::MIDI midi;
    Main* maincomp;
   
    int sync_count;
    bool is_master; //i.e this th should draw, handle input etc
   
    //bool frozen;
   
    Stats stats;
};

int w_th_exec(void* data);




#endif
