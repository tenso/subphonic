#ifndef MAIN_TH_H
#define MAIN_TH_H

#include <queue>
#include "../util/types.h"

#include "main.h"

#include "../audio/device.h"
#include "../screen/input.h"

//MASTER THREAD RESOURCES

//WARNING: no free functions are ever called in destructors

//all in here are used by one thread at a time: i.e no lock needed
//used by master thread, this makes master thread only one able to switch threads
class MasterResourses
{
  public:
    MasterResourses();
    ~MasterResourses();
   
    //returns 0 on sucess, -1 on fail
   
    int initInput();
    int freeInput();
   
    int initVideo(uint screen_w, uint screen_h, bool useGL = false);
    int freeVideo();
    //int initCon(); //FIXME: these belong here
    //int freeCon();
    int loadFonts();
    int freeFonts();
   
    void sdlInput();

    //beacuse console is in delayedExec it is ok to exec console commands here
    void handleInput();
   
    void draw();
   
    //meassages
    Main* maincomp; //WARNING: this points to current masters Main*, one in main.o MUST correspond(used by cmds.h)
    bool do_shutdown;
    bool change_master;
    uint next_master_th;
    bool do_sync;
    bool do_state_sync; //only set with do_sync!
   
    bool do_toggle_fullscreen;

    bool playon; //FIXME: implement
   
    //current th that have master, unless 'master_free'==true
    uint master_th_id;
   
    //resources
    spl::Input in;
    bool dodraw;
    Console* con;
    Screen* screen;
   
    //HALF OK?
    //BitmapFont* f;
    //BitmapFont* f1;
    //BitmapFont* fb;
   
    static ProgramState& progstate;
    static SurfaceHoldAutoBuild& pix;
    static ResHold<BitmapFont>& fnt;
   
    //"debug"
   
    void setStat(uint th_id, const Stats& s)
    {
        if(th_id>=stats.size())stats.resize(th_id+1);
        DASSERT(th_id<stats.size());
	
        stats[th_id]=s;
    }
   
    void setStatDrawTh(const Stats& s)
    {
        stats_draw=s;
    }
   
    void setStatSum(const Stats& s)
    {
        stats_all=s;
    }
   
   
    Stats stats_all;
    Stats stats_draw;
    vector<Stats> stats;
   
    //c_ is calc(gen+ev) d_ is draw
    double c_fps;
    double d_fps;
   
    bool change_d_fps;
    double d_run_fps;
};


//master:
//AFFECTS: draw, input, font, console, cmds
//MUST NOT AFFECT: adev (unless also last to upload)

//slaves: affect: NOTHING except own data + adev(if last uploader)


//lock this before every frame to check who is master
//maybe every N frames is enough

class SharedMessagePass
{
  public:
    SharedMessagePass();
    ~SharedMessagePass();
   
    /*
      void build(uint num_th);
      void free();*/
   
    bool change_master; //if this is true 'next_master_th_id' should grab master and set master_free=false
    uint next_master_th_id; //this thread should grab master when it locks BUT ONLY when master_free
   
    //bool idle; //if this is true, all threads should do just that, and set is_idle(i.e no read/write op while IDLE)
    //vector<bool> is_idle; //all vector<>(id) corresponds with th_id in WorkThread
   
    //if this differs form WorkThread::sync_count : discard all samples currently in maincomp::queue
    //do maincomp->sync, increase sync_count, continue as usuall
    int sync_to;
    int sync_state_to;
    bool sync_state; //if this is true, when doing a sync: also set state to 'sync_state_to'
   
    bool shutdown;
   
    //uint num_th;
   
    //cmds.h: changes progstate, must "lock":
    //make console just parse cmd to string not execute, instead return cmd for later use
    //tell all threads to freeze, idle until num_frozen==num_th-1, execute command unfreeze
    //then when command is actually executed no threads are running anything, so ok to execute
    //when threads are frozen they just idle(locking shared mess between, i.e ok to change global/thread data)
    bool freeze_threads;
    uint num_frozen;//when this is num_th-1 it is ok for state change(from master)
   
    StatsSum stats_sum;
   
    //this is used for the varius maincomps that not is master to
    //exec commands in master th
    std::queue<std::string> maincomp_cmd_queue;
   
};



//sync happens like so: when one th(master) wants to sync, sync_count++ is WorkThread
//and upload as usuall. SharedAudio will discover that sync count is different for the batches
//this causes it not mix but instead discard those batches with wrong sync_count i.e < master::sync_count
//then is should just continue as usuall. after threads sync, one after another, eventually all will
//have uploaded batches with correct sync_count.


//falls under shared_work_mut:

class SharedAudio
{
  public:
    SharedAudio();
    ~SharedAudio();
   
    //return 0 on sucess, -1 on fail, 1 on error but continue
   
    //uses progstate for params
    //NOTICE: updates progstate with obtained format
    int initAudioDev(); //SDL init before!
    void freeAudioDev();
   
    //buffer
    void build(uint num_th, uint genbuff, uint genlen);
    void free();
   
    bool shouldUpload(uint th_id, int sync_count);//if not, unlock, idle, continue thread
    void upload(Main* mc, uint th_id, int sync_count);//uploads the correct number of samples, i.e drains mc queue
   
    bool shouldMix(uint th_id);//if this returns true it is up to current lock-holder to call mixAndPlay()
   
    //handles audiodev mix+dump/render
    //play_adev:
    //true == sound is sent to audio dev, mix rate is decided by how much adev wants
    //false == process as much as possible(good for dump without play)
    //removes played from SharedAudio::queue
    void mix(bool play_adev);
   
    //needed at sync
    void resetFor(uint th_id);
   
  private:

    spl::SDLAudioDev adev;      
    const Sample* mixNext();
    Sample* mix_smp;
   
    //spl::SDLAudioDev adev;
   
    class WorkData
    {
      public:
        WorkData();
        ~WorkData();
	
        void build(uint genbuff, uint genlen);
        void free();
	
        void push(const Sample* smp);
        const Sample* pop();
        bool queueFull();
	
        void resetQueue();
	
        int sync_count;
        uint from_th;
	
        uint q_read;
        uint queue_max_size;
        queue<Sample*> smp_queue; //point into smp_data
        Sample** smp_data; //make sure this is same size as Maincomp::makeSampleQueue
	
        bool have_uploaded;
	
        //debug:
        uint d_num_pushed;
        uint d_num_popped;
    };
   
    uint num_uploaded;
    vector<WorkData*> uploaded;
    bool first_in_batch;
    int max_sync;
    static ProgramState& progstate;
};

#endif
