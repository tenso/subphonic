#ifndef G_BRIDGE_H
# define G_BRIDGE_H

//FIXME: hack done: INTEGER2 used for N slots

# include "g_common.h"
# include "../m_main/m_con.h"
# include "../m_main/m_bench.h"

# include "midibase.h"

class GBridge : public SoundComp
{
    static const bool STANDARD=0;
    static const bool LOADED_MODULE=1;
    static const int COMP_W=232+8;
    static const uint LABEL_NCHAR=17;
    //starting value
    static const uint STD_NUM_IO=8;
   
  public :
    GBridge(BitmapFont* f, GroundMixer& g);
    virtual ~GBridge();
   
    //WARNING: must run this before delete!
    //returns number of removed
    int remAllMidiHooks(MIDIcall* midicall);
   
    virtual string name() const
    {
        return string("GBridge");
    }
   
    Value** getOutput(unsigned int id);
    void remOutput(Value** o, unsigned int id);
   
    void addInput(Value** o, unsigned int fromid);
    void remInput(Value** o, unsigned int fromid);
   
    void flipComp(const string name);
    bool isFlipped();
   
    //there are two different bridge in m_main: 
    //unflipped: treat as ordinary comp in prog; wantFlip()=false
    //flipped: must load it's comp/line/state and add it as a input module; wantFlip()=true
    bool wantFlip();
   
    string getFilename();
    //WARNING: cont must not contain ANYTHING but SoundComp's (FIXME: maby make this explicit)
    void setContent(Container* cont, Connection* conn, BenchStates* states);
   
    virtual DiskComponent getDiskComponent();
   
    //make sure to run this before ANY addInput() etc, this sets num ports
    //this is OK: comps must be loaded before any connections and appendix will
    //be set at same time as loading
    virtual void setAppendix(const Uint8* data, unsigned int len);
   
    void updateLabels();
    //returns false if 'i' is out of range
    bool setLabel(const string& s, uint i);
   
   
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
      
    int syncToIndex(unsigned int index);
   
   
    //WARNING: because a port only have one name these are exactly the same:
   
    //port ignored for now
    //returns -1 if not found, cast to uint(if positive) for id
    int inputTakerId(const string& name, uint port);
    //returns -1 if not found, cast to uint(if positive) for id
    int outputGiverId(const string& name, uint port);
   
  private:
   
    /*REMOVE ME*/
   
    class State_V0
    {
      public:
	
        double knob_vals[16];
    };
   
   
    /*END*/
   
   
    /*State format:
     * StateHead
     * double vals[head::num_io] //cast to needed
     * IOEntry::CMODE modes[head::num_io]
     */
   
    class StateHead
    {
      public:
        uint num_io;
    };
   
    //remeber: buidState allocates 'ret_data'
    void buildState(byte** ret_data, uint* ret_size);
   
   
    /*PORTS*/
   
    class IOEntry //thightly coupled, ok
    {
      public:
        enum CMODE { NONE=0, KNOB, BUTTON_ONOFF, INTEGER2, EEND /*,BUTTON_RESET,*/};
	
        void updateOutVal(); //resets 'out_val'
	
        string name;
	
        bool std_uses_in;
        bool std_uses_out;
	
        InputTaker* it;
        OutputGiver* og;
	
        Const* out_val;
	
        Value** in;
        ValueForward* forward;
		
        Label* label;
	
        //one of there
        CMODE cmode;
        KnobSlider* knob;
        Button* button_onoff;
        Button* button_reset;
        Component* no_comp;
        NumberSelect<int>* ns_int2;
	
        uint id;
        bool have_out;
        bool have_in;
	
        //only used in stdmode
        Button* sel_mode;
        Component* sel_c;
    };
   
    void installPort();
    bool removeLastPort();
    void setPortMode(uint n, IOEntry::CMODE cmode, bool rearange=true);
    void setPortData(uint i, double val);
    double getPortData(uint i);
   
    void setBackSize();
    bool setNumPorts(uint n);
   
    class C_PortVal : public Action
    {
      public:
        C_PortVal(GBridge* src, int n);
        void action(Component* c);
	
      private:
        int n;
        GBridge* src;
    };
   
    class C_CyclePortMode : public Action
    {
      public:
        C_CyclePortMode(GBridge* src, int n);
        void action(Component* c);
	
      private:
        int n;
        GBridge* src;
    };
   
    class C_NumPort : public Action
    {
      public:
        C_NumPort(GBridge* src);
        void action(Component* c);
	
      private:
        GBridge* src;
    };
   
    static const string deflabel;
   
    //debug flag
    bool midi_hooks_rem;
   
    bool want_flip;
    bool flip;
   
    Label* title;
   
    Pixmap* back_pix;
    uint pix_w;
    uint pix_h;
   
    MoveBorder* move_border;
   
    NumberSelect<uint>* nportsel;
    Label* nportlab;
    uint nport;
   
    int port_yoff;
   
    BitmapFont* f;
   
    //size of this is num_io
    //sorted by id: so ports[0] is first etc...
    //vector choice: "probably" more random access than insert/remove
    vector<IOEntry*> ports;
   
    string filename;
   
    //when used as interface for module we need to save these
    Container* cont;
    Connection* conn;
    BenchStates* states;
   
    //for draw
    int out_xoff;
    int norm_out_xpush;
    int flip_label_xpush;
   
    /**********/
    /*APPENDIX*/
    /**********/
   
    //caller must delete returned 'data'
    void assembleApdx(Uint8** retdata, int* retlen);
   
    class Appendix
    {
        /*appendix data on disk:
         * Head
         * 
         * if(namesize)
         *  char name[namesize]
         * 
         * nlabel times of:
         *  LabelHead
         *  char labelname[LabelHead::size]
         * 
         * num_io times of:
         * IOEntryData
         */
	
      public:
	
        class Head
        {
          public:
            bool flipped; 
            uint namesize;//also acts as have_name; namelen!=0
            uint nlabel; //0 to 'num_io'
	     
            uint num_io;
        };
	
        class LabelHead
        {
          public:
            uint num; //what label
            uint size;//strlen
        };
	
        class IOEntryData
        {
          public:
            uint thissize; //of this, includeing all
            IOEntry::CMODE cmode;
        };
    };
};

#endif
