#ifndef O_GCON_H
#define O_GCON_H

#include <time.h>
#include <sys/types.h>

//linux#include <unistd.h>

#include <iostream>
#include <string>
#include <deque>
#include <queue>

#include <string.h>
#include <stdarg.h>
#include <SDL.h>

#include "../screen/all.h"
#include "cmdi.h"
#include "../util/debug.h"

namespace spl {

#define CON_UP 0
#define CON_DOWN 1
#define CON_SCROLL 2

#define SCROLL_UP -1
#define SCROLL_NO 0
#define SCROLL_DOWN 1

#define H_FORW 1
#define H_BACK -1

#define CON_MAXHISTORY 400
#define CON_MAXCMDS 100
#define GCON_LINELEN 200
#define CBLINK_RATE 36
#define EXPARG_BUFF 200
#define SCROLLSPEED 48 //default
#define HISTORY_STEP 20

#define LB_UNUSED 0
#define LB_OUTPUT 1
#define LB_INPUT 2

class ConsoleLineData
{
  public:
    string str;
    int type;
    uint add_count;
};

typedef deque<ConsoleLineData>::iterator hist_it;

class LineBuffer
{
  public:
   
    LineBuffer(uint len/*i.e max entries*/);
    ~LineBuffer();
   
    void add(const string& str, int type);
    void add(const char* str, int type)
    {
        add(string(str), type);
    }
   
    hist_it begin()
    {
        return data.begin();
    }
   
    hist_it fwd(const hist_it& iter, unsigned int off)
    {
        hist_it it=iter;
	
        for(unsigned int i=0; i<off && it!=end();i++)it++;
	
        return it;
    }
   
    hist_it fwdBegin(unsigned int off)
    {
        hist_it it=begin();
	
        for(unsigned int i=0;i<off && it!=end();i++)it++;
	
        return it;
    }
      
    hist_it end()
    {
        return data.end();
    }
   
   
    deque<ConsoleLineData> data;
    uint length;
    uint add_count;
};


class Console
{
    static const uint MAX_PQUEUE=1000;
   
  public:
   
    //use uniform color:
    Console(uint w, uint h, BitmapFont* fnt0, BitmapFont* fnt1, Screen* s);
    ~Console();
   
    //WARNING: untested
    //overrides w,h
    bool setBGSurface(SDL_Surface* surf);
   
    //if true, commands are exec'd when return is pressed
    //otherwise they are pushed on parsedQueue(); //better drain it, or commands will be lost
    //def: true
    void setDirectExec(bool v);
    bool getDirectExec();
   
    uint pQueueIn(); //num commands redy to execute
   
    //both these pop queue
    void pQueueExecNext();//same exec as would have been done as in direct(with history add etc)
    string pQueueNext(); //caller deside(no history add or anything)
   
    //execute commands via console(good for history add etc):
   
    //FIXME: bool return
    //executed directly, even if setDirectExec(false), good to circumvent queue temporarily
    void execCmdNow(const string& line);
   
    //conforms to setDirectExec i.e if that is false this is same as execCmdNow
    void execCmd(const string& line);
   
    void setMonogram(const string m);
   
    //0-SDL_ALPHA_OPAQUE(255); 255 is same as OFF
    void setAlpha(Uint8 a=SDL_ALPHA_OPAQUE);
    Uint8 getAlpha();   
   
    void drawBg();
    void drawText();
   
    void show(const char* fmt, ...);
    void show(const string& str);
   
    //only actualy runs if con is down: return 1 if con down or going down; 0 otherwise
    int handleEvents(Input& in);
    void draw();
   
    char* getLineBuff();
   
    bool isFull();
    void start();
    void stop();
      
    bool isActive();
   
    void togglefull();
    void togglescroll();
    void scroll();
    void setScrollSpeed(int s);
   
   
/*   int getOutLineNo(char* str, int i)
     {
     if(history->typeAt(i) == LB_UNUSED || history->typeAt(i)==LB_INPUT)return 0;
     char* tmp_str = history->dataAt(i);
	
     strcpy(str,tmp_str);
     return strlen(str);
     }*/
   
    void setDownShow(float f);
   
    void addHistory(const char* str)
    {
        history.add(str, LB_INPUT);
    }
   
    void setBgColor(Uint8 r, Uint8 g, Uint8 b);
   
    int addCmd(const std::string& name, cmd_fun fn );
    int remCmd(const std::string& name);
   
    //def:false
    void setShowLineNum(bool on);
   
    //def:false
    void setShowLastHist(bool on, uint num=1, bool show_in=true, bool show_out=true);
   
  private:
   
    //returns number of lines entry took
    //'x' igonored
    int drawHistoryEntry(const int x, const int y, const ConsoleLineData& ent, bool append_add_count=false);
   
    int state;
    CmdInterface cmd; //FIXME: make better
   
    string monogram;
   
    Screen* screen;
    SDL_Surface* bitmap;
   
    SDL_Rect bitrect;
    SDL_Rect size;
   
    SDL_Color bg_color;
   
    BitmapFont* fnt0;
    BitmapFont* fnt1;
   
    int cblink_i;
    bool cblink;
    int cursor;
    int cx, cy;
    int cur_start;
    int ch_perline;
    char linebuff[GCON_LINELEN];
    Uint8 alpha;
   
    int full;
    int scrollflag;
     
    uint histoff;
    int chist;
  
    LineBuffer history;
   
    int show_h;
    int scrollspeed;
   
    bool direct_exec;
    queue<string> parse_queue;
   
    char* tmpbuff;
    int buffsz;
   
    bool show_last_hist;
    uint show_last_hist_num;
   
    bool show_last_in;
    bool show_last_out;
   
    bool show_line_numbering;
};
}

#endif
