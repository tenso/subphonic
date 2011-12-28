#include "../main/config.h"

#ifdef BUILD_WINDOWS
#pragma comment( lib, "SDL.lib" )
#pragma comment( lib, "SDLmain.lib" )
#pragma comment( lib, "SDL_image.lib" )
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glew32.lib" )

#include <direct.h>

#ifndef CONSOLE_APP
#include <windows.h>
#include <windowsx.h>
#endif

#endif

#include "main.h" 
#include "main_th.h"
#include "../m_main/progstate.h"

using namespace std;
using namespace spl;

//not to important:
static const int INITIAL_FPS=30;


//WOWKFIX: SoundComp::st_seq is static(th) (good or bad?)
//m_main changes progstate
//not reset on syncAll: filter, delay, effects

/***********/
/*MAIN DATA*/
/***********/


//STARTUP DATA:
//FIXME: move to progstate!
uint num_threads=2; //cant change after init
const uint max_threads=9;

int screen_w=1280;
int screen_h=1024;
vector<string> initial_load(1); //one per workspace
string autoexec_path;
string run_after_load;
bool useGl = false;

//ALL SHARE:(FIXME)

//data integrity(these must be read only):
// progstate: NOT OK to change, every thread reads this:
//  POS1: make a idle command that puts all threads(except the one changeing progstate) to idle, wait for that, then update progstate, startup all again
//  POS2: fix progstate at program init, make read only
//  POS3: no global progstate, every thread has one that get updated by lock
// 
// bitmapfonts : OK - read only: never changes after init: however make this explicit by making it const in GUI and g_comp
// pix: OK - only used at init(before threads and then in g_comp constructors(only availible from master)
// 
ProgramState& progstate=ProgramState::instance();             //NOT OK
SurfaceHoldAutoBuild& pix = SurfaceHoldAutoBuild::instance(); //OK


//no mutex needed:

//2xMUST correspond to MasrerResources::maincomp
Main* maincomp=NULL;
Console* con=NULL;
ResHold<BitmapFont>& fnt = ResHold<BitmapFont>::instance();

//BitmapFont* f=NULL;
Screen* screen=NULL;

MasterResourses master_res;
SDL_mutex* master_res_mut=NULL;
bool lockMasterRes();
bool unlockMasterRes();

//draw is separated from generate+events for one reason:
//draw fps might be different from gen+ev fps
SDL_Thread* draw_th=NULL;
int draw_th_fun(void* data);


SharedMessagePass shared_mess;
SDL_mutex* shared_mess_mut;
bool lockSharedMess();
bool unlockSharedMess();

SharedAudio shared_audio;

SDL_mutex* shared_audio_mut=NULL; //lock this when done generating, upload, if last to upload mix and play
bool lockSharedAudio();
bool unlockSharedAudio();


void initAllShared();
void freeAllShared();

WorkThread* cmd_caller_th=NULL; //set so cmd.h knows 
vector<WorkThread*> w_threads; //all threads created at startup now: otherwise change

void waitAllThreads(); //main() waits for all to close
//void freeAll();


//Stats stats;

/*THIS IS OK TO CHANGE IN CMDS:
  all other w_thread[] are frozen so: anything 
*/ 

#include "cmds.h"

/******/
/*MAIN*/
/******/

#include "../util/res_hold.h"

#ifndef CONSOLE_APP
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv) 
#endif
{
   
    /***************/
    /*INITIALIZAION*/
    /***************/
   
    //WARNING: ORDER IMPORTANT!
   
    //set defaults
    progstate.setDevNSmp(2048);
    progstate.setSampleGenBuffer(4); //FIXME
    progstate.setSampleGenLen(2048);
   
    progstate.setNChan(1);
    progstate.setSampleRate(44100);
   
    master_res.d_run_fps=INITIAL_FPS;
    progstate.setFps(INITIAL_FPS);
   
    progstate.setGraphColor(255,0,0);
    
#ifdef DATA_PREFIX
    progstate.setDataPrefix(DATA_PREFIX);
#else
    char* buffer = get_current_dir_name();
    progstate.setDataPrefix(string(buffer) + "/data/");
    free(buffer);
#endif

    autoexec_path = progstate.getDataPrefix() + "autoexec.cfg";
   
    //parse args, FIXME: this makes autoexec override command line!
#ifndef CONSOLE_APP
    int argc = 0;
    char** argv = 0;
#endif
    int arg_ret=parseArgs(argc, argv);
   
    if(arg_ret==-1)return 1; //some error
    if(arg_ret==1)return 0; //showed help
   
   
    //set exit function
    int ex_err = atexit(quit);
    if(ex_err != 0)
    {
        cout << "atext() failed" << endl;
        return 1;
    }
   
   
    //SDL
    int err = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK); /*SDL_INIT_EVERYTHING*/
   
    if(err==-1)
    {
        cout << "SDL init failed" << endl;	
        cout << SDL_GetError() << endl;
        return 1;
    }
   
    //if(progstate.getGrabInput())SDL_WM_GrabInput(SDL_GRAB_ON);
   
    initAllShared();
   
    //WORKFIX: remove(for cmd now)
    /*int ret =*/
    
    err = master_res.initInput();
    if(err==-1)
    {
        cout << "init input failed" << endl;
        cout << SDL_GetError() << endl;
        exit(1);
    } 
   


    err = master_res.initVideo(screen_w, screen_h, useGl);
    if(err==-1)
    {
        cout << "init video failed" << endl;
        cout << SDL_GetError() << endl;
        exit(1);
    } 
  
    /*while(1)
      {
      glClear(GL_COLOR_BUFFER_BIT);

      glBegin(GL_QUADS);
    
      glColor3f(1,1,0);
      glVertex2i(0,0);
      //glTexCoord2f(0,0);

      glColor3f(1,0,0);
      glVertex2i(1024, 0);

      //glTexCoord2f(1,0);
    
      glColor3f(1,0,0);
      glVertex2i(1024, 768);

      //glTexCoord2f(1,1);
    
      glColor3f(1,0,0);
      glVertex2i(0, 768);

      //glTexCoord2f(1,0);

      glEnd();

      SDL_GL_SwapBuffers();
      }*/
    
    string bmdata = progstate.getDataPrefix() + "bitmaps/";
    cout << bmdata << endl;

    err = loadPixmaps( bmdata ); 
    if(err==-1)
    {
        cout << "load pixmaps failed" << endl;
        exit(1);
    }
   
    SDL_WM_SetCaption("multiverk",NULL);
    SDL_WM_SetIcon(pix["icon"], NULL);

    err = master_res.loadFonts();
    if(err==-1)
    {
        cout << "load fonts failed" << endl;
        exit(1);
    }
   
    initCon(); //FIXME: old:"after first takerMaster" still valid? hope not
   

    for(uint i=0;i<num_threads;i++)
    {
        WorkThread* th = new WorkThread(); //must have pointers: need to control destructor
        th->init(master_res.screen, fnt["std"]/*master_res.f*/, master_res.con);
        w_threads.push_back(th);
    }
    WorkThread* th_by_main = w_threads[0]; //thread run by main()
    //cmd_caller_th=th_by_main; //initial caller
   
    th_by_main->takeMaster();
   
    err = shared_audio.initAudioDev();
    if(err==-1)
    {
        cout << "audio dev failed" << endl;
        cout << SDL_GetError() << endl;
        exit(1);
    }
    if(err==2)
    {
        cout << "audio dev: settings not accepted(continue)" << endl;
    }
   
    shared_audio.build(w_threads.size(), progstate.getSampleGenBuffer(), progstate.getSampleGenLen()); 
   
    //FIXM: (used by cmd now)
    con = master_res.con;
    //f = master_res.f;
    screen = master_res.screen;
   
    //must be last init, and before threads start!
    initialCmds();
   
    //entering queued exec
    master_res.con->setDirectExec(false);
   
    cout << "init ok [" << progstate.getSampleRate() << " hz, " << progstate.getNChan() << " chan]" << endl;
   
    //START THREADS
   
    //ok to start draw, master_res init finished
    //draw_th = SDL_CreateThread(draw_th_fun, NULL);
   
   
    for(uint i=1;i<num_threads;i++)
    {
        w_threads[i]->startThread();
    }
   
    w_th_exec(th_by_main);
   
   
    //SHUTDOWN
   
    waitAllThreads();
   
    freeAllShared();
   
    for(uint i=0;i<num_threads;i++)
    {
        w_threads[i]->free();
        delete w_threads[i];
    }
   
   
   
    freeCon();
    master_res.freeFonts();
    master_res.freeVideo();
    master_res.freeInput();
    shared_audio.freeAudioDev();
    shared_audio.free();
   
    //cout << SDL_GetError() << endl;
   
    return 0; 
}

void waitAllThreads()
{
    //wait for thread
    for(uint i=0;i<w_threads.size();i++)
        SDL_WaitThread(w_threads[i]->th, NULL);
   
    //SDL_WaitThread(draw_th, NULL);
}


bool lockMasterRes()
{
    if(SDL_mutexP(master_res_mut)==-1)
    {
        DERROR("unable to lock mutex");
    }
   
    return true;
}

bool unlockMasterRes()
{
   
    if(SDL_mutexV(master_res_mut)==-1)
    {
        DERROR("unable to unlock mutex");
    }
    return true;
}


bool lockSharedAudio()
{
    if(SDL_mutexP(shared_audio_mut)==-1)
    {
        DERROR("unable to lock mutex");
    }
   
    return true;
}

bool unlockSharedAudio()
{
    if(SDL_mutexV(shared_audio_mut)==-1)
    {
        DERROR("unable to unlock mutex");
    }
    return true;
}

bool lockSharedMess()
{
    if(SDL_mutexP(shared_mess_mut)==-1)
    {
        DERROR("unable to lock mutex");
    }
    return true;
}

bool unlockSharedMess()
{
    if(SDL_mutexV(shared_mess_mut)==-1)
    {
        DERROR("unable to unlock mutex");
    }
    return true;
}

void initAllShared()
{
    master_res_mut = SDL_CreateMutex();
    shared_audio_mut = SDL_CreateMutex();
    shared_mess_mut = SDL_CreateMutex();
}

void freeAllShared()
{
    SDL_DestroyMutex(master_res_mut);
    SDL_DestroyMutex(shared_audio_mut);
    SDL_DestroyMutex(shared_mess_mut);
}


//WORKFIX: redo + move this

#include "cmds.h" 

int initCon() 
{
    //con = new Console("files/conback.bmp", f, f1, s);
    master_res.con = new Console(master_res.screen->W(), master_res.screen->H(), fnt["con_in"], fnt["con_out"], (Screen*)master_res.screen);
   
   
    //MUST DO THIS AFTER INITIAL LOAD
    //master_res.con->setDirectExec(false);
   
    master_res.con->setMonogram("-=KW v" + string(VERSION) + "=-");
    master_res.con->setBgColor(0,0,0);
   
    //master_res.con->setAlpha(255);
    master_res.con->setDownShow(0.75);
    master_res.con->setScrollSpeed(120);
   
    master_res.con->setShowLineNum(false); 
    master_res.con->setShowLastHist(true, 2,  /*show_in*/false, /*show_out*/true);
   
    //master_res.con->addCmd("help", help);
    master_res.con->addCmd("help_keys", help_keys);
    master_res.con->addCmd("help_cmds", help_cmds); 
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("ls", ls );
    master_res.con->addCmd("prefix", prefix );
    master_res.con->addCmd("prefix_startdir", prefix_startdir );
    master_res.con->addCmd("prefix_all", prefix_all);
    master_res.con->addCmd("prefix_startdir_all", prefix_startdir_all );
    master_res.con->addCmd("cd", cd );
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("load", load);
    master_res.con->addCmd("rload", rload); 
    master_res.con->addCmd("save", save);
    master_res.con->addCmd("reset",reset);
    master_res.con->addCmd("reset_all",reset_all);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("lsmod", lsmod );
    master_res.con->addCmd("modulepath",modulepath);
    master_res.con->addCmd("modulepath_s",modulepath_sing);
    master_res.con->addCmd("loadmodule", loadmodule);
    master_res.con->addCmd("loadmodulepoly", loadmodulepoly);
    master_res.con->addCmd("templatepath", templatepath);

    master_res.con->addCmd("smppath", smplp);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("process" , process);
    master_res.con->addCmd("playondev" , playondev);
    master_res.con->addCmd("sync" , sync);
    master_res.con->addCmd("sync_comps" , sync_comps);
    master_res.con->addCmd("sync_states" , sync_states);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("mb_label" , mb_label);
    master_res.con->addCmd("mb_label_rem" , mb_label_rem);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("state_saveall" , st_saveall);
    master_res.con->addCmd("state_tickall", state_tickall);
    master_res.con->addCmd("state_set_queue", set_state_queue);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("dumpfile", dumpfile );
    master_res.con->addCmd("dumpsmp", dumpsmp );
    master_res.con->addCmd("dumpsec", dumpsec);
    master_res.con->addCmd("dumpforce", dumpforce );
    master_res.con->addCmd("dumpabort", dumpabort );
    master_res.con->addCmd("render", render);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("quit", quit);
    master_res.con->addCmd("exec", exec );
    master_res.con->addCmd("rootexec", rootexec ); 
    master_res.con->addCmd("echo", echo);
    master_res.con->addCmd("echocon", echocon);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("sndinfo", sndinfo );
    master_res.con->addCmd("gen_len", gen_len);
    master_res.con->addCmd("gen_buff", gen_buff);
    master_res.con->addCmd("dev_nsmp", dev_nsmp);
    master_res.con->addCmd("dev_nchan", n_chan);
    master_res.con->addCmd("runfps", runfps );
    master_res.con->addCmd("drawfps", drawfps );
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("ws_bound", ws_bound);
    master_res.con->addCmd("line_fg", line_fg);
    master_res.con->addCmd("showfps", showfps );
    //master_res.con->addCmd("showdumpsec" , showdumpsec);
    master_res.con->addCmd("showstats" , showstats); 
    master_res.con->addCmd("showhavestate" , showhavestate);
    master_res.con->addCmd("fullscreen",fullscreen);
    master_res.con->addCmd("grab_in", grab_in);
    //master_res.con->addCmd("con_alpha", con_alpha );
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("draw_line_tension", draw_line_tension); 
    master_res.con->addCmd("draw_line_settle", draw_line_settle);
    master_res.con->addCmd("draw_line_fancy", draw_line_fancy);
    master_res.con->addCmd("draw_line_thickness", line_thickness);
    master_res.con->addCmd("draw_color_lin" , set_lin_color);
    master_res.con->addCmd("draw_color_bg" , set_bg_color); 
    master_res.con->addCmd("draw_color_grid" , set_grid_color);
    master_res.con->addCmd("draw_grid_space" , set_grid_space);
    master_res.con->addCmd("draw_color_graph", set_graph_color);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("lin_red" , red);
    master_res.con->addCmd("lin_green" , green);
    master_res.con->addCmd("lin_yellow" , yellow);
    master_res.con->addCmd("lin_blue" , blue);
    master_res.con->addCmd("lin_cyan" , cyan);
    master_res.con->addCmd("lin_magenta" , magenta);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("", NULL);
    master_res.con->addCmd("reset_allseqno",reset_allseqno);
    master_res.con->addCmd("set_forceload", set_force_load);
    //master_res.con->addCmd("autosave", autosave);
    //done add cmd
   
    master_res.con->addCmd("load_abs", load_abs);
   
    //master_res.con->show("*channels: %d",progstate.getNChan());
    //master_res.con->show(" ");
    master_res.con->show("help for program help");
    master_res.con->show(" ");
    master_res.con->show(" "); 
   
    return 0;
}

int freeCon()
{
    delete master_res.con;
    master_res.con=NULL;
   
    return 0;
}




/********************/
/****MAIN HELPERS****/
/********************/

//FIXME: better parse
int parseArgs(int argc, char** argv)
{
    bool aexec_set=false;
    bool show_help=false;
   
    for(int i=1;i<argc;i++)
    {
        /*if(strcmp(argv[i],"--chan")==0)
          {
          if(i==argc-1)
          {
          cout << "argument error, try: " << argv[0] << " -h" << endl;
          return -1;
          }
          if(argv[i+1][0] == '-')
          {
          cout << "argument error, try: " << argv[0] << " -h" << endl;
          return -1;
          }
          * 
          i++;
          istringstream is(argv[i]);
          * 
          uint chan;
          is >> chan;
          * 
          progstate.setNChan(chan);
          * 
          VERBOSE0(cout << "num chan: " << chan << endl;);
          }
          else*/
	
        if(strcmp(argv[i],"--nws")==0)
        {
            if(i==argc-1)
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
            if(argv[i+1][0] == '-')
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
	     
            i++;
            istringstream is(argv[i]);
	     
            is >> num_threads;
	     
            if(num_threads>max_threads)
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
            if(num_threads<1)
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
	     
	     
        }
        else if(strcmp(argv[i],"--res")==0)
        {
            if(i+2 >= argc)
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
            if(argv[i+1][0] == '-' || argv[i+2][0] == '-')
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
	     
            i++;
            istringstream is(argv[i]);
	     
            uint w=0;
            is >> w;
	     
            i++;
            is.clear();
            is.str(argv[i]);
	     
            uint h=0;
            is >> h;
	     
            VERBOSE1(cout << "screen resolution: " << w << "x" << h << endl;);
	     
            screen_w=w;
            screen_h=h;
        }
        else if(strcmp(argv[i], "--rate")==0)
        {
            if(i==argc-1)
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
            if(argv[i+1][0]=='-')
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
	     
            i++;
            istringstream is(argv[i]);
	     
            uint rate;
            is >> rate;
            progstate.setSampleRate(rate);
	     
            VERBOSE0(cout << "sample rate: " << rate << endl;);
        }
        else if(strcmp(argv[i], "-r")==0)
        {
            uint rate=22050;
            progstate.setSampleRate(rate);
	     
            VERBOSE0(cout << "sample rate: " << rate << endl;);
        }
        else if(strcmp(argv[i], "--data")==0)
        {
            if(i==argc-1)
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
            if(argv[i+1][0]=='-')
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
	     
            i++;
            istringstream is(argv[i]);
	     
            string dir;
            is >> dir;
            progstate.setDataPrefix(dir);
	     
            VERBOSE0(cout << "data dir: " << dir << endl;);
	     
            if(!aexec_set)
            {
                autoexec_path = progstate.getDataPrefix() + "autoexec.cfg";
            }
	     
        }
        else if(strcmp(argv[i], "--cfg")==0)
        {
            if(i==argc-1)
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
            if(argv[i+1][0]=='-')
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
	     
            i++;
            istringstream is(argv[i]);
            string dir;
            is >> dir;
	     
            autoexec_path = dir;
	     
            VERBOSE0(cout << "exec: " << dir << endl;);
	     
            aexec_set=true;
        }
        else if(strcmp(argv[i], "-a")==0)
        {
            if(i==argc-1)
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
            if(argv[i+1][0]=='-')
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
	     
            i++;
            istringstream is(argv[i]);
            is >> run_after_load;
        }
        else if(strcmp(argv[i], "-w")==0)
        {
            if(i+2 >= argc)
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
            if(argv[i+1][0] == '-' || argv[i+2][0] == '-')
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
	     
            i++;
            istringstream is(argv[i]);
            uint th=0;
            is >> th;
	     
            if(th>=initial_load.size())
            {
                if(th>max_threads)
                {
                    cout << "argument error, try: " << argv[0] << " -h" << endl;
                    return -1;
                }
		  
                initial_load.resize(th+1);
            }
	     
            i++;
            initial_load[th].assign(argv[i]);
        }
        else if(strcmp(argv[i],"-h")==0)
        {
            show_help=true;
        }
        else if(strcmp(argv[i],"-s") == 0)
        {
            useGl=false;
        }
        else
        {
            if(argv[i][0]=='-')
            {
                cout << "argument error, try: " << argv[0] << " -h" << endl;
                return -1;
            }
	     
            initial_load[0].assign(argv[i]);
        }
    }
   
    if(show_help)
    {
        cout << "Multiverk " << VERSION << " (C) 2007 Anton Olofsson" << endl;
        cout << "help:" << endl <<
            "          arg       : initially load 'arg' (now: " << initial_load[0] << ")" << endl <<
            "  --nws   arg       : num workspaces(threads) (now: " << num_threads << ") min: 1, max: " << max_threads << endl <<
            //"  --chan  arg       : num channels (now: " << progstate.getNChan() << ")" << endl <<
            "  --res   arg0 arg1 : screen res: 'arg0'x'arg1' (now: " << screen_w << "x" << screen_h << ")" << endl <<
            "  --rate  arg       : sample rate (now: " << 	progstate.getSampleRate()  << ")" << endl <<
            "  -r                : sample rate to 22050" << endl<<
            "  --data  arg       : program data dir(now: " << progstate.getDataPrefix() << ")" << endl <<
            "  --cfg   arg       : startup script, run before anything else (now: " << autoexec_path << ")" << endl <<
            "  -a      arg       : run a second script, run after everything else (now: " << run_after_load << ")" << endl <<
            "  -w      arg0 arg1 : load 'arg1' into workspace 'arg0'" << endl <<
            "  -s                : use software rendering" << endl;
        //FIXME: add -w
        return 1;
    }
   
    return 0;
}

//FIXME: move to MasterRes?
int initialCmds()
{
    //exec cfg
    con->show("rootexec: " + autoexec_path);
    //con->cmd.parseLine( "rootexec " + autoexec_path ); //autoexec EXEC*/
   
    con->execCmdNow("rootexec " + autoexec_path);
   
    //initial load
    for(uint i=0;i<initial_load.size(); i++)
    {
        if(initial_load[i].length()==0)continue;
        if(i>=num_threads)break;
	
        size_t last = initial_load[i].find_last_of("/");
	
        string path, name;
	
        if(last==string::npos) //no '/' found
        {
            path = string("./");
            name = initial_load[i];
        }
        else if(last==initial_load[i].size()) //no filename given
        {
            path = initial_load[i];
        }
        else
        {
            path = initial_load[i].substr(0,last+1);
            name = initial_load[i].substr(last+1);
        }
	
        if(name.length()!=0)
        {
            //remove suffix(.kwf)
            last = name.find_last_of(".");
            if(last!=string::npos)name = name.substr(0,last);
	     
            cout << "prefix: " << path << endl;
            cout << "loading: " << name << endl;
	     
            /*con->cmd.parseLine("prefix " + path);
              con->cmd.parseLine("load " + name);*/
	     
            //cmd uses maincomp
            Main* old_mc= maincomp;
            maincomp = w_threads[i]->maincomp;
	     
            con->execCmdNow("prefix " + path);
            con->execCmdNow("load " + name);
	     
            maincomp=old_mc;
        }
        else
        {
            Main* old_mc= maincomp;
            maincomp = w_threads[i]->maincomp;
	     
            cout << "prefix: " << path << endl;
            //con->cmd.parseLine("prefix " + path);
            con->execCmdNow("prefix " + path);
	     
            maincomp=old_mc;
        }
    }
   
    if(run_after_load.length()!=0)
    {
        //con->cmd.parseLine( ( string("rootexec ") +  run_after_load).c_str());
        con->execCmdNow("rootexec " +  run_after_load);
    }
   
    return 0;
}


//rewrite
void quit()
{
    cout << "sdl quit" << endl;
    SDL_Quit();
}

//ok
int loadPixmaps(const string prefix)
{
    //pix.setMaskColor(0xff,0xff,0xff);
    
    SDL_Color c = {140,140,140,0};
    
    pix.setBuildColor(c);
   
    DIR* dir = opendir(prefix.c_str());
    if(dir==NULL)
    {
        ERROR(cout << "no such dir: " << prefix.c_str());
        return -1;
    }
   
    dirent* ent;
    while((ent = readdir(dir))!=NULL)
    {
        string fname(ent->d_name);
        if(fname=="." || fname == "..")continue;
        string::size_type pos = fname.find_last_of("."); 
        if(pos==string::npos)
        {
            continue;
        }
	
        string end(fname,pos,fname.length());
        if(end != ".bmp"/* && end!=".png"*/)continue;
	
        //cout << prefix+fname  << " " << string(fname,0,pos) << endl;
	
        string name=string(fname,0,pos);
        
        bool do_mask=false;
        if(name=="no_draw")do_mask=true;
        //if(name=="insignia")do_mask=true;
        if(name=="icon")do_mask=true;
        if(name=="10c_back")do_mask=true;
        if(name=="20c_back")do_mask=true;
        if(name=="30c_back")do_mask=true;
        if(name=="in")do_mask=true;
        if(name=="out")do_mask=true;
        if(name=="slide_knobv")do_mask=true;
        if(name=="slide_knob")do_mask=true;
        if(name=="slide_under")do_mask=true;
        if(name=="slide_underv")do_mask=true;
        if(name=="bs_left")do_mask=true;
        if(name=="bs_right")do_mask=true;
        if(name=="bs_middle_v")do_mask=true;
        if(name=="bs_middle_h")do_mask=true;
        if(name=="bs_up")do_mask=true;
        if(name=="bs_down")do_mask=true;
        //if(name=="back96x24")do_mask=true;
        //if(name=="back128x24")do_mask=true;
        if(name=="knob16x16")do_mask=true;
        if(name=="led_on")do_mask=true;
        if(name=="led_off")do_mask=true;
        if(name=="ledg_on")do_mask=true;
        if(name=="ledg_off")do_mask=true;
        if(name=="ledr_on")do_mask=true;
        if(name=="ledr_off")do_mask=true;
	
        if(name=="wf_tri_s")do_mask=true;
        if(name=="wf_squ_s")do_mask=true;
        if(name=="wf_saw_s")do_mask=true;
        if(name=="wf_sin_s")do_mask=true;
        if(name=="wf_pulse_s")do_mask=true;
        if(name=="wf_noise_s")do_mask=true;
	
        if(name=="slew_lin")do_mask=true;
        if(name=="slew_sin")do_mask=true;
        if(name=="slide_knobv_10_8")do_mask=true;
        if(name=="slide_knobv_14_12")do_mask=true;
        if(name=="slide_underv_128_6")do_mask=true;
        if(name=="inout_used")do_mask=true;
	
        pix.loadSurface(prefix+fname, name, do_mask);
    }
   
    pix.setBuildName("back_build32x32");
   
    VERBOSE1(cout << "loaded " << pix.getLoadedBytes()/(1024.0*1024.0) << "mb worth of pixmaps" << endl;);
   
    closedir(dir);
   
    return 0;
}


/************/
/*WORKTHREAD*/
/************/

//init: 

uint WorkThread::num_th=0;

WorkThread::WorkThread()
{
    maincomp=NULL;
    th=NULL;
   
   
    th_id = num_th++;
   
    sync_count=0;
    is_master=false;
   
    //check_ms_count=CHECK_MS_INTERVAL;
    //frozen=false;
}

WorkThread::~WorkThread()
{
    free();
   
    /*if(th!=NULL)
      {
      }*/
   
} 

void WorkThread::free()
{
    if(maincomp==NULL)return;
   
    delete maincomp;
    maincomp=NULL;
   
#ifdef USE_MIDI
    midi.close();
#endif
}


void WorkThread::init(Screen* screen, BitmapFont* f, Console* c/*uint fps*/)
{
    maincomp = new Main(0,0, screen, f, c);
   
    fps.setFPSLimit(progstate.getFps());
   
    //midi
#ifdef USE_MIDI
    if(!midi.open("mv_0" + spl::toStr(num_th-1), /*in:*/ 1, /*out:*/ 1))
    {
        cout << midi.errorMess() << endl;
        //return 1; //FIXME: really need error checking
    }
    else
    {
        //FIXME: make opt
        midi.setPollTimeout(0/*keep at 0==nonblocking(to much other stuff to do in th)*/);
    }
   
    midi.setEventCallbacks(maincomp->getMIDIcallbacks());
#endif
    /* ostringstream ss;
       ss << midi.getClientName() << " [" << midi.getClientId() << ":";
       for(uint i=0;i<midi.getNumPorts();i++)ss << midi.getClientPort(i) << ",";
       ss << "]";
       SDL_WM_SetCaption(ss.str().c_str(),NULL);
       ss.clear();*/
   
    //MUST change ground depending on stereo/mono
    if(progstate.getNChan()==2)
    {
        maincomp->getGround().setStereo(true);
    } 
#ifdef USE_MIDI
    maincomp->setMIDI(&midi);
#endif
}


void WorkThread::startThread()
{
    DASSERT(th==NULL);
    th = SDL_CreateThread(w_th_exec, this);
}


void WorkThread::takeMaster()
{
    is_master=true;
    master_res.maincomp=maincomp; 
    master_res.master_th_id=th_id;
    ::maincomp = maincomp; //FIXME
    //cmd_caller_th=this;
   
}

void WorkThread::yieldMaster()
{
    is_master=false;
    master_res.maincomp=NULL;
    ::maincomp = NULL;
}


void WorkThread::sync(uint to, bool set_state, int state_to)
{
    sync_count=to;
   
    //sync requires to drop current samples
    maincomp->smpQueueClear();
    maincomp->syncAllToIndex(0);
   
    if(set_state)maincomp->bench->setCurrentMarkerIndex(state_to);
}

int WorkThread::generate()
{
    //it is important to have many "small" samples rather than one in audio queue:
    //otherwise it is impossible to keep queue from draining completely
   
    //(1) generate data(keep it full)
    while(maincomp->smpQueueIn() < maincomp->smpQueueSize())
    {
        maincomp->smpQueuePush();
    }
   
    return 0;
}



int w_th_exec(void* data)
{
    WorkThread* me = (WorkThread*)data;
   
    bool stats_upd=false;
   
    bool idle_some=false;
    bool run=true;
    bool did_sync=false;
   
    bool is_frozen=false; //never for is_freezer
   
    bool pending_freeze=false;
    bool init_freeze=false; //start freeze
    bool is_freezer=false; //thread calling for freeze
    bool unfreeze=false;
    bool pending_un_freeze=false;
    bool all_frozen=false;
    bool yield_master=false;
   
    int check_sm_count=WorkThread::CHECK_MS_INTERVAL;
   
    //allthough nothing must be touched during a freeze, a me->th_id for debug is ok
   
    //FIXME: make master() and slave()
    while(run)
    {
        //lockMasterRes(); //!
        me->stats.tick();
	
        if(!is_frozen && !is_freezer)me->fps.startFrame();
	
        check_sm_count--;
	
        if(check_sm_count==0)
        {
            check_sm_count=WorkThread::CHECK_MS_INTERVAL;
	     
            lockSharedMess(); //FIXME: possible to copy in mess and release?
	     
            if(stats_upd)
            {
                shared_mess.stats_sum.update(me->th_id, me->stats);
                stats_upd=false;
            }
	     
	     
            if(!is_freezer)
            {
                if(is_frozen)
                {
                    if(!shared_mess.freeze_threads)
                    {
                        VERBOSE2(cout << me->th_id << " , unfreeze" << endl;);
                        DASSERT(shared_mess.num_frozen>0);
                        shared_mess.num_frozen--;
                        is_frozen=false;
                    }
                    //ok to go on as usuall, frame time somewhat off(no matter?)
                    me->fps.startFrame();
                }
                else
                {
                    if(shared_mess.freeze_threads)
                    {
                        VERBOSE2(cout << me->th_id << " , freeze" << endl;);
			    
                        DASSERT(!me->is_master); //only master should be able to freeze
			    
                        is_frozen=true;
                        shared_mess.num_frozen++;
                        me->fps.endFrame(); //cant even use fps when frozen
                    }
                }
            }
	     
            if(!is_frozen)
            {
                if(me->is_master)
                {
                    if(shared_mess.stats_sum.updatedSum())
                    {
                        lockMasterRes(); //!
                        for(uint i=0;i<me->num_th;i++)
                        {
                            master_res.setStat(i, shared_mess.stats_sum.getSingle(i));
                        }
                        master_res.setStatSum(shared_mess.stats_sum.getSum());
                        unlockMasterRes(); //!
                    }
		       
		       
                    if(pending_un_freeze)
                    {
                        if(shared_mess.num_frozen==0)
                        {
                            pending_un_freeze=false;
				 
                            VERBOSE2(cout << me->th_id << " , all unfrozen" << endl;);
                            //all done, back to normal
				 
                            is_freezer=false;
				 
                            DASSERT(!all_frozen);
                            DASSERT(!pending_freeze);
				 
                            me->fps.startFrame();
                        }
                    }
                    else if(unfreeze)
                    {
                        unfreeze=false;
			    
                        //update done, let threads get back to business, however
                        //this thread needs to wait for all to unfreeze before starting normal op.
                        //: threads must be unfrozen to be frozen again(if that would happen before)
                        DASSERT(is_freezer);
                        all_frozen=false;
                        shared_mess.freeze_threads=false;
			    
                        pending_un_freeze=true;
			    
                    }
                    else if(pending_freeze)
                    {
                        if(shared_mess.num_frozen==me->num_th-1) //all but this thread frozen
                        {
                            pending_freeze=false;
				 
                            all_frozen=true;
                        }
                    }
                    else if(init_freeze) //do freeze?
                    {
                        //only time a new freeze is possible should be when all threads are unfrozen
                        DASSERT(shared_mess.num_frozen==0);
			    
                        init_freeze=false;
			    
                        shared_mess.freeze_threads=true;
			    
                        pending_freeze=true;
                    }
                    else //no other messages during freeze!
                    {
                        
                        lockMasterRes(); //!
                        //poll master resources for messages and relay to shared_mess
                        if(master_res.do_shutdown)
                        {
                            //dont reset this mess: MUST reach draw_th
                            //master_res.do_shutdown=false;
				 
                            shared_mess.shutdown=true;
                        }
			    
                        if(master_res.change_master)
                        {
                            master_res.change_master=false;
				 
                            shared_mess.change_master=true;
                            shared_mess.next_master_th_id = master_res.next_master_th%me->num_th; //( (me->th_id+1) % me->num_th);
				 
                            //cant yield directly: still have lockMasterRes()
                            yield_master=true;
				 
                            VERBOSE2(cout << me->th_id << " changes master to:" << shared_mess.next_master_th_id << endl;);
                        }
			    
                        if(master_res.do_sync)
                        {
                            master_res.do_sync=false;
				 
                            me->sync(me->sync_count+1); //dont set state for master
                            did_sync=true;
                            shared_mess.sync_to=me->sync_count;
				 
                            if(master_res.do_state_sync)
                            {
                                master_res.do_state_sync=false;
				      
                                shared_mess.sync_state=true;
				      
                                shared_mess.sync_state_to=me->maincomp->bench->getCurrentMarkerIndex();
                            }
                            else shared_mess.sync_state=false;
				 
                            VERBOSE2(cout << me->th_id << ", master syncs: " << me->sync_count << endl;);
                        }
                        unlockMasterRes(); //!
                    }
                } //end is_master
		  
                if(yield_master)
                {
                    lockMasterRes(); //!
                    yield_master=false;
                    me->yieldMaster();
                    unlockMasterRes();
                }
		  
		  
                //frozen threads dont get here so:
                if(!is_freezer) //dont let it shutown while things are frozen
                {
                    if(shared_mess.shutdown)
                    {
                        run=false;
                    }
		       
                    if(shared_mess.sync_to != me->sync_count)
                    {
                        me->sync(shared_mess.sync_to, shared_mess.sync_state, shared_mess.sync_state_to);
			    
                        VERBOSE2(cout << me->th_id << ", slave syncs: " << me->sync_count << endl;);
			    
                        did_sync=true;
                    }
		       
                    if(shared_mess.change_master && (shared_mess.next_master_th_id == me->th_id))
                    {
                        VERBOSE2(cout << me->th_id << "takes master" << endl;);
                        shared_mess.change_master=false;
                        lockMasterRes(); //!
                        me->takeMaster();
                        unlockMasterRes(); //!
                    }
                }
            }//end !is_frozen
	     
            //FIXME: this should probably come after generate but that would require another lock
            while(me->maincomp->haveNextCmd())
                shared_mess.maincomp_cmd_queue.push(me->maincomp->getNextCmd());
	     
            if(me->is_master)
            {
                lockMasterRes(); //!
                while(!shared_mess.maincomp_cmd_queue.empty())
                {
                    master_res.con->execCmd(shared_mess.maincomp_cmd_queue.front());
                    shared_mess.maincomp_cmd_queue.pop();
                }
                unlockMasterRes(); //!
            }
	     
	     
            unlockSharedMess();
	     
        }//end check_sm_count
	
        DASSERT(!init_freeze); //should be caught before here
	
        if(is_freezer/* && me->is_master*/)
        {
            DASSERT(me->is_master);
	     
            //master_res.draw();
	     
            if(pending_freeze) 
            {
                //usleep(me->IDLE_TIME_USEC); //good?
                //SDL_Delay(1);
            }
            else if(all_frozen)
            {
                //cout << "exec queue" << endl;
                //all is frozen, exec what was needed, and unfreeze
                cmd_caller_th=me; //DONT FORGET(ok for cmd to change)
                while(con->pQueueIn()>0)con->pQueueExecNext();
		  
                all_frozen=false;
		  
                unfreeze=true;
                VERBOSE2(cout << me->th << ", actual exec" << endl;);
            }
            else if(pending_un_freeze)
            {
                //usleep(me->IDLE_TIME_USEC); //good?
                SDL_Delay(1);
            }
	     
            me->stats.tack(Stats::EVENT);
            //me->fps.endFrame();
	     
            //unlockMasterRes(); !
            continue;
        }
	
        if(is_frozen)
        {
            DASSERT(!me->is_master);
            DASSERT(!is_freezer);
            //usleep(me->IDLE_TIME_USEC); //good?
            //SDL_Delay(1);
	     
            me->stats.tack(Stats::EVENT);
	     
            //if(me->is_master)unlockMasterRes();
            continue;
        }
	
        //order here could be important 
        //gen audio should come directly after midi+events so as to minimize latency
        //if draw was in between latency would be greater
	
        //MIDI
        //FIXME: midi event latency OK, but start gen new samples is not therefore 
        //not midi's fault latency
#ifdef USE_MIDI
        if(!me->midi.pollEvents())
        {
            cout << me->midi.errorMess() << endl;
        }
#endif
        me->stats.tack(Stats::EVENT); //this means that all th handling+midi goes under event stat

        if (me->th_id == 0)
        {
            lockMasterRes(); //!
            master_res.sdlInput();
            unlockMasterRes(); //!
        }

        //if master HANDLE EVENTS
        if(me->is_master)
        {
            lockMasterRes(); //!
            int done = con->handleEvents(master_res.in);
	     
            if(con->pQueueIn()>0)
            {
                //not all commands need to freeze execution, but it matters little so do it:
		  
                //now it is needed to :
                //1.freeze threads
                //2.wait for freeze_count
                //3.exec command
                //4. unfreeze
                //5. wait for all to unfreeze
		  
                init_freeze=true;
                is_freezer=true;
		  
                check_sm_count=1; //do it directly
            }

            master_res.c_fps = me->fps.getFps();
            if(!done && master_res.maincomp != NULL)
            {
                master_res.handleInput();
            }
            
            unlockMasterRes(); //!
        }
        //me->stats.tack(Stats::EVENT); //this means that all th handling+midi goes under event stat
	
	
        //GENERATE AUDIO
        me->stats.tick();
        me->generate();
        me->stats.tack(Stats::GEN);
	
        //MIX+PLAY AUDIO
        me->stats.tick();
        lockSharedAudio();
	
        if(did_sync)
        {
            //cout << me->th_id << ", resets" << endl;
            //have to reset upload so that it only contains "new, synced" samples all will do this
            shared_audio.resetFor(me->th_id);
            did_sync=false;
        }
	
	
        if(shared_audio.shouldUpload(me->th_id, me->sync_count)) 
        {
            //cout << me->th_id << ", uploads" << endl;
            shared_audio.upload(me->maincomp, me->th_id, me->sync_count);
        }
	
        //is this thread the one mixing to adev?
        if(shared_audio.shouldMix(me->th_id))
        {
            //cout << me->th_id << ", plays with sync_count: " << me->sync_count  << endl;
            if(progstate.getOn())shared_audio.mix(progstate.getPlayOnDevice());
        }
        else
        {
            idle_some=true;
        }
	
        unlockSharedAudio();
	
        me->stats.tack(Stats::PLAY);
	
	
        //DRAW 
        //FIXME: This is a very dubious solution(at it's best)
        //(gl needs to run in only one context)
        //because screen was created in main th we can safely run draw here but:
        //this makes for nothing in the actual master to use maincomp during this time: CHECK THIS
        //
        //Better solution: make draw_th work again by creating draw context there AND refactor ALL
        //classes so only their draw functions takes screen as argument.
        //
        
        if(me->th_id == 0)
        {
            /*if(!me->is_master)*/lockMasterRes();
            
            if(master_res.do_toggle_fullscreen)
            {
                master_res.screen->toggleFullScreen();
                master_res.do_toggle_fullscreen = false;
            }

            if(master_res.maincomp != NULL)
            {
                master_res.c_fps = me->fps.getFps();
	     
                me->stats.tick();
                master_res.draw();
                me->stats.tack(Stats::DRAW);
            }
            /*if(!me->is_master)*/unlockMasterRes();
        }

        if(idle_some)
        {
            idle_some=false;
            //usleep(me->IDLE_TIME_USEC); //FIXME: good?
            //SDL_Delay(1);
        }
	
        //before fps.endFrame() so draw_th may compute while this sleeps(in endFrame)
        //draw_th never uses 'me'
        //if(me->is_master)unlockMasterRes();
	
        me->fps.endFrame();
	
        //FIXME: not accurate if more threads than processors
        me->stats.tSleep(me->fps.getMilliToSpare()*1000);
        stats_upd = me->stats.accum();
    }
    return 0;
}

int draw_th_fun(void* data)
{
    //WARNING: only lockMasterRes in here
    spl::Fps fps(INITIAL_FPS);
    Stats stats;
   
    bool stats_upd=false;
    bool run=true;
    while(run)
    {
        fps.startFrame();
	
        stats.tick();
	
        lockMasterRes();
        if(master_res.maincomp!=NULL) //this can be NULL during change_master
        {
            master_res.draw();
	     
            master_res.d_fps = fps.getFps();
	     
            if(master_res.do_shutdown)run=false;
            if(master_res.change_d_fps)
            {
                master_res.change_d_fps=false;
                fps.setFPSLimit((int)master_res.d_run_fps);
                master_res.con->show("draw fps set: %d", fps.getFPSLimit());
            }
        }
	
        if(stats_upd)
        {
            stats_upd=false;
            master_res.setStatDrawTh(stats);
        }
	
	
        unlockMasterRes();
	
        stats.tack(Stats::DRAW);
        stats_upd = stats.accum();
	
        fps.endFrame();
    }
   
    return 0;
}

/*BUFFER*/

//DTICK();

//FIXME: WARNING: autosave does not save states, neither does save
//autosave + ssave should go through all comps and save states, that way have choice
/*
  if(progstate.autosave)
  {
  * 
  if(progstate.autotick<=0)
  {
  progstate.autotick=SDL_GetTicks();
  }
  else 
  {
  Uint32 t = SDL_GetTicks();
  uint ut = (uint)( (t-progstate.autotick)/1000.0 );
  if(ut>=progstate.autotime)
  {
  progstate.autotick=0;
  autosave();
  }
  }
  }
*/

