#ifndef CMDS_H
# define CMDS_H

# include <sys/types.h>
# include <dirent.h>

# include <string>
# include <sstream>
#include "../main/config.h"

//#include "g_comp/g_all.h"

//FIXME: UGLY: depends on beeing included in main for data etc...

/*********/
/*HELPERS*/
/*********/

//WORKFIX: somethings(module path etc) should be set for all maincomps

#ifdef BUILD_WINDOWS
char* get_current_dir_name()
{
    char* buffer;
    if( (buffer = _getcwd( NULL, 0 )) == NULL )
    {
        cout << "_getcwd error" << endl;
    }
    return buffer;
}
#endif

template <class T>
bool parseNum(const std::string& s, uint num, vector<T>& ret)
{
    using namespace std;
   
    if(num==0)return true;
   
    string::size_type beg=s.find_first_not_of(string(" "), 0);
    for(uint c=0;c<num-1;c++)
    {
        string::size_type end = s.find_first_of(string(" "), beg);
        if(end==string::npos)
        {
            con->show("invalid input");
            return false;
        }
        string sub(s,beg,end);
        beg=s.find_first_not_of(" ", end);
        if(beg==string::npos)
        {
            con->show("invalid input");
            return false;
        }
	
        istringstream ss(sub);
        T d;
        ss >> d; 
        ret.push_back(d);
    }
   
    //last
    string sub(s,beg);
    istringstream ss(sub);
    T d;
    ss >> d;
    ret.push_back(d);
   
    return true;
}



void parseRGB(const string& s, Uint8* color)
{
    color[0]=0;
    color[1]=0;
    color[2]=0;
   
    string::size_type beg=s.find_first_not_of(string(" "), 0);
    for(int c=0;c<2;c++)
    {
        string::size_type end = s.find_first_of(string(" "), beg);
        if(end==string::npos)
        {
            con->show("invalid color");
            return;
        }
        string sub(s,beg,end);
        beg=s.find_first_not_of(" ", end);
        if(beg==string::npos)
        {
            con->show("invalid color");
            return;
        }
	
        istringstream ss(sub);
        int i;
        ss >> i; 
        color[c]=(Uint8)i;
    }
    string sub(s,beg);
    istringstream ss(sub);
    int i;
    ss >> i;
    color[2]=(Uint8)i;
}

void parseRGB(const string& s, SDL_Color& ret)
{
    Uint8 color[3];
   
    string::size_type beg=s.find_first_not_of(string(" "), 0);
    for(int c=0;c<2;c++)
    {
        string::size_type end = s.find_first_of(string(" "), beg);
        if(end==string::npos)
        {
            con->show("invalid color");
            return;
        }
        string sub(s,beg,end);
        beg=s.find_first_not_of(" ", end);
        if(beg==string::npos)
        {
            con->show("invalid color");
            return;
        }
	
        istringstream ss(sub);
        int i;
        ss >> i; 
        color[c]=(Uint8)i;
    }
    string sub(s,beg);
    istringstream ss(sub);
    int i;
    ss >> i;
    color[2]=(Uint8)i;
   
    ret.r=color[0];
    ret.g=color[1];
    ret.b=color[2];
}

/**********/
/*COMMANDS*/
/**********/


void grab_in(void* arg)
{
    if(arg==NULL)
    {
        con->show("grab in:%d",progstate.getGrabInput());
        return;
    }
   
    int on = atoi((char*)arg);
    progstate.setGrabInput(on);
   
    if(progstate.getGrabInput())SDL_WM_GrabInput(SDL_GRAB_ON);
    else SDL_WM_GrabInput(SDL_GRAB_OFF);
   
    con->show("grab in:%d",progstate.getGrabInput());
}


void ws_bound(void* arg)
{
    if(arg==NULL)
    {
        con->show("virtual screen: %d, %d",progstate.getWSxmax(), progstate.getWSymax());
        return;
    }
   
    string s((char*)arg);
    vector<int> size;
   
    if(!parseNum(s,2,size))
    {
        con->show("invalid arg");
        return;
    }
   
   
    if(size[0]<0)size[0]=-size[0];
    if(size[1]<0)size[1]=-size[1];
   
    progstate.setWSxmax(size[0]);
    progstate.setWSymax(size[1]);
   
    maincomp->setCompOffBound(progstate.getWSxmin(), progstate.getWSymin(), 
    progstate.getWSxmax(), progstate.getWSymax());
   
    maincomp->setCompOff(0,0);
   
    con->show("virtual screen: %d, %d",progstate.getWSxmax(), progstate.getWSymax());
}


void con_alpha(void* arg)
{
    if(arg==NULL)
    {
        con->show("alpha: %d",con->getAlpha());
        return;
    }
    int on = atoi((char*)arg);
    if(on<0)on=0;
    if(on>255)on=255;
    con->setAlpha(on);
    con->show("alpha:%d",con->getAlpha());
}


void st_saveall(void* arg)
{
    con->show("all states saved");
    maincomp->bench->saveAllStateIndex();
}

void process(void* arg)
{
    if(arg==NULL)
    {
        //con->show("process:%d",progstate.getOn());
        progstate.setOn(!progstate.getOn());
        con->show("process:%d",progstate.getOn());
        return;
    }
   
    int on = atoi((char*)arg);
    progstate.setOn(on);
    con->show("process:%d",progstate.getOn());
}

void playondev(void* arg)
{
    if(arg==NULL)
    {
        progstate.setPlayOnDevice(!progstate.getPlayOnDevice());
        con->show("playondev:%d",progstate.getPlayOnDevice());
        return;
    }
   
    int on = atoi((char*)arg);
    progstate.setPlayOnDevice(on);
    con->show("playondev:%d",progstate.getPlayOnDevice());
}


void line_fg(void* arg)
{
    if(arg==NULL)
    {
        progstate.setDrawLinesFG(!progstate.getDrawLinesFG());
        con->show("line fg:%d",progstate.getDrawLinesFG());
        return;
    }
   
    int on = atoi((char*)arg);
    progstate.setDrawLinesFG(on);
    con->show("line fg:%d",progstate.getDrawLinesFG());
}

/* 
   str: 1 2 *3(repeat last 3x times: 2 2 2 2) 3 #3(linear step from last: 3 4 5 6)
   * so 1 2 *3 3 #3 expands to: 1 2 2 2 2 3 4 5 6
   */

void set_state_queue(void* arg)
{
    if(arg==NULL)
    {
        maincomp->bench->voidMarkerList();
        con->show("removing state seq");
        return;
    }
   
    list<int> marks;
   
    //first split into queue/options
    string str((char*)arg);
    string opt;
    string q;
   
    size_t opt_p = str.find_first_of(":");
    if(opt_p!=string::npos)
    {
        opt = str.substr(opt_p+1);
        q = str.substr(0, opt_p-1);
    }
    else
    {
        q = str;
    }
   
    //PARSE QUEUE
    string c;
   
    ostringstream qstr;
   
    while(q.length()>0)
    {
        size_t s = q.find_first_not_of(" ");
	
        if(s==string::npos)
        {
            break;
        }
	
        size_t e = q.find_first_of(" ", s);
	
        if(e!=string::npos)
        {
            c = q.substr(s, e-s);
            q = q.substr(e+1);
        }
        else 
        {
            c = q.substr(s);
            q.clear();
        }
	
        if(c[0]=='*')
        {
            if(marks.size()==0)
            {
                con->show("input error: start with *");
                return;
            }
            if(c.size()==1)
            {
                con->show("input error: only *");
                return;
            }
	     
            istringstream ss(c.substr(1));
            int times;
            ss >> times;
            if(ss.fail())
            {
                con->show("input error: number parse");
                return;
            }
	     
            for(int i=0;i<times;i++)
            {
                int n = marks.back();
                marks.push_back(n);
                qstr << n << " ";
            }
	     
        }
        else if(c[0]=='#')
        {
            if(c.size()==1)
            {
                con->show("input error: only #");
                return;
            }
	     
            int start=0;
            if(marks.size()!=0)
            {
                start = marks.back()+1;
            }
	     
            istringstream ss(c.substr(1));
            int times;
            ss >> times;
            if(ss.fail())
            {
                con->show("input error: number parse");
                return;
            }
	     
            for(int i=0;i<times;i++)
            {
                int n = start+i;
                marks.push_back(n);
                qstr << n << " ";
            }
        }
        else if(isdigit(c[0]))
        {
            int n=0;
            istringstream ss(c);
            ss >> n;
            if(ss.fail())
            {
                con->show("input error: number parse");
                return;
            }
            else 
            {
                marks.push_back(n);
                qstr << n << " ";
            }
	     
        }
        else
        {
            con->show("input error: unrecognized token");
            return;
        }
    }
   
    //PARSE OPTIONS
    bool apply_direct=false;
    size_t p = opt.find("n");
    if(p!=string::npos)apply_direct=true;
   
    bool do_all=progstate.getStateTickAll();
    p = opt.find("s");
    if(p!=string::npos)do_all=false;
   
    p = opt.find("a");
    if(p!=string::npos)do_all=true;  
   
    //build string to show:
   
   
   
    //APPLY
    maincomp->bench->setMarkerList(marks, do_all, apply_direct);
   
   
   
    con->show("markers: %s options: %s", qstr.str().c_str(), opt.c_str());
}


/*void showdumpsec(void* arg)
  {
  if(arg==NULL)
  {
  con->show("showdumpsec:%d",progstate.showDumpSec());
  return;
  }
  int on = atoi((char*)arg);
  progstate.showDumpSec(on);
  con->show("showdumpsec:%d",progstate.showDumpSec());
  }*/

void showhavestate(void* arg)
{
    if(arg==NULL)
    {
        con->show("showhavestate:%d",progstate.getShowHaveState());
        return;
    }
    int on = atoi((char*)arg);
    progstate.setShowHaveState(on);
   
    con->show("showhavestate:%d",progstate.getShowHaveState());
}

void sync_comps(void* arg)
{
    /*   int index=0;
         if(arg!=NULL)
         {
         index = atoi((char*)arg);
         }
         maincomp->syncAllToIndex(index);*/
    master_res.do_sync=true;
    //con->show("synced: %d",index);
    con->show("sync: components");
}

void sync_states(void* arg)
{
    master_res.do_state_sync=true;
    con->show("sync: state");
}

void sync(void* arg)
{
    sync_comps(NULL);
    sync_states(NULL);
}

void mb_label(void* arg)
{
    if(arg==NULL)
    {
        con->show("usage: mb_label n name");
        return;
    }
   
    if(maincomp->getSelectedComp()==NULL || maincomp->getSelectedComp()->getSCType()!=G_BRIDGE)
    {
        con->show("select bridge comp"); 
        return;
    }
   
    GBridge* b = (GBridge*)maincomp->getSelectedComp(); 
   
    string s((char*)arg);
    string::size_type beg=s.find_first_not_of(string(" "), 0);
    string::size_type end = s.find_first_of(string(" "), beg);
    if(end==string::npos)
    {
        con->show("invalid");
        return;
    }
    string num(s,beg,end);
   
    beg=s.find_first_not_of(" ", end);
    if(beg==string::npos)
    {
        con->show("invalid");
        return;
    }
   
    string name(s,beg,s.length()-beg);
   
    istringstream ss(num);
    int i;
    ss >> i;
   
    b->setLabel(name, i);
}

void mb_label_rem(void* arg)
{
    if(arg==NULL)
    {
        con->show("usage: mb_label_rem n");
        return;
    }
   
    if(maincomp->getSelectedComp()==NULL || maincomp->getSelectedComp()->getSCType()!=G_BRIDGE)
    {
        con->show("select bridge comp"); 
        return;
    }
   
    GBridge* b = (GBridge*)maincomp->getSelectedComp(); 
   
    string name("");
   
    string num((char*)arg);
    istringstream ss(num);
    int i;
    ss >> i;
   
    b->setLabel(name, i);
}



void set_lin_color(void* arg)
{
    SDL_Color color = progstate.getLineColor();
    if(arg==NULL)
    {
        con->show("lin color: %d %d %d", color.r , color.g, color.b);
        return;
    }
   
    string s((char*)arg);
    parseRGB(s, color);
   
    con->show("lin color: %d %d %d", color.r , color.g, color.b);
    progstate.setLineColor(color);
   
}

void set_graph_color(void* arg)
{
    if(arg==NULL)
    {
        SDL_Color c =  progstate.getGraphColor();
        con->show("graph color: %d %d %d",c.r, c.g, c.b);
        return;
    }
   
    string s((char*)arg);
    SDL_Color color;
    parseRGB(s, color);
    progstate.setGraphColor(color);
    con->show("graph color: %d %d %d",color.r, color.g, color.b);
}

void set_bg_color(void* arg)
{
    if(arg==NULL)
    {
        con->show("bg color: %d %d %d", progstate.bg_color.r,progstate.bg_color.g,progstate.bg_color.b);
        return;
    }
   
   
    Uint8 color[3];
   
    string s((char*)arg);
    parseRGB(s, color);
   
    progstate.bg_color.r=color[0];
    progstate.bg_color.g=color[1];
    progstate.bg_color.b=color[2];
    con->show("bg color: %d %d %d", progstate.bg_color.r,progstate.bg_color.g,progstate.bg_color.b);
}

void set_mousesens(void* arg)
{
    if(arg==NULL)
    {
        con->show("mousesens: %f ", progstate.scroll_speed);
        return;
    }

    progstate.scroll_speed = atof((char*)arg);
    con->show("mousesens: %f ", progstate.scroll_speed);
}

void set_grid_color(void* arg)
{
    if(arg==NULL)
    {
        con->show("grid color: %d %d %d", progstate.grid_color.r,progstate.grid_color.g,progstate.grid_color.b);
        return;
    }
   
    Uint8 color[3];
   
    string s((char*)arg);
    parseRGB(s, color);
   
    progstate.grid_color.r=color[0];
    progstate.grid_color.g=color[1];
    progstate.grid_color.b=color[2];
   
    con->show("grid color: %d %d %d", progstate.grid_color.r,progstate.grid_color.g,progstate.grid_color.b);
}

void set_grid_space(void* arg)
{
    if(arg==NULL)
    {
        con->show("grid space: %f", progstate.grid_spc);
        con->show("turn off with 0");
        return;
    }
   
    stringstream s((char*)arg);
    double spc=0;
    s >> spc;
    if(spc<0)spc=0;
   
    if(spc==0)progstate.draw_grid=false;
    else progstate.draw_grid=true;
   
    progstate.grid_spc=spc;
    con->show("grid space: %f", progstate.grid_spc);
}


void red(void* arg)
{
    char t[] = "220 25 25";
    set_lin_color(t);
}


void green(void* arg)
{
    char t[] = "25 220 25";
    set_lin_color(t);
}

void yellow(void* arg)
{
    char t[] = "220 220 25";
    set_lin_color(t);
}

void blue(void* arg)
{
    char t[] = "25 25 220";
    set_lin_color(t);
}

void cyan(void* arg)
{
    char t[] = "25 220 220";
    set_lin_color(t);
}

void magenta(void* arg)
{
    char t[] = "220 25 220";
    set_lin_color(t);
}


void line_thickness(void* arg)
{
    if(arg==NULL)
    {
        con->show("line thickness: %d", progstate.line_w);
        return;
    }
   
    progstate.line_w=atoi((char*)arg);
   
    con->show("line thickness: %d", progstate.line_w);
}





void draw_line_fancy(void* arg)
{
    if(arg==NULL)return;
   
    progstate.fancy_lines = (atoi((char*)arg) >0) ? true : false;
   
    con->show("fancy lines: %d", progstate.fancy_lines);
}

void draw_line_tension(void* arg)
{
    if(arg==NULL)return;
   
    progstate.draw_line_tension=atoi((char*)arg);
    con->show("line tension: %d", progstate.draw_line_tension);
}

void draw_line_settle(void* arg)
{
    if(arg==NULL)return;
   
    progstate.draw_line_settle = (atoi((char*)arg) >0) ? true : false;
    con->show("line settle: %d", progstate.draw_line_settle);
}

void state_tickall(void* arg)
{
    if(arg==NULL)return;
   
    progstate.setStateTickAll( (atoi((char*)arg) > 0));
    con->show("tick all states: %d", progstate.getStateTickAll());
}


void smplp(void* arg)
{
    if(arg==NULL)
    {
        con->show("sample path is: %s", progstate.getSamplePath().c_str());
        return;
    }
   
   
    string s((char*)arg);
   
    DASSERT(s.length()>0);
   
    if(s[s.length()-1] != '/')s.append("/");
   
    progstate.setSamplePath(s.c_str());
    con->show("sample path is: %s", progstate.getSamplePath().c_str());
}

void fullscreen(void* arg)
{
    master_res.do_toggle_fullscreen = true;
    //screen->toggleFullScreen();
}

void echocon(void* arg)
{
    if(arg==NULL)return;
    cout << (char*)arg <<endl;
}

void echo(void* arg)
{
    if(arg==NULL)return;
    con->show("%s", (char*)arg);
}


void reset(void* arg)
{
    //must save some runtime data before rest
    string pfix = cmd_caller_th->maincomp->getPrefix();

    delete cmd_caller_th->maincomp;
   
    cmd_caller_th->maincomp = new Main(0,0, master_res.screen,  fnt["std"]/*master_res.f*/,master_res.con);
    cmd_caller_th->maincomp->setPrefix(pfix);
    cmd_caller_th->midi.setEventCallbacks(cmd_caller_th->maincomp->getMIDIcallbacks());
   
    //FIXME
    if(progstate.getNChan()==2)
    {
        cmd_caller_th->maincomp->getGround().setStereo(true);
    }
   
    maincomp = cmd_caller_th->maincomp;
    master_res.maincomp=maincomp;
   
    //VERY important
    //SoundComp::resetSeqCount();
   
    //reset autosave timer
    //progstate.autotick=0;
   
    //remove last save so that this is impossible:
    //making module, save(def name set) , reset, thinks used load; now save-overwrites module
    maincomp->setLastLoad("");
   
    con->show("reset ok");
}

void reset_all(void* arg)
{
    for(uint i=0;i<w_threads.size();i++)
    {
        WorkThread* th = w_threads[i];
	
        //must save some runtime data before rest
        string pfix = th->maincomp->getPrefix();
	
        delete th->maincomp;
        th->maincomp = new Main(0,0,master_res.screen, fnt["std"]/*master_res.f*/, master_res.con);
        th->maincomp->setPrefix(pfix);
        th->midi.setEventCallbacks(th->maincomp->getMIDIcallbacks());
	
        //FIXME
        if(progstate.getNChan()==2)
        {
            th->maincomp->getGround().setStereo(true);
        }
	
        //reset autosave timer
        //progstate.autotick=0;
	
        //remove last save so that this is impossible:
        //making module, save(def name set) , reset, thinks used load; now save-overwrites module
        th->maincomp->setLastLoad("");
    }
   
    SoundComp::resetSeqCount();
   
    maincomp = cmd_caller_th->maincomp;
    master_res.maincomp=maincomp;
   
    con->show("reset_all ok");
}



void reset_allseqno(void* arg)
{
    int count = maincomp->resetSeqNums();
   
    //VERY important
    SoundComp::setSeqCount(count);
    con->show("sequence numbers reset, states invalided");
}


void dumpfile(void* arg)
{
    if(arg==NULL)
    {
        con->show("dump file: %s", progstate.getDumpName().c_str());
        return;
    }
   
    char* a = (char*)arg;
    con->show("setting dump file: %s", a);
    progstate.setDumpName(a);
}


void dumpsmp(void* arg)
{
    if(arg==NULL)return;
    int len = atoi((char*)arg);
   
    if(progstate.runningDump())
    {
        con->show("aboring already running dump");
        progstate.abortDump();
    }
   
    //FIXME: what if dump name changes during dump...(not to important)
    string fname = maincomp->getPrefix() + progstate.getDumpName();
    con->show("dumping wav %s (%d)", fname.c_str(), len);
   
    progstate.initDump(len, maincomp->getPrefix() + progstate.getDumpName());
}

void dumpabort(void* arg)
{
    if(!progstate.runningDump())
    {
        con->show("not running dump");
        return;
    }
   
    progstate.abortDump();
    con->show("dump aborted");
}


void dumpforce(void* arg)
{
    if(!progstate.runningDump())
    {
        con->show("not running dump");
        return;
    }
   
    int sec = progstate.dumpLenDone()/(progstate.getSampleFreq()*progstate.getNChan());
    int sec_o = (progstate.dumpLenDone()+progstate.dumpLenLeft())/(progstate.getSampleFreq()*progstate.getNChan());
   
    if(progstate.dumpToFile())con->show("premature dump of wav at: %d of %d sec: ok", sec, sec_o);
    else con->show("dump failed");
}

void dumpsec(void* arg)
{
    if(arg==NULL)
    {
        con->show("usage: dumpsec sec");
        return;
    }
   
    int len = atoi((char*)arg)*progstate.getSampleFreq()*progstate.getNChan();
   
    if(progstate.runningDump())
    {
        con->show("aboring already running dump");
        progstate.abortDump();
    }
   
    //thos can be needed, to be sure, do it:
   
    string fname = maincomp->getPrefix() + progstate.getDumpName();
    con->show("dumping wav %s (%d)", fname.c_str(), len);
   
    progstate.initDump(len, maincomp->getPrefix() + progstate.getDumpName());
   
}



void render(void* arg)
{
    if(arg==NULL)
    {
        con->show("usage: render sec");
        return;
    }
   
    //int len = atoi((char*)arg)*progstate.getSampleFreq()*progstate.getNChan();
   
    sync(NULL);
    string cmd("0");
    playondev((void*)cmd.c_str());
    dumpsec(arg);
    cmd.assign("1");
    process((void*)cmd.c_str());
}


void ls(void* arg)
{
    string append("");
   
    if(arg!=NULL)
    {
        append.assign((char*)arg);
    }
   
    string dirname = maincomp->getPrefix()+append;
   
    FileDir fd;
    bool exist = fd.read(dirname, false);
    if(!exist)
    {
        con->show("(E) no such dir: %s",dirname.c_str() );
        return;
    }
    vector<string> files;
    fd.getFiles(files);
   
   
    con->show(" %s",dirname.c_str());
    con->show(".");
    if(files.size()==0)return;
    for(uint i=0;i<files.size();i++)
    {
        con->show("  %s",files[i].c_str());
    }
}


void lsmod(void* arg)
{
    string append("");
   
    if(arg!=NULL)
    {
        append.assign((char*)arg);
    }
   
    string dirname = progstate.getModuleLoadPath()+append;
   
   
    FileDir fd;
    bool exist = fd.read(dirname, false);
    if(!exist)
    {
        con->show("(E) no such dir: %s",dirname.c_str() );
        return;
    }
    vector<string> files;
    fd.getFiles(files);
   
    if(files.size()==0)return;
   
    con->show("in module path:");
    con->show(" ");
   
    for(uint i=0;i<files.size();i++)
    {
        con->show("%s",files[i].c_str());
    }
}

void showfps(void* arg)
{
    if(arg==NULL)
    {
        con->show("show fps: %d", progstate.getShowFps());
        return;
    }
   
    progstate.setShowFps(atoi((char*)arg));
    con->show("show fps set: %d", progstate.getShowFps());
}


void showstats(void* arg)
{
    if(arg==NULL)
    {
        con->show("show stats: %d", progstate.getShowStats());
        return;
    }
   
    progstate.setShowStats(atoi((char*)arg));
    con->show("show stats set: %d", progstate.getShowStats());
}


void runfps(void* arg)
{
    if(arg==NULL)
    {
        con->show("run fps: %d", progstate.getFps());
        return;
    }
   
    progstate.setFps(atoi((char*)arg));
    if(progstate.getFps()<0)progstate.setFps(1);
   
   
    for(uint i=0;i<w_threads.size();i++)
    {
        WorkThread* th = w_threads[i];
        th->fps.setFPSLimit(progstate.getFps());
    }
   
    con->show("run fps set: %d", progstate.getFps());
   
}

void drawfps(void* arg)
{
    if(arg==NULL)
    {
        con->show("draw fps: %.0f", master_res.d_run_fps);
        return;
    }
   
    master_res.change_d_fps=true;
    master_res.d_run_fps = atoi((char*)arg);
    if(master_res.d_run_fps<0)master_res.d_run_fps=1;
   
    //let draw_th show("") when done
}


void exec(void* arg)
{
    if(arg==NULL)return;
   
    string name;
    name = maincomp->getPrefix() + string((char*)arg);
   
    ifstream fin(name.c_str());
   
    if(fin == NULL)
    {
        con->show("no such file:%s",(char*)arg);
        return;
    }
   
    string in;
   
    while(!fin.eof())
    {
        getline(fin,in);
        if(fin.bad())
        {
            DERROR("read error");
            break;
        }
	
        //strip comment
        in = in.substr(0, in.find_first_of("#"));
	
        if(in.length()==0)continue;
	
        /*con->addHistory((char*)in.c_str());
          if(!con->cmd.parseLine((char*)in.c_str()))
          {
          con->show("no such cmd");
          }
          con->show(" ");*/
	
        con->execCmd(in);
    }
   
    fin.close();
}

void rootexec(void* arg)
{
    if(arg==NULL)return;
   
    string name;
    name = string((char*)arg);
   
    ifstream fin(name.c_str());
   
    if(fin == NULL)
    {
        con->show("no such file:%s",(char*)arg);
        return;
    }
    if(!fin.good())
    {
        DERROR("read error");
        return;
    }
   
    string in;
   
    while(!fin.eof())
    {
        getline(fin,in);
        if(fin.bad())
        {
            DERROR("read error");
            break;
        }
	
        //strip comment
        in = in.substr(0, in.find_first_of("#"));
	
        if(in.length()==0)continue;
	
        con->execCmd(in);
    }
   
    fin.close();
}


void modulepath_sing(void* arg)
{
    if(arg==NULL)
    {
        con->show("module path: %s",progstate.getModuleLoadPath().c_str());
        return;
    }
   
    string s((char*)arg);
    DASSERT(s.length()>0);
    if(s[s.length()-1] != '/')s.append("/");
   
    DIR* dir = opendir(s.c_str());
    if(dir==NULL)
    {
        con->show("no such dir: %s",s.c_str());
        progstate.setModuleLoadPath("");
        return;
    }
   
    progstate.setModuleLoadPath(s.c_str());
    con->show("module path set: %s", progstate.getModuleLoadPath().c_str());
   
    maincomp->dirmenu->setDir(progstate.getModuleLoadPath());
}

void modulepath(void* arg)
{
    if(arg==NULL)
    {
        con->show("module path: %s",progstate.getModuleLoadPath().c_str());
        return;
    }
   
    string s((char*)arg);
    DASSERT(s.length()>0);
    if(s[s.length()-1] != '/')s.append("/");
   
    DIR* dir = opendir(s.c_str());
    if(dir==NULL)
    {
        con->show("no such dir: %s",s.c_str());
        progstate.setModuleLoadPath("");
        return;
    }
   
    progstate.setModuleLoadPath(s.c_str());
    con->show("module path set: %s", progstate.getModuleLoadPath().c_str());
   
    for(uint i=0;i<w_threads.size();i++)
    {
        WorkThread* th = w_threads[i];
        th->maincomp->dirmenu->setDir(progstate.getModuleLoadPath());
    }
}

void templatepath(void* arg)
{
    if(arg==NULL)
    {
        con->show("template path: %s",progstate.getTemplatePath().c_str());
        return;
    }
   
    string s((char*)arg);
    DASSERT(s.length()>0);
    if(s[s.length()-1] != '/')s.append("/");
   
    DIR* dir = opendir(s.c_str());
    if(dir==NULL)
    {
        con->show("no such dir: %s",s.c_str());
        progstate.setTemplatePath("");
        return;
    }
   
    progstate.setTemplatePath(s.c_str());
    con->show("template path set: %s", progstate.getTemplatePath().c_str());
   
    for(uint i=0;i<w_threads.size();i++)
    {
        WorkThread* th = w_threads[i];
        th->maincomp->tempmenu->setDir(progstate.getTemplatePath());
    }
}


void prefix_all(void* arg)
{
    if(arg==NULL)
    {
        con->show("prefix path: set prefix for all workspaces at once");
        return;
    }
   
    string s((char*)arg);
    DASSERT(s.length()>0);
    if(s[s.length()-1] != '/')s.append("/");
   
    DIR* dir = opendir(s.c_str());
    if(dir==NULL)
    {
        con->show("no such dir: %s",s.c_str());
        maincomp->setPrefix("./");
        return;
    }
   
    for(uint i=0;i<w_threads.size();i++)
    {
        WorkThread* th = w_threads[i];
        th->maincomp->setPrefix(s.c_str());
    }
   
    con->show("all prefixes set: %s", maincomp->getPrefix().c_str());
}

void prefix_startdir_all(void* arg)
{
    char* pwd = get_current_dir_name();
    string s(pwd);
    free(pwd);
   
    DASSERT(s.length()>0);
    if(s[s.length()-1] != '/')s.append("/");
   
    DIR* dir = opendir(s.c_str());
    if(dir==NULL)
    {
        con->show("no such dir: %s",s.c_str());
        maincomp->setPrefix("./");
        return;
    }
   
    for(uint i=0;i<w_threads.size();i++)
    {
        WorkThread* th = w_threads[i];
        th->maincomp->setPrefix(s.c_str());
    }
   
    con->show("all prefixes set: %s", maincomp->getPrefix().c_str());
}


void prefix(void* arg)
{
    if(arg==NULL)
    {
        con->show("prefix: %s",maincomp->getPrefix().c_str());
        return;
    }
   
    string s((char*)arg);
    DASSERT(s.length()>0);
    if(s[s.length()-1] != '/')s.append("/");
   
    DIR* dir = opendir(s.c_str());
    if(dir==NULL)
    {
        con->show("no such dir: %s",s.c_str());
        maincomp->setPrefix("./");
        return;
    }
   
    maincomp->setPrefix(s.c_str());
    con->show("prefix set: %s", maincomp->getPrefix().c_str());
}

void prefix_startdir(void* arg)
{
    char* pwd = get_current_dir_name();
    string s(pwd);
    free(pwd);
   
    DASSERT(s.length()>0);
    if(s[s.length()-1] != '/')s.append("/");
   
    DIR* dir = opendir(s.c_str());
    if(dir==NULL)
    {
        con->show("no such dir: %s",s.c_str());
        maincomp->setPrefix("./");
        return;
    }
   
    maincomp->setPrefix(s.c_str());
    con->show("prefix set: %s", maincomp->getPrefix().c_str());
}

void cd(void* arg)
{
    if(arg==NULL)
    {
        prefix_startdir(NULL);
        //con->show("prefix: %s", maincomp->getPrefix().c_str());
        return;
    }
   
    string s((char*)arg);
   
    DASSERT(s.length()>0);
    if(s[s.length()-1] != '/')s.append("/");
   
    string nstr = maincomp->getPrefix()+s;
   
    //remove ../'s taking a piece of prefix with
    size_t pos = nstr.find("../");
    while(pos!=string::npos)
    {
        size_t end = pos+3; //after "../"
	
        if(pos<2)break;
        size_t rem = nstr.rfind("/", pos-2);
	
        if(rem != string::npos)
        {
            rem+=1; // on '/'
	     
            nstr = nstr.substr(0, rem) + nstr.substr(end);
        }
        else
        {
            nstr = nstr.substr(end);
        }
        pos = nstr.find("../");
    }
   
    if(nstr.size()==0)nstr.assign("./");
   
    DIR* dir = opendir(nstr.c_str());
    if(dir==NULL)
    {
        con->show("no such dir: %s",nstr.c_str());
        //maincomp->setPrefix("./");
        return;
    }
   
    maincomp->setPrefix(nstr.c_str());
    con->show("prefix set: %s", maincomp->getPrefix().c_str());
}


void quit(void* arg)
{
    progstate.setRun(false);
   
    master_res.do_shutdown=true;
}

void help(void* arg)
{
    con->show("--------- Application HELP --------------------------------------------");
    con->show(" ");
    con->show("help_keys - show help for key");
    con->show("help_cmds - show help for commands");
    con->show(" ");
}

void help_cmds(void* arg)
{
    con->show("--------- Application COMMANDS --------------------------------------------");
    con->show(" ");
    con->show("quit         - quit application");
    con->show("sync         - sync_comps + sync_states");
    con->show("sync_comps   - sync components to same index for all threads");
    con->show("sync_states  - sync states to current workspace index for all threads");
    con->show(" ");
}

void help_keys(void* arg)
{
    con->show("--------- Application KEYS --------------------------------------------");
    con->show(" ");
    con->show(" ");
    con->show(" esc    - bring down console");
    con->show("   tab     - show all console commands");
    con->show("   ctrl-e  - end of line");
    con->show("   ctrl-a  - beg of line");
    con->show(" ");
    con->show(" lalt + 1,2,...,9   - change workspace");
    con->show(" ");
    con->show(" F1     - sync audio+state to current workspace (sync)");
    con->show(" F2     - sync audio to current workspace (sync_comps)");
    con->show(" F3     - processing(audio) on/off");
    con->show(" F4     - toggle playondev");
    con->show(" ");
    con->show(" F5     - toggle line fg/bg");
    con->show(" F6     - add one more of last added comp at mouse");
    con->show(" F7     - save(same as last save/load)");
    con->show(" F8     - rload(same as last save/load");
    con->show(" ");
    con->show(" F9     - toggle draw on");
    con->show(" F10    - toggle fullscreen");
    con->show(" F11    - toggle grab input");
    con->show(" F12    - quit");
    con->show(" ");
    con->show(" ");
    con->show(" ");
   
    con->show("-------- Workspace --------------------------------------------");
    con->show(" ");
    con->show(" ");
    con->show(" lctrl-r - delete component mouse is over");
    con->show(" ");
    con->show(" right click on workspace  - component menu");
    con->show(" l-alt + mouse move        - workspace move");
    con->show(" mwheel (l-shift)          - workspace move");
    con->show(" lshift + w,a,s,d          - workspace move");
    con->show(" ");
    con->show(" lsuper                    - move all comps");
    con->show(" ");
    con->show(" ");
    con->show("--------- State --------------------------------------------");

    con->show(" ");
    con->show(" ");
    con->show(" ");
    con->show("-------- Bench --------------------------------------------");
    con->show(" ");
    con->show(" ");
    con->show(" rsuper/menu key          - step through states");
    con->show(" ");
    con->show(" left click               - marks state slot and sets states");
    con->show("                             (all or just selected comp state - state_tickall)");
    con->show(" left click + lshift      - marks state slot");
    con->show(" middle click             - sets stopmarker");
    con->show(" middle click + lshift    - sets startmarker");
    con->show(" rctrl +pgup/pgdown       - move states for selected");
    con->show("                            + lshift moves for all");
    con->show(" ");
    con->show(" ");
    con->show(" lshift + lctrl-f         - delete all states for selected component");
    con->show(" lctrl-a                  - save all component's states at marker(needed for save)");
    con->show(" lctrl-s                  - save selected components current state at marker(needed for save)");
    con->show(" lctrl-d                  - delete selected components state at marker");
    con->show(" lctrl-e                  - erase state at marker for all components");
    con->show(" lctrl-c                  - copy selected component state");
    con->show(" lctrl-p                  - copy PARTIAL component state (not for all)");
    con->show("                            tracker: copy selected track)");
    con->show(" lctrl-v                  - paste to selected component & state slot");
    con->show(" right click              - sets copy/del marker");
    con->show(" right click + lshift     - sets copy/del marker");
    con->show(" lctrl + z                - c/d marker COPY to current for ALL");
    con->show(" lctrl + x                - c/d marker COPY to current for CURRENT");
    con->show(" lctrl + t                - c/d marker DEL to current for ALL");
    con->show(" lctrl + g                - c/d marker DEL to current for CURRENT");
    con->show(" ");
    con->show(" ");
    con->show(" ");
    con->show("-------- Tracker --------------------------------------------");
    con->show(" ");
    con->show(" ");
    con->show(" a to z                            - add note");
    con->show(" space                             - play/stop");
    con->show(" del                               - clear entry");
    con->show(" insert                            - move track entries down (from sel row)");
    con->show(" backspace                         - move track entries up (from sel row)");
    con->show(" ");
    con->show(" lctrl +        + (KP plus/minus)  - transpose sel track one note");
    con->show(" lctrl + lshift + (KP plus/minus)  - transpose sel track one octave");
    con->show(" ");
    con->show(" rctrl +        + (KP plus/minus)  - transpose all tracks one note");
    con->show(" rctrl + lshift + (KP plus/minus)  - transpose all tracks one octave");
    con->show(" ");
    con->show(" lctrl + del                       - clear track entries");
    con->show(" rctrl + del                       - clear all track's entries");
    con->show(" ");
    con->show(" pgup                              - jump 8 up");
    con->show(" pgdown                            - jump 8 down");
    con->show(" home                              - top of tracks");
    con->show(" end                               - bottom of tracks");
    con->show(" ");
    con->show(" ,                                 - octave down");
    con->show(" .                                 - octave up");
    con->show(" ");
    con->show(" 1,2                               - addstep down/up");
    con->show(" 3,4                               - delstep down/up");
    con->show(" ");
    con->show("---- in ctrl column:");
    con->show(" ");
    con->show(" q,w,e                             - set 0.0, 0.5, 1.1");
    con->show(" ");
    con->show("---- in ctrl enter mode:");
    con->show(" ");
    con->show(" up/down                           - add/sub 0.1");
    con->show(" right/left                        - add/sub 0.01");
   
}

void set_force_load(void* arg)
{
    if(arg==NULL)
    {
        con->show("force load: %d" , progstate.getForceLoad());
        return;
    }
   
    string in((char*)arg);
    istringstream ss(in);
    bool v=0;
    ss >> v;
   
    progstate.setForceLoad(v);
    con->show("force load: %d" , progstate.getForceLoad());
}


void load(void* arg)
{
    //automatically assigns last load if arg empty
    string name;
    string fname(maincomp->getPrefix());
   
    if(arg!=NULL)
    {
        name.assign((char*)arg);
    }
    else
    {
        string ll = maincomp->getLastLoad();
        if(ll.empty())return;
        name= ll;
    }
    fname.append(name);
    fname += progstate.getPostfix();
   
    con->show("load: %s",fname.c_str());
   
    m_err err = maincomp->loadProgram(fname, progstate.getForceLoad());
    if(err!=OK)
    {
        if(err==FILE_NOTFOUND_ERR)con->show("no such file:%s",fname.c_str()); 
        if(err==COMP_FAIL_ERR)con->show("comp(s) load fail");
        if(err==FILE_CORRUPT_ERR)con->show("file corrupt: %s",fname.c_str());;
        if(err==CON_FAIL_ERR)con->show("(E) line connection(s) failed");
        if(err==FILE_IO_ERR)con->show("(E) file io error");
        if(err==ERR)con->show("(W) something wrong happened");
    }
   
    maincomp->setLastLoad(name);
}


//loads without appending anything;or using lastload
//i.e complete path needed
void load_abs(void* arg)
{
    //automatically assigns last load if arg empty
    string name;
    if(arg!=NULL)
    {
        name.assign((char*)arg);
    }
    else
    {
        con->show("load_abs, needs full path");
        return;
    }
   
    con->show("load_abs: %s",name.c_str());
   
    m_err err = maincomp->loadProgram(name, progstate.getForceLoad());
    if(err!=OK)
    {
        if(err==FILE_NOTFOUND_ERR)con->show("no such file:%s",name.c_str()); 
        if(err==COMP_FAIL_ERR)con->show("comp(s) load fail");
        if(err==FILE_CORRUPT_ERR)con->show("file corrupt: %s",name.c_str());;
        if(err==CON_FAIL_ERR)con->show("(E) line connection(s) failed");
        if(err==FILE_IO_ERR)con->show("(E) file io error");
        if(err==ERR)con->show("(W) something wrong happened");
    }
}



//reset & load
void rload(void* arg)
{
    if(arg==0)
    {
        DASSERT(cmd_caller_th!=NULL);
	
        string s = cmd_caller_th->maincomp->getLastLoad();
	
        if(s.empty())
        {
            return;
        }
	
        char* c = strdup(s.c_str());
        reset(NULL);
        load(c);
        free(c);
    }
    else
    {
        reset(NULL);
        load(arg);
    }
   
}

void loadmodule(void* arg)
{
    if(arg==NULL)
    {
        con->show("loadmodule filename");
        con->show("searches: pwd then modulepath");
        return;
    }
   
   
    string name;
    name.assign((char*)arg);
    name+=progstate.getPostfix();
   
   
    //1. current dir
   
    //2. module path
   
    string fname = name;
   
    con->show("loadmodule search-one: %s", (maincomp->getPrefix()+fname).c_str() );
   
    //FIXME:maybe fix setCurrentLoadVersion()
   
    SoundComp* bridge;
    m_err err = loadModuleBridge(fname, maincomp->getPrefix(), maincomp->f, &bridge, maincomp->getMIDIcall(), maincomp->getGround());
   
    if(err==FILE_NOTFOUND_ERR)
    {
        if(progstate.getModuleLoadPath().size()==0)
        {
            con->show("module path not set");
            return;
        }
	
        fname = name;
	
        con->show("loadmodule search-two: %s",  (progstate.getModuleLoadPath()+fname).c_str());
	
        err = loadModuleBridge(fname, progstate.getModuleLoadPath(), maincomp->f, &bridge, maincomp->getMIDIcall(), maincomp->getGround());
    }
   
    if(err!=OK)
    {
        if(err==FILE_NOTFOUND_ERR)
        {
            con->show("(W) file missing:%s", fname.c_str());
            return;
        }
	
        if(err==BRIDGE_NOTFOUND_ERR)
        {
            con->show("(E) MODULEBRIDGE not found, abort");
            return;
        }
	
        if(err==FILE_IO_ERR)
        {
            con->show("(E) file error, abort");
            return;
        }
        if(err==SOME_COMP_FAIL)
        {
            con->show("(W) some comp(s) failed to load");
        }
        if(err==SOME_STATE_FAIL)
        {
            con->show("(W) some state(s) failed to load");
        }
        if(err==FILE_CORRUPT_ERR)
        {
            con->show("(E) file corrupted, failed to load");
            return;
        }
	
    }
   
    DASSERT(bridge!=NULL); 
    //maincomp->addComp(bridge, bridge->pos.x, bridge->pos.y);
    maincomp->addComp(bridge, 100, 100);
}


//only check module path(good to skip pwd)
void loadmodule_mp_only(void* arg)
{
    if(arg==NULL)
    {
        con->show("loadmodule_mp_only filename");
        con->show("searches: modulepath");
        return;
    }
   
    string name;
    name.assign((char*)arg);
    name+=progstate.getPostfix();
   
    //1. module path
      
    if(progstate.getModuleLoadPath().size()==0)
    {
        con->show("module path not set");
        return;
    }
   
    string fname = name;
   
    con->show("loadmodule search-one: %s",  (progstate.getModuleLoadPath()+fname).c_str());
   
    SoundComp* bridge;
    m_err err = loadModuleBridge(fname, progstate.getModuleLoadPath(), maincomp->f, &bridge, maincomp->getMIDIcall(), maincomp->getGround());
   
   
    if(err!=OK)
    {
        if(err==FILE_NOTFOUND_ERR)
        {
            con->show("(W) file missing:%s", fname.c_str());
            return;
        }
	
        if(err==BRIDGE_NOTFOUND_ERR)
        {
            con->show("(E) MODULEBRIDGE not found, abort");
            return;
        }
	
        if(err==FILE_IO_ERR)
        {
            con->show("(E) file error, abort");
            return;
        }
        if(err==SOME_COMP_FAIL)
        {
            con->show("(W) some comp(s) failed to load");
        }
        if(err==SOME_STATE_FAIL)
        {
            con->show("(W) some state(s) failed to load");
        }
        if(err==FILE_CORRUPT_ERR)
        {
            con->show("(E) file corrupted, failed to load");
            return;
        }
	
    }
   
    DASSERT(bridge!=NULL); 
    //maincomp->addComp(bridge, bridge->pos.x, bridge->pos.y);
    maincomp->addComp(bridge, 100, 100);
}


void save(void* arg)
{
    string name;
    if(arg!=NULL)
    {
        name.assign((char*)arg);
        size_t sz = name.find_first_not_of(" ");
        if(sz==string::npos)
        {
            string ll = maincomp->getLastLoad();
            if(ll.empty())return;
            name= ll;
        }
        else
        {
            name = name.substr(sz);
            maincomp->setLastLoad(name);
        }
    }
    else
    {
        string ll = maincomp->getLastLoad();
        if(ll.empty())
        {
            con->show("no arg/last load");
            return;
        }
	
        name= ll;
    }
   
   
    VERBOSE1(cout << "save: " << name << endl;);
   
   
    string fname(maincomp->getPrefix());
    fname.append(name);
    fname.append(progstate.getPostfix());
   
    con->show("saving: %s",fname.c_str());
    m_err err = maincomp->saveProgram(fname);
    if(err!=OK)con->show("(W) save failed");
    else con->show("ok");
}

/*special command*/
/*void autosave(void* arg=NULL)
  {
  string name = progstate.autoname;
  * 
  VERBOSE1(cout << "auto save: " << name << endl;);
  * 
  string fname(maincomp->getPrefix());
  fname.append(name);
  fname.append(progstate.getPostfix());
  * 
  //con->show("save: %s",fname.c_str());
  * 
  m_err err = maincomp->saveProgram(fname);
  * 
  if(err!=OK)con->show("(W) autosave failed");
  }*/




/*******/
/*SOUND*/
/*******/

void sndinfo(void* arg)
{
    con->show("samplerate: %d (req restart)",progstate.getSampleRate());
    con->show("internal bits: %d (req recomp)",sizeof(smp_t)*8);
    con->show("dump bits: %d (req recomp)",sizeof(short)*8);
    con->show(" ");
    con->show("num channels: %d",progstate.getNChan());
    con->show("dev_nsmp:%d", progstate.getDevNSmp());
   
    con->show("sample generation length: %d", progstate.getSampleGenLen());
    con->show("sample generation buffer is:%d", progstate.getSampleGenBuffer());
   
    con->show("run fps:%d", (int)progstate.getFps());
   
}

void n_chan(void* arg)
{
    if(arg==NULL)
    {
        con->show("num channels: %d",progstate.getNChan());
        return;
    }
   
    uint ch = atoi((char*)arg);
    if(ch>2)ch=2;
   
    progstate.setNChan(ch);
   
    con->show("resetting num channels, restarting device...");
   
    //reopen device
    shared_audio.freeAudioDev();
    shared_audio.initAudioDev(); //uses progstate, FIXME: check returnt
   
    for(uint i=0;i<w_threads.size();i++)
    {
        WorkThread* th = w_threads[i];
        if(progstate.getNChan()==1)th->maincomp->getGround().setStereo(false);
        else th->maincomp->getGround().setStereo(true);
	
        if(th->maincomp->play!=NULL)th->maincomp->play->setNChan(progstate.getNChan());
    }
   
    con->show("num channels: %d",progstate.getNChan());
}

void dev_nsmp(void* arg)
{
    if(arg==NULL)
    {
        con->show("dev_nsmp:%d", progstate.getDevNSmp());
        return;
    }
   
   
    con->show("resetting smpbuff, restart device...");
   
   
    progstate.setDevNSmp(atoi((char*)arg));
    if(progstate.getDevNSmp()<0)
    {
        con->show("(E) argument neg");
        progstate.setDevNSmp(1024);
    }
   
    shared_audio.freeAudioDev();
    int ret = shared_audio.initAudioDev();
   
    if(ret==-1)
    {
        //FIXME: not dropdead here!
        DERROR("audio failed to reopen");
    }
   
    //FIXME: if there is an error progstate is not updated(fix in SharedAudio
    con->show("dev_smp:%d", progstate.getDevNSmp());
}


void gen_len(void* arg)
{
    if(arg==NULL)
    {
        con->show("sample generation length: %d", progstate.getSampleGenLen());
        return;
    }
    con->show("resetting sample generation, clearing buffers...");
   
    progstate.setSampleGenLen(atoi((char*)arg));
    con->show("sample generation length: %d", progstate.getSampleGenLen());
   
    for(uint i=0;i<w_threads.size();i++)
    {
        WorkThread* th = w_threads[i];
        th->maincomp->makeSampleQueue(progstate.getSampleGenBuffer(), progstate.getSampleGenLen());
    }
   
    shared_audio.free(),
        shared_audio.build(w_threads.size(), progstate.getSampleGenBuffer(), progstate.getSampleGenLen());
}



void gen_buff(void* arg)
{
    if(arg==NULL)
    {
        con->show("sample generation buffer is:%d", progstate.getSampleGenBuffer());
        return;
    }
    con->show("resetting sample generation, clearing buffers...");
   
   
    progstate.setSampleGenBuffer(atoi((char*)arg));
    if(progstate.getSampleGenBuffer() < 0)
    {
        con->show("(E) argument neg");
        progstate.setSampleGenBuffer(4);
    }
   
    for(uint i=0;i<w_threads.size();i++)
    {
        WorkThread* th = w_threads[i];
        th->maincomp->makeSampleQueue(progstate.getSampleGenBuffer(), progstate.getSampleGenLen());
    }
   
    shared_audio.free(),
        shared_audio.build(w_threads.size(), progstate.getSampleGenBuffer(), progstate.getSampleGenLen());
   
   
    con->show("sample generation buffer is:%d", progstate.getSampleGenBuffer());
}

/***********/
/*POLY LOAD*/
/***********/


GBridge* hlp_loadBridge(const string& fname)   
{
    SoundComp* bridge;
   
    m_err err = loadModuleBridge(fname, maincomp->getPrefix(), maincomp->f, &bridge, maincomp->getMIDIcall(), maincomp->getGround());
   
    if(err==FILE_NOTFOUND_ERR)
    {
        if(progstate.getModuleLoadPath().size()==0)
        {
            con->show("module path not set");
            return NULL;
        }
	
        err = loadModuleBridge(fname, progstate.getModuleLoadPath(), maincomp->f, &bridge, maincomp->getMIDIcall(), maincomp->getGround());
    }
   
    if(err!=OK)
    {
        if(err==FILE_NOTFOUND_ERR)
        {
            con->show("(W) file missing:%s", fname.c_str());
            return NULL;
        }
        if(err==BRIDGE_NOTFOUND_ERR)
        {
            con->show("(E) MODULEBRIDGE not found, abort");
            return NULL;
        }
        if(err==FILE_IO_ERR)
        {
            con->show("(E) file error, abort");
            return NULL;
        }
        if(err==SOME_COMP_FAIL)
        {
            con->show("(W) some comp(s) failed to load");
        }
        if(err==SOME_STATE_FAIL)
        {
            con->show("(W) some state(s) failed to load");
        }
        if(err==FILE_CORRUPT_ERR)
        {
            con->show("(E) file corrupted, failed to load");
            return NULL;
        }
	
    }
   
    return (GBridge*)bridge;
}

bool hlp_addCon(Connection* connection, GMIDIkey* midi, GBridge* bridge, const string& name, uint i)
{
    int id = midi->outputGiverId(name, i);
    if(id<0)
    {
        con->show(name + " ouput taker not found on midikey");
        return false;
    }
   
    OutputGiver* og = midi->getOutputGiver(id);
    if(og==NULL)
    {
        con->show(name + " output giver does not exist, abort");
        return false;
    }
   
    id = bridge->inputTakerId(name, 0);
    if(id<0)
    {
        //con->show(name << "input taker not found on bridge");
        return true;//false;
    }
   
    InputTaker*  it = bridge->getInputTaker(id);
    if(it==NULL)
    {
        con->show(name + "input taker does not exist, abort");
        return false;
    }
   
    connection->makeCon(og, it);
    return true;
}


void loadmodulepoly(void* arg)
{
    if(arg==NULL)
    {
        con->show("loadmodulepoly filename numpoly");
        con->show("numpoly defaults to: 4");
        con->show("bridge tags: hz, gate, vel, pwheel, out");
        return;
    }
   
    string argstr((char*)arg);
    istringstream ss(argstr);
   
    string name;
    ss >> name;
    if(name.size()==0)
    {
        return;
    }
    name+=progstate.getPostfix();
   
    uint npoly=0;
    ss >> npoly;
    if(npoly==0 || npoly>16)npoly=4;
   
    //1. current dir
   
    //2. module path
   
    string fname = name;
   
    int xoff=600;
    int yoff=100;
   
    maincomp->addComp(G_MIDIKEY, xoff, yoff);
    GMIDIkey* midi = (GMIDIkey*)maincomp->getSelectedComp(); //(GMIDIkey*)SCFactory(G_MIDIKEY, xoff, yoff, maincomp->f, maincomp->getGround());
    if(midi==NULL)
    {
        con->show("comp create failed");
        return;
    }
    midi->setNPolyUsed(npoly);
    //maincomp->midicall.addKey(midi);
    //maincomp->addComp(midi, xoff, yoff);
   
    yoff+=50;
    xoff+=midi->getPos().w+50;
   
   
   
    uint nmix = 1+(npoly-1)/(GMix::NUM_IN);
    GMix** mixers = new GMix*[nmix];
    for(uint i=0;i<nmix;i++)
    {
        mixers[i] = (GMix*)SCFactory(G_MIX, xoff, yoff, maincomp->f, maincomp->getGround());
        //check return
	
        maincomp->addComp(mixers[i], xoff, yoff);
        xoff+=mixers[i]->getPos().w+20;
    }
   
    Connection* connection = maincomp->getConnection();
   
   
    xoff=100;
    yoff=150+midi->getPos().h;
   
    uint added_mix=0;
    bool ok=true;
    for(uint i=0;i<npoly;i++)
    {
        GBridge* bridge;
        bridge = hlp_loadBridge(fname);
	
        if(bridge==NULL)
        {
            ok=false;
            break;
        }
	
        maincomp->addComp(bridge, xoff, yoff);
	
        xoff+=bridge->getPos().w+20;
	
        //FIXME: increase yoff, xoff(reset) if xoff>ws_width
	
	
        /********/
        /*INPUTS*/
        /********/
	
        //tags: hz, gate, vel, out, pwheel
	
	
        //FIXME: do something about 'ok'
        bool ok;
        ok = hlp_addCon(connection, midi, bridge, "hz", i);
        ok = hlp_addCon(connection, midi, bridge, "gate", i);
        ok = hlp_addCon(connection, midi, bridge, "vel", i);
        //fix: pwheel
	
	
        /*********/
        /*OUTPUTS*/
        /*********/
	
        string name("out");
        int id = bridge->outputGiverId(name, i);
        if(id>=0)
        {
            OutputGiver* og = bridge->getOutputGiver(id);
            if(og==NULL)
            {
                con->show(name + " output giver does not exist, abort");
                ok=false;
                break;
            }
	     
            uint id = added_mix%GMix::NUM_IN;
            uint cmixer=added_mix/GMix::NUM_IN;
            InputTaker*  it = mixers[cmixer]->getInputTaker(id);
            if(it==NULL)
            {
                con->show("mixer input taker does not exist, abort");
                ok=false;
                break;
            }
	     
            added_mix++;
	     
            connection->makeCon(og, it);
        }
        else
        {
            //no output on bridge
            break;
        }
	
    }
   
    if(!ok)
    {
        //no comp clean up if fail
        con->show("operation failed");
        return;
    }
   
   
    con->show("ok");
}



#endif
