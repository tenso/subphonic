#include "progstate.h"

#include "../util/debug.h"

ProgramState ProgramState::state;


ProgramState::ProgramState()
{
    setGrabInput(true);
    setDrawLinesFG(true);
   
    setDataPrefix("./data/");
   
    setDevFmt(AUDIO_S16);
   
    show_have_state=true;   
   
    force_load=false;
   
    autoname.assign("autosave");
    autosave=true;
    autotime=180;
    autotick=0; //must be zero
   
    setWSxmax(2000);
    setWSymax(2000);
    setWSxmin(0);
    setWSymin(0);
   
    scroll_speed=18;
   
    grid_color.r = 140;
    grid_color.g = 140;
    grid_color.b = 140;
    draw_grid=true;
    grid_spc=1280/(double)2;
   
    bg_color.r=128;
    bg_color.g=128;
    bg_color.b=128;
   
    setDevNSmp(1024);
    wavdump=NULL;
    dumplen=0;
    setDumpName("unnamed");
    //setPrefix("./");
    setPostfix(".kwf");
   
    run=true;
    showfps=false;
   
    setSamplePath(string("./"));
   
    setSampleGenLen(1024);
    setSampleGenBuffer(4);
   
    statetickall=0;
    ison=false;
    //buffsamp=3;
    samplefreq=44100;
    fancy_lines=true;
   
    draw_line_tension=10.0;
    draw_line_settle=true;
   
  
    setLineColor(0,255,0);


    line_w=2;
   
    fps=50;
    //showdumpsec=true;
   
   
    def_graph_color.r=47; 
    def_graph_color.g=243;
    def_graph_color.b=255;
   
    setCurrentLoadVersion(FILEFORMAT_VERSION);
   
    show_stats=false;
   
    setPlayOnDevice(true);
}

bool ProgramState::getPlayOnDevice()
{
    return playdev;
}

void ProgramState::setPlayOnDevice(bool v)
{
    playdev=v;
}


void ProgramState::setLineColor(const SDL_Color& c)
{
    line_color = c;
}

void ProgramState::setLineColor(Uint8 r, Uint8 g, Uint8 b)
{
    line_color.r=r;
    line_color.g=g;
    line_color.b=b;
}

SDL_Color ProgramState::getLineColor() const
{
    return line_color;
}


ProgramState::~ProgramState()
{
    delete wavdump;
}

bool ProgramState::getGrabInput() const
{
    return grab_in;
}

void ProgramState::setGrabInput(bool v)
{
    grab_in=v;
}


void ProgramState::setDrawLinesFG(bool v)
{
    d_line_fg=v;
}

bool ProgramState::getDrawLinesFG() const
{
    return d_line_fg;
}


string ProgramState::getModuleLoadPath() const
{
    return mod_load_path;
}

void ProgramState::setModuleLoadPath(const string& s)
{
    mod_load_path=s;
}


ProgramState& ProgramState::instance()
{
    return state;
}


int ProgramState::getWSxmax() const
{
    return ws_max_x;
}

int ProgramState::getWSymax() const
{
    return ws_max_y;
}

int ProgramState::getWSxmin() const
{
    return ws_min_x;
}

int ProgramState::getWSymin() const
{
    return ws_min_y;
}

void ProgramState::setWSxmax(int v)
{
    ws_max_x=v;
}

void ProgramState::setWSymax(int v)
{
    ws_max_y=v;
}

void ProgramState::setWSxmin(int v)
{
    ws_min_x=v;
}

void ProgramState::setWSymin(int v)
{
    ws_min_y=v;
}


void ProgramState::setDevFmt(Uint16 fmt)
{
    this->dev_fmt=dev_fmt;
}

Uint16 ProgramState::getDevFmt() const
{
    return dev_fmt;
}

void ProgramState::setShowHaveState(bool v)
{
    show_have_state=v;
}

bool ProgramState::getShowHaveState() const
{
    return show_have_state;
}


void ProgramState::setDataPrefix(const string& s)
{
    if(s.length()==0)return;
   
    data_prefix=s;
   
    if(data_prefix.at(data_prefix.length()-1) != '/')
    {
        data_prefix= data_prefix + "/";
    }
   
}


string ProgramState::getDataPrefix() const
{
    return data_prefix;
}


bool ProgramState::getShowStats() const
{
    return show_stats;
}

void ProgramState::setShowStats(bool v)
{
    show_stats=v;
}


void ProgramState::setCurrentLoadVersion(int ver)
{
    c_load_ver=ver;
}

int ProgramState::getCurrentLoadVersion() const
{
    //loadmodule() run: does not set this
    //DASSERT(c_load_ver!=-1);
    return c_load_ver;
}


bool ProgramState::getForceLoad() const
{
    return force_load;
}

void  ProgramState::setForceLoad(bool v)
{
    force_load=v;
}

/*void ProgramState::setLastLoad(const string& s)
  {
  lastload=s;
  }

  string ProgramState::getLastLoad() const
  {
  return lastload;
  }*/


/*void ProgramState::setPrefix(const string& n)
  {
  if(n.length()==0)return;
   
  prefix=n;
   
  if(prefix.at(prefix.length()-1) != '/')
  {
  prefix = prefix + "/";
  }
  }

  string ProgramState::getPrefix() const
  {
  return prefix;
  }*/


void ProgramState::setPostfix(const string& n)
{
    postfix=n;
}

string ProgramState::getPostfix() const
{
    return postfix;
}


void ProgramState::setRun(bool b)
{
    run=b;
}
bool ProgramState::getRun() const
{
    return run;
}

//dump file


/*void ProgramState::showDumpSec(bool v)
  {
  showdumpsec=v;
  }
  bool ProgramState::showDumpSec() const
  {
  return showdumpsec;
  }*/


void ProgramState::setDumpName(const string& s)
{
    dumpname=s;
}

string ProgramState::getDumpName() const
{
    return dumpname;
}

int ProgramState::dumpLenLeft() const
{
    return dumplen;
}

int ProgramState::dumpLenDone() const 
{
    return dumpoff;
}


void ProgramState::initDump(int len, const string& to_file) //samples
{
    if(runningDump())
    {
        ERROR("already running dump (use abortDump?)");
        return;
    }
   
    if(to_file.size()==0)
    {
        ERROR("dump name length 0, using: out.wav");
        running_dump_name.assign("out.wav");
    }
    else running_dump_name=to_file;

   
    dumplen=len;
    wavdump = new Sample(dumplen);
    wavdump->zero();
    dumpoff=0;
   
}

bool ProgramState::runningDump() const
{
    return wavdump!=NULL;
}

bool ProgramState::dumpReady() const
{
    if(!runningDump())
    {
        DERROR("not running dump");
        return false;
    }
    DASSERT(wavdump!=NULL);
   
    return dumplen==0;
}

bool ProgramState::appendToDump(const Sample* s)
{
    if(!runningDump())
    {
        DERROR("not running dump");
        return false;
    }
   
    int rest = s->getLen();
   
    if(rest>dumplen)rest=dumplen;
   
    bool ok = wavdump->append(s, dumpoff, rest);
    DASSERT(ok);
   
    dumpoff+=rest;
    dumplen-=rest;
    if(dumplen<0)DERROR("underun");

    return true;
}


bool ProgramState::dumpToFile()
{
    if(!runningDump())
    {
        DERROR("not running dump");
        return false;
    }
   
    Wav w;
    w.make(wavdump->getBytes(),dumpoff*2 /*bytes*/,getSampleRate(),nchan,16);
   
    string fname = running_dump_name;
    DASSERT(fname.size()>0);
   
    w.save(fname,true);
    cout << "wrote: " << fname.c_str() << endl;
    w.del();
   
    delete wavdump;
    wavdump=NULL;
    dumplen=0;
   
    return true;
   
}


void ProgramState::abortDump()
{
    if(!runningDump())return;
   
    delete wavdump;
    wavdump=NULL;
    dumplen=0;
}



void ProgramState::setSamplePath(const string& p)
{
    samplepath = p;
}

string ProgramState::getSamplePath() const
{
    return samplepath;
}

int ProgramState::getDevNSmp() const
{
    return dev_nsmp;
}

void ProgramState::setDevNSmp(int n)
{
    dev_nsmp=n;
}


void ProgramState::setNChan(int val)
{
    nchan=val;
}
int ProgramState::getNChan() const
{
    return nchan;
}

void ProgramState::setSampleGenLen(int val)
{
    sample_generate_len=val;
}

int ProgramState::getSampleGenLen() const
{
    return sample_generate_len;
}

void ProgramState::setSampleGenBuffer(int val)
{
    sample_generate_buff=val;
}

int ProgramState::getSampleGenBuffer() const
{
    return sample_generate_buff;
}



void ProgramState::setShowFps(bool val)
{
    showfps=val;
}
int ProgramState::getShowFps() const
{
    return showfps;
}

bool ProgramState::getStateTickAll() const
{
    return statetickall;
}
void ProgramState::setStateTickAll(bool v)
{
    statetickall=v;
}

void ProgramState::setOn(bool v)
{
    ison=v;
}

bool ProgramState::getOn() const
{
    return ison;
}
/*
  int ProgramState::getBufferedSamples() const
  {
  return buffsamp;
  }

  void ProgramState::setBufferedSamples(int v)
  {
  buffsamp=v;
  }*/

int ProgramState::getSampleFreq() const
{
    return samplefreq;
}
void ProgramState::setSampleFreq(int v)
{
    samplefreq=v;
}

int ProgramState::getSampleRate() const
{
    return samplefreq;
}
void ProgramState::setSampleRate(int v)
{
    samplefreq=v;
}


int ProgramState::getFps() const
{
    return fps;
}
void ProgramState::setFps(int f)
{
    fps=f;
}


SDL_Color ProgramState::getGraphColor() const
{
    return def_graph_color;
}

void ProgramState::setGraphColor(const SDL_Color& c)
{
    def_graph_color=c;
}

void ProgramState::setGraphColor(Uint8 r, Uint8 g, Uint8 b)
{
    def_graph_color.r=r;
    def_graph_color.g=g;
    def_graph_color.b=b;
}

string ProgramState::getTemplatePath() const
{
    return template_path;
}

void ProgramState::setTemplatePath(const string& s)
{
    template_path=s;
}
