#ifndef M_MAIN_H
#define M_MAIN_H

#include <queue>

#include <SDL.h>

#include "../g_comp/g_all.h"
#include "../screen/all.h"
#include "m_con.h"
#include "../gui/gui.h"
#include "progstate.h"
#include "m_bench.h"
#include "m_main_help.h"
#include "../util/o_gcon.h"

#include "../audio/midi.h"

/*holds a copy of a comp state for edit*/
class StateCopyHold
{
  public:
    enum ERR {NO_COPY, NOTSAME_COMP_TYPE, NO_DEST, NO_SRC, OK};
   
    StateCopyHold()
    {
        parent=NULL;
    }
    ~StateCopyHold()
    {
        purge();
    }
   
    SoundComp* getParent();
   
    //nullify hold, this must be done if *parent is deleted from program!
    void purge();
   
    ERR copyState(SoundComp* from, bool partial);
    ERR pasteState(SoundComp* to);
   
    //void cutState(SoundComp* from);
   
  private:
    SoundComp*      parent;
    SoundCompEvent data;
};

/************************/
/*this is the main class; handles comp's sample making drawing etc...*/
/************************/

class Main
{
  public:
   
    friend class MenuAction;
    friend class MenuCmdAction;
    friend class C_MenuDir;
    friend class C_FileDir;
    friend class C_TempDir;
   
    //FIXME: remove screen from here to draw(Screen*)
    //FIXME:somewhat dangerous: 'con' can only be used when this comp is master
    Main(int x, int y, Screen* s, BitmapFont* font, Console* con);
    ~Main();
   
    SoundComp* getMouseOver(int x, int y);
    bool remMouseOver(int x, int y);

    bool handleInput(Input& in);
   
    //this needs mouse x,y, so last handleMouseMovePos() is used...
    void draw();
   
    /*bool playIsOn()
      {
      if(play==NULL)return false;
      return play->isOn();
      }*/
   
    //FIXME: semantics changed, no need for more than 1 sample now
    //returns next batch of samples(i.e drained from mainmix)
    //const Sample* makeSample();
   
    /*SAVE/LOAD*/
    m_err saveProgram(const string& file);
    m_err loadProgram(const string& file, bool try_force=false);

    void setLastLoad(const string& s);
    string getLastLoad() const;
   
    void setPrefix(const string& s);
    string getPrefix() const;
   
    //also select's what is added
    m_err addComp(SCType id, int x, int y);
    m_err addComp(SoundComp* s, int x, int y); //FIXME: this is pretty ugly can break stuff
   
    void remComp(SoundComp* sct);
   
    SoundComp* selectComp(SoundComp* c);
   
    //allways check return for NULL, that means no selectedComp is present
    SoundComp* getSelectedComp()
    {
        return selected_comp;
    }
  
    bool keyboardSelected();
    bool keyboardStay();
    void giveKeyboard(Input* in);
   
    void giveJoystick(Input* in);
   
    bool midiCompPresent();
    spl::MIDIcallbacks* getMIDIcallbacks();
    MIDIcall* getMIDIcall();
    void setMIDI(spl::MIDI* midi);
   
    void syncAllToIndex(unsigned int index);
   
    //audio generation
    void freeSampleQueue(); 
    void makeSampleQueue(uint n, uint buff_len); //automatically runs freeSampleQueue if needed
    const Sample*  smpQueuePop();
    uint smpQueueSize(); //'buff_len'
    uint smpQueueIn(); //between 0 -> 'buff_len'
    int smpQueuePush(); //returns 1 if queue full, 0 if all ok
    void smpQueueClear();
   
    //returns new highest seqnum
    int resetSeqNums();
   
    //comp position
   
    //workspace pos
    void setCompOffBound(int min_x, int min_y, int max_x, int max_y);
    void addCompOff(int x, int y);
    void setCompOff(int x, int y);
   
    SDL_Rect getCompOff();
   
    //actual pos
    void relMoveAllComps(int x, int y);

    StateCopyHold::ERR statePasteSelected();
    StateCopyHold::ERR stateCopySelected(bool partial=false);
   
    //FIXME:?
    BitmapFont* f;
    MainBench* bench;
   
    void setLastMenuAdd(SCType t);
    SCType getLastMenuAdd();
   
    GroundMixer& getGround()
    {
        return *ground;
    }
   
    Connection* getConnection()
    {
        return &connection;
    }
   
    bool clipSCPos(SoundComp* sc);
   
    //use these to safely exec things on console:
    //"uploaded" in thread to master_th
    //beware: cannot exec commands that are maincomp specific: exec'd as master's maincomp
    void pushCmd(const string& cmd);
   
    //used to "upload"
    bool haveNextCmd();
    string getNextCmd();
   
   
    //FIXME: cmd.h n_chan need to reset this
    GPlay* play;
   
    //FIXME: cmd.h n_chan needs this to
    DirReaderMenu* dirmenu;
    DirReaderMenu* tempmenu;
    DirReaderMenu* filemenu;
   
    spl::MIDI* midi;
   
  private:
    void buildMenu();
    bool handleReleaseOver(int x, int y);
   
    //0-left, 1-middle, 2-right
    SoundComp* handleClickOver(int x, int y, int button, bool l_shift);
   
    SCType last_menu_add;
   
    int lmouse_x;
    int lmouse_y;
   
    StateCopyHold sc_copy;
   
    bool co_doclamp;
    int co_min_x;
    int co_min_y;
    int co_max_x;
    int co_max_y;
    SDL_Rect cont_off;
   
    m_err loadComponentsMain(istream* fin, int sc_seq_off);
    m_err loadStatesMain(istream* in, int sc_seq_off, bool try_force=false);
   
    Container* cont;
   
    Mixer* mainmix;
    GroundMixer* ground;
    //SingleGround* groundp;
   
    Menu* menu;
    bool menuon;
   
    SDL_Rect addpos;
   
    //special components for various reason
    //FIXME: make into list(so many instances possible):
    GKeyboard* keyboard;
   
    //add any midi comp to this so that it will recieve events
    MIDIcall midicall;
   
    GStateTick* statetick;

    SoundComp* selected_comp;
    Screen* s;
   
    Connection connection;
   
    unsigned int csmp;
    unsigned int nsample_buff;
    Sample** smpls;
   
    //FIXME: this is cahnged(th)
    static ProgramState& progstate;
   
    std::queue<Sample*> smp_queue;
   
    string lastload;
    string prefix;
   
    Console* con;
   
    std::queue<std::string> cmd_queue;
    std::list<GJoystick*> joysticks;
   

};

/*move these to Main::*/

class MenuAction : public Action
{
  public:
    MenuAction(SCType mode, Main* src);
    void action(Component* c);
   
    Main* src;
    SCType mode;
};

class MenuCmdAction : public Action
{
  public:
    MenuCmdAction(const string& cmd, Main* src);
    void action(Component* c);
   
    Main* src;
    string cmd;
};

class C_MenuDir : public DirReaderAction
{
  public:
    C_MenuDir(Main* src);
    void action(Component* c);
   
  private:
    Main* src;
};

class C_FileDir : public DirReaderAction
{
  public:
    C_FileDir(Main* src);
    void action(Component* c);
   
  private:
    Main* src;
};

class C_TempDir : public DirReaderAction
{
  public:
    C_TempDir(Main* src);
    void action(Component* c);
   
  private:
    Main* src;
};

#endif
