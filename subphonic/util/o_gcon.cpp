#include "o_gcon.h" 
#include "../util/debug.h"

namespace spl {
using namespace std;

LineBuffer::LineBuffer(uint len)
{
    length=len;
    add_count=0;
}

LineBuffer::~LineBuffer()
{
}

void LineBuffer::add(const string& str, int type)
{
    if(data.size()==length)
    {
        data.pop_back();
    }
   
    ConsoleLineData d;
    d.str=str;
    d.type=type;
    d.add_count=add_count;
    add_count++;
   
    data.push_front(d);
}

/*********/
/*CONSOLE*/
/*********/

//WARNING: untested
bool Console::setBGSurface(SDL_Surface* surf)
{
    if(surf==NULL)
    {
        ERROR("surf NULL");
        return false;
    }
   
    bitmap=surf;
   
    //RESET SIZE
    size.x=0;
   
    size.w=bitmap->w;
    ch_perline = size.w/fnt0->charW();
   
    size.y=0;
    size.h=bitmap->h;
   
    bitrect=size;
   
    return true;
}


Console::Console(uint w, uint h, BitmapFont* fnt0, BitmapFont* fnt1, Screen* s)
    : history(CON_MAXHISTORY)
{
    bitmap=NULL;
   
    VERBOSE1(cout << "starting console...";);
    cout.flush();
   
    this->screen=s;
   
    size.x=0;
    size.w=w;
    size.y=0;
    size.h=h;
   
    bitrect=size;
   
    cursor=0;
    cur_start=0;
    cblink_i=0;
    cblink=true;
   
    cx=cy=0;
   
    //clear inbuff
    memset(linebuff,'\0',GCON_LINELEN);
   
    state=CON_UP;
    scrollflag=SCROLL_NO;
    full=false;
   
    alpha=SDL_ALPHA_OPAQUE;
   
    histoff=0;
    chist=-1;
    this->fnt0=fnt0;
    this->fnt1=fnt1;
    DASSERT(fnt0->charW()!=0);
   
    ch_perline = size.w/fnt0->charW();
   
    setDownShow(0.5);
   
    setScrollSpeed(SCROLLSPEED);
   
    VERBOSE1(cout << "ok" << endl;);
   
    setBgColor(0,0,0);
   
    setDirectExec(true);
   
    tmpbuff = new char[EXPARG_BUFF];
    buffsz = EXPARG_BUFF;
   
    show_last_hist=false;
    show_last_hist_num=0;
    show_last_in=false;
    show_last_out=false;
   
    show_line_numbering=false;
   
}

void Console::setShowLineNum(bool on)
{
    show_line_numbering=on;
}

void Console::setShowLastHist(bool on, uint num, bool show_in, bool show_out)
{
    show_last_hist=on;
    show_last_hist_num=num;
    show_last_in=show_in;
    show_last_out=show_out;
}


void Console::setDirectExec(bool v)
{
    direct_exec=v;
}

bool Console::getDirectExec()
{
    return direct_exec;
}


uint Console::pQueueIn()
{
    return parse_queue.size();
}


//both these pop queue
void Console::pQueueExecNext()
{
    string n = parse_queue.front();
    parse_queue.pop();
    execCmdNow(n);
}

string Console::pQueueNext()
{
    string n = parse_queue.front();
    parse_queue.pop();
    return n;
}

void Console::execCmd(const string& line)
{
    if(direct_exec)
    {
        execCmdNow(line);
    }
    else
    {
        if(parse_queue.size()<MAX_PQUEUE)
            parse_queue.push(line);
        else DERROR("pqueue overflows");
    }
}


void Console::execCmdNow(const string& line)
{
    history.add(" ", LB_OUTPUT); // add nl
   
    //push it on history stack
    history.add(line.c_str(), LB_INPUT);
   
    if(!cmd.parseLine(line.c_str()))
    {
        show("no such cmd");
    }
}


void Console::setMonogram(const string m)
{
    monogram=m;
}


void Console::setBgColor(Uint8 r, Uint8 g, Uint8 b)
{
    bg_color.r=r;
    bg_color.g=g;
    bg_color.b=b;
}


Console::~Console()
{
    if(bitmap!=NULL)SDL_FreeSurface(bitmap);
    delete[] tmpbuff;
}

void Console::setAlpha(Uint8 a)
{
    alpha=a;
    if(bitmap==NULL)return;
   
    SDL_SetAlpha(bitmap, SDL_SRCALPHA | SDL_RLEACCEL, alpha);
}

Uint8 Console::getAlpha()
{
    return alpha;
}

void Console::setDownShow(float f)
{
    DASSERT(f>0 && f<=1.0);
    if(f<0)f=0;
    if(f>1.0)f=1.0;
   
    show_h=(int)(size.h*f);
}

void Console::setScrollSpeed(int s)
{
    DASSERT(s>0);
    scrollspeed=s;
}


void Console::drawBg()
{
    if(bitmap==NULL)
    {
        screen->box(0,0, bitrect.w, bitrect.h,screen->makeColor(bg_color));
    }
    else screen->blit(bitmap, NULL, &bitrect);
}


int Console::drawHistoryEntry(const int x, const int y, const ConsoleLineData& ent, bool append_add_count)
{
    int slen = ent.str.length();
    int nlines=0;
    int tlen=slen;
    while(tlen>0)
    {
        tlen-=ch_perline-3;
        nlines++;
    }
   
    int start=0;
    int nchar= (ch_perline-3>slen) ? slen : ch_perline-3;
   
    string ustr;
   
    for(int i=0;i<nlines;i++)
    {
        ustr = ent.str.substr(start,nchar);
	
        if(append_add_count)
        {
            if(ent.type==LB_INPUT)
                fnt0->printF(fnt0->charW(), y+fnt0->charH()*(i-nlines+1), "%d: %s" , ent.add_count, ustr.c_str());
            else if(ent.type==LB_OUTPUT)
                fnt1->printF(fnt1->charW(), y+fnt1->charH()*(i-nlines+1), "%d: %s", ent.add_count, ustr.c_str());
        }
        else
        {
            if(ent.type==LB_INPUT)
                fnt0->printF(fnt0->charW(), y+fnt0->charH()*(i-nlines+1), "%s" , ustr.c_str());
            else if(ent.type==LB_OUTPUT)
                fnt1->printF(fnt1->charW(), y+fnt1->charH()*(i-nlines+1), " %s", ustr.c_str());
        }
	
        start += ch_perline-3;
	
        if(start>=slen)break;
        if(start+nchar > slen)nchar=slen-start;
    }
   
    return nlines;
}

void Console::drawText()
{
    //bottom-4 of console
    int y=bitrect.h-fnt0->charH()*4;
   
    //print all text
   
    hist_it it= history.fwdBegin(histoff);
   
    for(;it!=history.end();it++)
    {
        if(y<fnt0->charH())break;
	
        int nlines = drawHistoryEntry(0, y, *it, show_line_numbering);
	
        y-=fnt0->charH()*(nlines);
    }
   
    //cursor blink
    if(cblink_i<CBLINK_RATE/2)cblink=true;
    if(cblink_i>CBLINK_RATE/2)cblink=false;
    if(++cblink_i>CBLINK_RATE)cblink_i=0;
   
    fnt0->printF(fnt0->charW(), bitrect.h-fnt0->charH()*2,">");
    if(cblink)fnt0->printF((2+cursor-cur_start)*fnt0->charW(), bitrect.h-fnt0->charH()*2,"_");
   
    bool wrap = fnt0->setWrap(false);
   
    int n = strlen(linebuff);
    if(n<ch_perline-2)
    {
        fnt0->printF(fnt0->charW()*2, bitrect.h-fnt0->charH()*2,"%s",linebuff+cur_start);
    }
   
    else
    {
        string s(linebuff, cur_start, ch_perline-2);
        fnt0->printF(fnt0->charW()*2, bitrect.h-fnt0->charH()*2,"%s",s.c_str());
    }
   
    fnt0->setWrap(wrap);
   
    if(histoff)fnt0->printF(fnt0->charW(), bitrect.h-fnt0->charH()*3,"^^^^^^^");
   
    //MONOGRAM
    if(monogram.length()>0)
        fnt0->printF(size.w-monogram.length()*fnt0->charH(), bitrect.h-fnt0->charH(), monogram.c_str());
}

void Console::show(const string& str)
{
    history.add(str, LB_OUTPUT);
}

void Console::show(const char* fmt, ...)
{
    va_list ap;
   
    va_start(ap,fmt);
    int buffsz2 = vsnprintf(tmpbuff,buffsz,fmt,ap);
    va_end(ap);
   
    if(buffsz2 >= buffsz)
    {
        //DERROR("buffsz to small expaning it, consider bigger initial value.");
        delete[] tmpbuff;
        buffsz=buffsz2*2;
        tmpbuff = new char[buffsz];
	
        va_start(ap,fmt);
        vsnprintf(tmpbuff,buffsz,fmt,ap);
        va_end(ap);
    }
   
    tmpbuff[buffsz-1]='\0';
   
    history.add(tmpbuff, LB_OUTPUT);
}

bool Console::isFull()
{
    return full;
}

void Console::start()
{
    //SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
}

void Console::stop()
{
    //SDL_EnableUNICODE(0);
    SDL_EnableKeyRepeat(0, 0);
}


bool Console::isActive()
{
    return (state==CON_DOWN);
}


int Console::handleEvents(Input& in)
{
    if(state==CON_UP)return 0;
   
    int slen;
    string str;
    char ch;
    int matched;
    int shortest_dup;
    int s_dlen;
   
    if (in.keySet(SDLK_LCTRL, false))
    {
        if (in.keySet(SDLK_a)){
            cursor=0;
            cur_start=0;
        }
        else if (in.keySet(SDLK_e)) {
            while(linebuff[cursor]!='\0')
            {
                cursor++;
                if(cursor+3>ch_perline)cur_start++;;
            }
        }
        else if (in.keySet(SDLK_d)) {
            if(linebuff[cursor] != '\0')
            {
                memmove(&linebuff[cursor],&linebuff[cursor+1],strlen(&linebuff[cursor]));
            }
            else linebuff[cursor]='\0';
        }
    }

    bool found=false;
    hist_it it;
    
    if (in.keySet(SDLK_ESCAPE, true)) {
        if(!full)togglescroll();
    }
    else if (in.keySet(SDLK_PAGEUP, true)) {
        histoff+=HISTORY_STEP;
        if(histoff>history.data.size())histoff=history.data.size();
    }
	else if (in.keySet(SDLK_PAGEDOWN, true)) {
        if(histoff>=HISTORY_STEP)
            histoff-=HISTORY_STEP;
        else histoff=0;
    }  
    else if (in.keySet(SDLK_BACKSPACE, true)) {
        if(cursor>0)cursor--;
        if(cur_start>0)cur_start--;

        if(linebuff[cursor] != '\0')
        {
            memmove(&linebuff[cursor],&linebuff[cursor+1],strlen(&linebuff[cursor]));
        }
        else linebuff[cursor]='\0';
    }
    else if (in.keySet(SDLK_LEFT, true)) {
        if(cursor>0)cursor--;
        if(cur_start>0)cur_start--;
    }
    else if (in.keySet(SDLK_RIGHT, true)) {
        if(linebuff[cursor]!='\0') {
            if(cursor<GCON_LINELEN-1)cursor++;
            if(cursor+3>ch_perline)cur_start++;
        }
    }
    else if (in.keySet(SDLK_RETURN, true)) {
        // run command?
        if(linebuff[0]!='\0') {
            execCmd(string(linebuff));

            //clear linebuff
            memset(linebuff,'\0',GCON_LINELEN);
            cursor=0;
            cur_start=0;
            chist=-1;
        }
    }
    else if (in.keySet(SDLK_UP, true)) {
        if(chist+1 != history.data.size()) {
            //find a command, start search on chist+1
            //when top of history is reached the input line is cleared
            //so that it is easy to enter new command

            chist++;
            it = history.fwdBegin(chist);

            while(true)
            { 
                if(it->type==LB_INPUT)
                {
                    str = it->str;
                    found=true;
                    break;
                }
                it++;
                if(it == history.end())break;
                chist++;
            }

            if(!found)
            {
                memset(linebuff,'\0', GCON_LINELEN);
                cursor=0;
            }
            else {
                memset(linebuff,'\0', GCON_LINELEN);
                slen = str.length();
                memcpy(linebuff,str.c_str(), slen);
                cursor=slen;
            }
        }
    }
    else if (in.keySet(SDLK_DOWN, true)) {
        //start search one lower than chist
        if(chist==-1)return 1;

        chist--;

        if(chist>=0) {
            //find a command
            it = history.fwdBegin(chist);

            while(true)
            { 
                if(it->type==LB_INPUT)
                {
                    found=true;
                    str = it->str;
                    break;
                }
                if(it==history.begin())return 1;
                it--;
                chist--;
            }
        }


        if(chist==-1)
        {
            memset(linebuff,'\0', GCON_LINELEN);
            cursor=0;
        }
        else if(found) {
            memset(linebuff,'\0', GCON_LINELEN);

            slen = str.length();
            memcpy(linebuff,str.c_str(),slen);
            cursor=slen;
        }
    }
    else if (in.keySet(SDLK_TAB, true)) {
        int i;

        if(cursor==0)
        {
            history.add(" ",LB_OUTPUT);
            history.add("all commands:",LB_OUTPUT);
            history.add(" ",LB_OUTPUT);

            for(i=0;i<cmd.num();i++)
            {
                //if(cmd.cmds[i].fn==NULL)continue;

                //print empty as separator
                if(cmd.getFun(i)==NULL)history.add(" ", LB_OUTPUT);
                else history.add(cmd.getName(i),LB_OUTPUT);
            }
            return 1;
        }		  
		  
        matched = -1;
        shortest_dup=-1;
        s_dlen=-1;

        for(i=0;i<cmd.num();i++)
        {
            if(cmd.getFun(i)==NULL)continue;

            if(!strncmp(linebuff, cmd.getName(i).c_str(), cursor))
            {
                if (matched >= 0) //this means duplicate commands
                {
                    if(s_dlen==-1)
                    {
                        int tmpi=0;
                        shortest_dup = i;
                        while(cmd.getName(shortest_dup)[tmpi]==cmd.getName(matched)[tmpi])tmpi++;
                        s_dlen=tmpi;
                    }
                    else
                    {
                        int tmpi=0;
                        while(cmd.getName(shortest_dup)[tmpi]==cmd.getName(i)[tmpi])tmpi++;
                        if(tmpi<s_dlen)s_dlen=tmpi;
                    }
                }

                matched = i;
            }
        }
        if (matched >= 0 && s_dlen==-1)
        {
            memcpy(linebuff, cmd.getName(matched).c_str(), cmd.getName(matched).length());
            cursor=cmd.getName(matched).length()+1;

            linebuff[cursor-1] = ' ';
        } 
        else if(shortest_dup>=0)
        {
            memcpy(linebuff, cmd.getName(shortest_dup).c_str(), s_dlen );
            cursor=s_dlen;

            history.add(" ",LB_OUTPUT);
            history.add("matched commands:",LB_OUTPUT);

            for(i=0;i<cmd.num();i++)
            {
                if(cmd.getFun(i)==NULL)continue;
                if(!strncmp(linebuff, cmd.getName(i).c_str(), cursor))
                    history.add(cmd.getName(i), LB_OUTPUT);
            }
        }
    }
    else {
        string str;
        in.inputStr(str);
                
        for (int i=0;i<str.length();i++)
        {
            char ch = str.at(i);

            if(cursor>=GCON_LINELEN-2)
            {
                cursor--;
                if(cursor+2>ch_perline)cur_start--;
            }
        
            slen = strlen(&linebuff[cursor]);
            if(linebuff[cursor] != '\0' && cursor+slen < GCON_LINELEN-2)
            {
                memmove(&linebuff[cursor+1],&linebuff[cursor], slen);
            }

            linebuff[cursor] = ch;
            cursor++;
            if(cursor+3>ch_perline)cur_start++;
        }
    }
    return 1;
}

void Console::draw()
{
    if(state==CON_UP)
    {
        if(show_last_hist)
        {
            int h = show_last_hist_num*fnt0->charH()+4;
            int w = bitrect.w;
            //FIXME: erronous, does not count nlines!
            screen->box(0,0, w, h, screen->makeColor(bg_color));
	     
            int y=h-fnt0->charH()-2;
	     
            //print all text
	     
            hist_it it= history.fwdBegin(histoff);
	     
            int done=0;
            for(;it!=history.end() && done<show_last_hist_num; it++)
            {
                if(y<0)break;
		  
                if(show_last_out && it->type==LB_OUTPUT)
                {
                    int nlines = drawHistoryEntry(0, y, *it, true);
		       
                    y-=fnt1->charH()*nlines;
		       
                    done+=nlines;
                }
                if(done<show_last_hist_num && show_last_in && it->type==LB_INPUT)
                {
                    int nlines = drawHistoryEntry(0, y, *it, true);
		       
                    y-=fnt0->charH()*nlines;
		       
                    done+=nlines;
                }
		  
            }
        }
	
        return;
    }
   
    //console is down or scrolling
   
    //FIXME: maybe move
    scroll(); //are we scrolling?
   
    drawBg();
   
    drawText();
   
}

void Console::togglefull()
{
    full = !full;
   
    if(full)
    {
        SDL_SetAlpha(bitmap, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
	
        bitrect.y=0;
        bitrect.h=size.h+1;
        state=CON_DOWN;
        scrollflag=SCROLL_NO;
    }
    else
    {
        SDL_SetAlpha(bitmap, SDL_SRCALPHA, 128);
	
        bitrect.y=show_h;
        bitrect.h=0;
        state=CON_UP;
        scrollflag=SCROLL_NO;
    }
}

void Console::togglescroll()
{
    if(full)return; //no scrolling here!
   
    if(state==CON_SCROLL) //already in scroll motion reverse it
    {
        if(scrollflag==SCROLL_UP)
        {
            scrollflag=SCROLL_DOWN;
            //start();
        }
	
        else 
        {
            scrollflag=SCROLL_UP;
            stop();
        }
	
    }
   
    if(state==CON_UP)
    {
        //start();
        scrollflag = SCROLL_DOWN;
        bitrect.y=size.h;
        bitrect.h=0;
    }
    if(state==CON_DOWN)
    {
        stop();
        scrollflag = SCROLL_UP;
        bitrect.y=0;
        bitrect.h=show_h;
    }
    state=CON_SCROLL;
}

void Console::scroll()
{
    if(full)return;
    if(scrollflag==SCROLL_NO)return;
   
   
    if(scrollflag==SCROLL_DOWN)
    {
        if(bitrect.h < show_h)
        {
            bitrect.y-=scrollspeed;
            bitrect.h+=scrollspeed;
            if(bitrect.h>show_h)bitrect.h=show_h;
            if(bitrect.y<0)bitrect.y=0;
        }
        else 
        {
            scrollflag=SCROLL_NO;
            state=CON_DOWN;
	     
            start(); //?
        }
	
    }
   
    if(scrollflag==SCROLL_UP)
    {
        if(bitrect.h > 0)
        {
            bitrect.y+=scrollspeed;
            bitrect.h-=scrollspeed;
	     
            //value is Uint, so it will flip:
            if(bitrect.h>size.h)bitrect.h=0;
	     
            if(bitrect.y>size.h)bitrect.y=size.h;
        }
        else 
        {
            scrollflag=SCROLL_NO;
            state=CON_UP;
        }
    }
}

char* Console::getLineBuff()
{
    return linebuff;
}


int Console::addCmd(const std::string& name, cmd_fun fn )
{
    return cmd.add(name, fn);
}

int Console::remCmd(const std::string& name)
{
    return cmd.rem(name);
}

}