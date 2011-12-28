#ifndef PROGSTATE_H
#define PROGSTATE_H

#include <SDL.h>

#include "../util/debug.h"
#include "../sig/wav.h"

#include <string>
using namespace std;
using namespace spl;

#include "../main/config.h"

//FIXME: move all data to private!

/*SINGLETON*/

class ProgramState
{
  public:
   
    static ProgramState& instance();
   
   
    /******/
    /*MISC*/
    /******/
   
    bool getGrabInput() const;
    void setGrabInput(bool v);
   
    void setRun(bool b);
    bool getRun() const;
    void setOn(bool v);
    bool getOn() const;
   
    bool getStateTickAll() const;
    void setStateTickAll(bool v);
   
    //ACTUAL: not only works for file loads: set to current version even if no load used; so
    //g_comps can use it to determine how to load state
    void setCurrentLoadVersion(int ver);
    int getCurrentLoadVersion() const;
   
   
    /***********/
    /*WORKSPACE*/
    /***********/
   
    int getWSxmax() const;
    int getWSymax() const;
    int getWSxmin() const;
    int getWSymin() const;
    void setWSxmax(int v);
    void setWSymax(int v);
    void setWSxmin(int v);
    void setWSymin(int v);
   
   
    /***********/
    /*AUDIO-GEN*/
    /***********/
   
    //def=AUDIO_S16
    void setDevFmt(Uint16 fmt);
    Uint16 getDevFmt() const;
    int getDevNSmp() const;
    void setDevNSmp(int);
   
    void setNChan(int val);
    int getNChan() const;
   
    void setSampleGenLen(int val);
    int getSampleGenLen() const;
    void setSampleGenBuffer(int val);
    int getSampleGenBuffer() const;
   
    /*int getBufferedSamples() const;
      void setBufferedSamples(int v);*/
   
    //equals
    int getSampleFreq() const;
    void setSampleFreq(int v);
    int getSampleRate() const;
    void setSampleRate(int v);
   
    //def:true
    bool getPlayOnDevice();
    void setPlayOnDevice(bool v);
   
    /******/
    /*DRAW*/
    /******/
      
    int getFps() const;
    void setFps(int f);
   
    void setShowHaveState(bool v);
    bool getShowHaveState() const;
    void setShowFps(bool val);
    int getShowFps() const;
    bool getShowStats() const;
    void setShowStats(bool v);
   
    SDL_Color getGraphColor() const;
    void setGraphColor(const SDL_Color& c);
    void setGraphColor(Uint8 r, Uint8 g, Uint8 b);
   
    void setDrawLinesFG(bool v);
    bool getDrawLinesFG() const;
   
    void setLineColor(const SDL_Color& c);
    void setLineColor(Uint8 r, Uint8 g, Uint8 b);
    SDL_Color getLineColor() const;
   
    /**************/
    /*FILE LOADING*/
    /**************/
   
    string getModuleLoadPath() const;
    void setModuleLoadPath(const string& s);
    string getTemplatePath() const;
    void setTemplatePath(const string& s);
   
    bool getForceLoad() const;
    void setForceLoad(bool v);
    /*void setLastLoad(const string& s);
      string getLastLoad() const;*/
    void setDataPrefix(const string& s);
    string getDataPrefix() const;
    /*void setPrefix(const string& n);
      string getPrefix() const;*/
    void setPostfix(const string& n);
    string getPostfix() const;
   
    void setSamplePath(const string& p);
    string getSamplePath() const;
   
   
    /**************/
    /*FILE DUMPING*/
    /**************/
   
    /*void showDumpSec(bool v);
      bool showDumpSec() const;*/
    void setDumpName(const string& s);   //FIXME: name to filename
    string getDumpName() const;
    int dumpLenLeft() const;
    int dumpLenDone() const;
    void initDump(int len, const string& to_file); //samples
    bool runningDump() const;
    bool dumpReady() const;
    bool appendToDump(const Sample* s);
    bool dumpToFile(); //returns true if OK
    void abortDump();
   
   
   
    //FIXME: MOVE!
    bool fancy_lines;
    double draw_line_tension;
    bool draw_line_settle;
    int line_w;
    SDL_Color bg_color;
    double scroll_speed;
    SDL_Color grid_color;
    bool draw_grid;
    double grid_spc;
    string autoname;
    uint autotick;
    uint autotime;
    bool autosave;
   
  private:
   
    static ProgramState state;
   
    //disallow
    ProgramState(ProgramState&);
    ProgramState& operator=(ProgramState&);
   
    ProgramState();
    ~ProgramState();
   
   
    /******/
    /*DATA*/
    /******/
    Uint16 dev_fmt;
    int dev_nsmp;
   
    SDL_Color line_color;
    int samplefreq;
    bool grab_in;
    bool d_line_fg;
   
    string mod_load_path;
    string template_path;
   
    int ws_min_x;
    int ws_min_y;
    int ws_max_x;
    int ws_max_y;
   

   
    bool show_have_state;
   
    string data_prefix;
   
    bool show_stats;
   
    int c_load_ver;
   
    SDL_Color def_graph_color;
   
    bool force_load;
    //string lastload;
    //bool showdumpsec;
    bool statetickall;
    int buffsamp;
    int nchan;
    bool run;
    string samplepath;
    string dumpname;
    int dumpoff;
    int dumplen;
    Sample* wavdump;
   
    //string prefix;
    string postfix;
   
    bool showfps;
    bool ison;
    int sample_generate_len;
    int sample_generate_buff;
    int fps;

    string running_dump_name;
   
    bool playdev;
};

#endif
