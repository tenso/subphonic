#ifndef G_COMMON_H
# define G_COMMON_H

/*USES SINGLETON GLOBALS:
 * ProgramState - const
 *SurfaceHoldAutoBuild - changed as needed
 */

//FIXME: st_seq is static(th)

#include <sstream>
#include <map>

#include "../gui/all.h"
#include "../screen/all.h"
#include "../sig/all.h"
#include "../m_main/progstate.h"
#include "../util/types.h"
#include "../util/res_hold.h"

#include "../util/util.h"
#include "../math/mathutil.h"

using namespace spl;

/*FIXME: defs not here?*/

static const int GCOMP_LABEL_XOFF=40;
static const int GCOMP_LABEL_YOFF=5;

static const int GCOMP_STDIN_XOFF=5;
static const int GCOMP_STDIN_YOFF=3;
static const int GCOMP_STDOUT_XOFF=19;
static const int GCOMP_STDOUT_YOFF=3;

static const smp_t KNOB_TURN_SLOW=0.02;
static const smp_t KNOB_TURN_FAST=0.05;



//THERE IS A PROBLEM: beacuse of dopplers which "cache" value once per tick it is possible
//to cache A(wanting to sync for new state) as 0 (before A set's sync out to 1) and then things
//will only sync(e.g changing state) the next tick. however it is not enough to wait just one sample
//the thing syncing out can not continu just yet; a stat tick might be after A o if a starts now it will still be 
//on old state. i.e two samples minimum sync

//SO: things MUST hold syncout signal for this time and not continue(update outs) before
//SYNCIN ALWAYS president's i.e if a sync in comes while waiting; discard wait and resume a usuall
#define GCOMP_WAIT_SMPL_AFTER_SYNCOUT 2


//FIXMENOW: fix
# define OUTPUT_GIVER GUI_LASTID+1 
# define INPUT_TAKER GUI_LASTID+2
# define SOUND_COMP GUI_LASTID+3

//remember not to remove when removing comp's, otherwise all saves will load wrong comps(overwrite instead)
enum SCType //use FREE_TO_BE_USED_X first
{
    G_UNUSED=0, G_NONE/*dont remove*/, G_WAV, G_OSC, G_SAMPLE_AND_HOLD, G_MIX, G_GAIN, G_FILTER, //=7
    G_DELAY, G_ADSR/*OBS*/, G_WS, G_DC, G_ENV, G_ABS, G_CROSSFADE, G_SAVE/*UNUSED*/, G_DOPPLER, G_SCOPE, G_NOTE, G_STEPMAT, //=19
    G_PUNCHPAD, G_METRO, G_KEYBOARD, G_PLAY, G_BRIDGE, G_STATETICK, G_SPEC, G_VECTOR, G_DRAWENV, //=28
    G_MIDIKEY, G_MIDICTRL, G_MIDIPGM, G_SELECTIN, G_TRACKER, G_SLEW, G_OCTSCALE/*OBS*/, G_CLIP, G_LOGISTICMAP, //=37
    G_THRESHOLD, G_AHDSR, G_DRAIN, G_FFT_FILT, G_NOTEGAIN, G_EXP, G_LOG, G_DIVIDE, G_MULT, G_SORT, G_KEYEXTR, G_GATETOPULSE, //49
    G_PRINT, G_SIGN, G_PLUS, G_MINUS, G_MIN, G_MAX, G_FLOOR, G_CEIL, G_TRUNC, G_SIN, G_MOD, G_SAMPLERATE,
    G_EXECCMD_DUMPFORCE, G_EXECCMD_PROCESS, G_LINEAR, G_LESSTHAN, G_LESSTHANEQUAL, G_INTEGRATE, G_CHANGEPULSE, G_JOYSTICK,
    G_ONOFF, G_BUFFER, G_BITCRUSH, G_EDELAY, G_ALTZEROCTRL
};

class SoundComp; //predef
class OutputGiver;
class InputTaker;

//this is a connection as saved on disk(not used in running program)
class ConPair 
{
  public:
    void offsetSeq(int i)
    {
        DASSERT(seq_out+i>=0);
        DASSERT(seq_in+i>=0);
	
        seq_out+=i;
        seq_in+=i;
    }
   
    unsigned int seq_out; //getSeqNum of comp i.e numbering 
    unsigned int seq_in;
   
    //FIXME: namechange: remove seq, not a seq at all
    unsigned int seq_out_id;//the value id for that comp, i.e what "hole" on the comp
    unsigned int seq_in_id;  
   
};

//FIXME: 
/*represents a connection in program, keeps track of draw info: positions colors animation
  + the intkaer/outgiver pair that made the connection(for later removal etc)*/
class LineData
{
  public:
    LineData(SDL_Rect s, SDL_Rect e, OutputGiver* out, InputTaker* in);
   
    ConPair getConPair(); //builds a ConPair from this(for disk save)
   
    SDL_Rect line[2]; //draw
   
    //a connection must keep the giver/taker pair so that when removeLine(m_con) is run
    //it can flag the soundcomps to action
    OutputGiver* out;
    InputTaker* in;

   
    double tmp_settle;

    //not efficient but rather good for coupling
    //FIXME: should save these on disk
    bool have_color;
    Uint8 color[3];
    Uint32 color_v;
   
    double tmp_count;
};

//OutputGiver and InputTaker is the actual GUI-drawn connection holes
//that lines are made between in program. 
//They mediate the appropriate SoundComp that can then make that appropriate Value** connections
//found here because of there strong coupling with SoundComp

//FIXME: change name of id; but what?

class InputTaker;

class OutputGiver : public Component
{
    friend class InputTaker;
  public:
    OutputGiver(SDL_Surface* pixmap, SoundComp* sc, unsigned int id=0);
    virtual ~OutputGiver();
     
    GUI_COMPID compID();
    void draw(Screen* screen);
      
    //signal the comp to remove output
    void remOutput();
   
    void relMove(int x, int y);
   
    //the ONLY reason for this:
    //when this comp moves it must update the Line positions
    void setLine(LineData* d);
    void remLine();
   
    SoundComp* getSrcSC();
    uint getId(); //not to be confused with compID() completely different
   
    //FIXME: move to protected
    LineData* line;
  protected:
   
    SDL_Surface* back;
   
    SoundComp* src;
    unsigned int id;
};


class InputTaker : public Component
{
    friend class OutputGiver;
  public:
    InputTaker(SDL_Surface* pixmap, SoundComp* src, unsigned int id=0);
    virtual ~InputTaker();
   
    GUI_COMPID compID();

    void relMove(int x, int y);
    void draw(Screen* screen);
   
    //runs SoundComp::addInput for 'src' with getOutput from the arg giver 'in'
    void setInput(OutputGiver* in);
   
    //tell 'src' to remove this input
    void remInput();
   
    //the ONLY reason for this:
    //when this comp moves it must update the Line positions
    void setLine(LineData* d);
    void remLine();
   
    SoundComp* getSrcSC();
    uint getId(); //not to be confused with compID() completely different
    //const LineData* getLineData();
   
    //FIXME: move to protected
    LineData* line;
   
  protected:
    SDL_Surface* back;
   
    SoundComp* src;
    OutputGiver* og;
    unsigned int id;

};

//FIXME: namechange
//raw state data for SoundComp, send one in to a SC (getState() et al) and it knows how to
//set it delete it etc.
//FOR STATE
class SoundCompEvent
{
  public:
    SoundCompEvent();
    ~SoundCompEvent();
   
    SoundCompEvent(const SoundCompEvent& from);
    SoundCompEvent& operator=(const SoundCompEvent& from);
   
    bool empty() const;
    uint getSize() const;
    const byte* getData() const;
   
    //takes care of allocation/reallocation
    void copyData(const byte* d, uint s);
    void free();
   
   
    //SPECIAL
    //most will not need these: used for partial state copys etc: e.g copy a single tracker track
    //just ignore if not useing
   
    //set before/checked in getState()
    void setWantPartial(bool v);
    bool getWantPartial() const;
   
    //in setState()
    void setIsPartial(bool v);
    bool getIsPartial() const;
   
    void setPartialNo(int no);
    int getPartialNo() const;
   
   
  private:
   
    //also reallocs
    void allocate(uint size);
    void deallocate();
   
    char* data;
    uint size; //in bytes
   
   
    //SPECIAL
    bool want_partial;
    bool is_partial;
    int partial_no;
};

//class used to save soundcomp data on disk
//+ appendix (not states)
//FIXME: name, SoundCompDiskEntry, FOR COMP
class DiskComponent
{
  public:
    class Data;
   
    DiskComponent();
    DiskComponent(SCType id, unsigned int s, SDL_Rect p);
    ~DiskComponent();
    void offsetSeq(int i);
   
    DiskComponent(const DiskComponent& c);
    DiskComponent& operator=(const DiskComponent& c);
   
    //APPENDIX is for set-once data, i.e at load and never again
    //while states(SoundCompEvent) can be many and set anytime
    bool haveAppendix() const;
   
    //local copy
    void setAppendix(const Uint8* raw, unsigned int len);
    void setAppendix(const Uint8* raw);
   
    const Uint8* getAppendixData() const;
    unsigned int getAppendixSize();
   
    Data getData() const;
    void setData(SCType id, unsigned int seq, SDL_Rect pos);
    void setData(const Data& d);
   
    //access data
    SCType getSCType();
    unsigned int getSeqNum();
    SDL_Rect getPos();

   
    //data format
    class Data //saved on disk
    {
      public:
        Data()
        {
            sc_type=G_UNUSED;
            seq=0;
            appendix_len=0;
            memset(&pos,0,sizeof(SDL_Rect));
        }
	
        void offsetSeq(int i)
        {
            seq+=i;
        }
	
        SCType sc_type;
        unsigned int seq;
        SDL_Rect pos;
        unsigned int appendix_len;
	
    };
   
  private:
    bool empty;
    Data data;
    Uint8* appendix; //might be saved on disk
};

//should not be instanciated
//THIS is the program building block, i.e a GUI capable, input output capable 'Value in base' thing
class SoundComp : public Container
{
  public:
    SoundComp(GroundMixer& ground = SingleGround::instance());
    virtual ~SoundComp();
   
    virtual string name() const
    {
        return string("SoundComp");
    }
   
   
    /*START
      SHOULD BE OVERLOADED*/
     
   
    //OUTPUT
    virtual Value** getOutput(unsigned int id);
   
    //most comps probably do not need this
    //WARNING: o is NULL
    virtual void remOutput(Value** o, unsigned int id);
      
    //input
    virtual void addInput(Value** out, unsigned int id);
   
    //FIXME: remove 'out', no need for it
    virtual void remInput(Value** out, unsigned int id);
         
    //virtual void reset(); //UNUSED
   
    //state handling, empty here in base
   
    //get current state of comp, also allocates memory, so caller MUST deallocate, use: e->deallocate()
    //MUST DO'S in DERIVIED
    //must allways set size use: e->allocate(uint)
    //must allways check e->data for NULL if empty event is sent in
    //derivied can atleast do a little sanity check: state size
    virtual void getState(SoundCompEvent* e);
    virtual void setState(SoundCompEvent* e);
      
    //virtual int getInterpolatedState(SoundCompEvent* dst, SoundCompEvent* start, SoundCompEvent* end, double pos);
   
    //some might want this to put all affected in some common state i.e g_punchpad etc..
    virtual int syncToIndex(unsigned int index);
   
    //use this to append any appendix from comp
    virtual DiskComponent getDiskComponent();
   
    //and then implement this to set comp from appendix data
   
    //Appendix IS set at same time as comp load, so it is set before anything else e.g State or Connecions
    //this is good to know, so appendix can set the number of IO's etc...
    virtual void setAppendix(const Uint8* data, unsigned int len);
   
    /*SHOULD BE OVERLOADED 
      END*/
   
   
    virtual GUI_COMPID compID();
   
    void setSCType(SCType id);
    SCType getSCType();
   
    unsigned int getSeqNum();
    void setSeqNum(unsigned int s);
   
    //comps get seq num automatically(static) unless setSeqNum() is used.
    static unsigned int getSeqCount()
    {
        return st_seq;
    }
   
    static unsigned int resetSeqCount()
    {
        st_seq=0;
        return st_seq;
    }
   
    static void setSeqCount(unsigned int n)
    {
        st_seq=n;
    }
      
   
    //return NULL/0 on fail
    InputTaker* addInputTaker(InputTaker* in) ;
    void addInputTaker(InputTaker** in, int num) ;
   
    bool remInputTaker(InputTaker* in) ;
   
    //get by id
    virtual InputTaker* getInputTaker(unsigned int id=0);
    map<uint,InputTaker*>* getAllIntakers();
   
    //sets the corresponding giver for o->id
    OutputGiver* addOutputGiver(OutputGiver* o);
    void addOutputGiver(OutputGiver** o, int num);
    bool remOutputGiver(OutputGiver* o);
   
    virtual OutputGiver* getOutputGiver(unsigned int id=0);
    map<uint,OutputGiver*>* getAllOutgivers();
      
   
    //base default: -1, i.e if used derived must override
    //returns -1 if not found, cast to uint(if positive) for id
    virtual int inputTakerId(const string& name, uint port);
    //returns -1 if not found, cast to uint(if positive) for id
    virtual int outputGiverId(const string& name, uint port);
   
  protected: //FIXME: make private?
    SCType sc_type;
   
    map<uint, InputTaker*>  intakers;
    map<uint, OutputGiver*> outgivers;
   
    //Value** output;
    Value* empty;
    unsigned  int seq;
    static unsigned int st_seq;
   
    //class global static data: many g_comp want these
    //FIXME: name g_pix; ?
    static SurfaceHoldAutoBuild& pix;
    static const ProgramState& progstate;

    static ResHold<BitmapFont>& fnt;
   
    //might need different grounds:
    //static SingleGround& ground;
    GroundMixer& ground; //default is SingleGround
};


#endif
