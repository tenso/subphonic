#ifndef TRACKER_H
#define TRACKER_H

#include <sstream>

#include "../gui/gui.h"
#include "../gui/gui_sheet.h"

#include "../sig/signal.h"

#include "../util/debug.h"

#include "../screen/input.h"

using namespace spl;

/*FIXMES:
 * no pre-look for tracks
 * key repeat for note add would be nice when filling notes
 */

class Tracker : public Component
{
    static const uint TRACK_COLS=3;
   
    static const uint DEF_NTRACKS=8;
    static const uint DEF_UTRACKS=4;
   
    static const uint DEF_NROWS=64;
    static const uint DEF_UROWS=32;
    static const uint DEF_PRELOOK=15;
   
    static const uint CH_COL=0;
    static const uint NOTE_COL=1;
    static const uint CTRL_COL=2;
   
    //FIXME:
    static const int KEY_REP=1;
    static const int KEY_DEL=10;
   
#define ENT_CTRL_BIG 0.1
#define ENT_CTRL_SMALL 0.01
   
  public:
   
    static const uint NCHAR_CH=2;
    static const uint NCHAR_NOTE=3;
    static const uint NCHAR_CTRL=5;
   
    enum POS {NORM, BEG, END};
   
    Tracker(BitmapFont* font);
    virtual ~Tracker();
   
    void setPageStep(uint s);
    uint getPageStep();
   
    virtual bool giveInput(spl::Input& in);
   
    virtual void draw(Screen* s);
   
    //can not set view bigger than num 
    //number of rows to display
    //prelook is half of this
    void setNViewRows(uint v);
   
    //FIXME: no prelook
    void setNViewTracks(uint v);
   
    //WARNING: numViewRows must be reset after this...
    void setNRows(uint n);
    uint getNRows() const;
   
    void setNTracks(uint n); //INT: also updates chache size
    uint getNTracks() const;
   
    //used to check if change have happend to track since last read
   
    //use CACHED access below for speed
   
    //FIXME: switch track-row
    //tracks are always in a vector so:
    //for(getNtracks())getCH(i,getSelRow());
   
    bool haveNote(uint track, uint row) const;
    bool haveCtrl(uint track, uint row) const;
   
    uint getCh(uint track, uint row) const;
    int getNote(uint track, uint row) const; //returns key number(i.e 49=A4 etc)
   
    double getCtrlDouble(uint track, uint row) const;
    string getCtrl(uint track, uint row) const;
   
    void setCh(uint track, uint row, uint ch);
    void setNote(uint track, uint row, int key);
    void setCtrl(uint track, uint row, const string& ctrl);
    void setCtrl(uint track, uint row, double ctrl);
   
    void remNote(uint track, uint row);
    void remCtrl(uint track, uint row);
   
    void transposeTrackNotes(uint track, int tran);
    void transposeAllNotes(int tran);
    void transposeNote(uint track, uint row, int tran);

    //nothing happens if trying to step when at BEG/END
    //caller can check limits before stepping and decide what to to
    //if at limit: wraparound or stay
   
    //THERE are two markers: sel(edit) and play
    //stepping sel marker updates tracker view
    //stepping play marker updates cache
   
    //if this is true moving sel marker will move play marker the same amount
    //however using stepSelMarker() will not affect stepPlayMarker()
    //this is only for internal op's
    void setMarkersFollow(bool v=true);
    bool getMarkersFollow();
   
    //no wraparound
    //SEL MARKER
    //FIXME: all names to selSetRow etc not setSelRow for consistency!
    void setSelRow(uint n);
    void stepSelRow(int n); //-1 up, 1 down
   
    void setSelTrack(uint n);
    void stepSelTrack(int n); //-1 left, 1 right
   
    POS selRowPos() const;
    POS selTrackPos() const;
   
    uint getSelRow() const;
    uint getSelTrack() const;
   
    //only affects sel marker
    //default:off
    void setTrackSelWrap(bool on);
    bool getTrackSelWrap();
   
    //default:on
    void setAddStep(int step);
    int getAddStep();
    void setDelStep(int step);
    int getDelStep();
   
   
    //PLAY MARKER
    void setPlayRow(uint n);
    void stepPlayRow(int n); //-1 up, 1 down
    POS playRowPos() const;
    uint getPlayRow() const;

   
   
    //default:off, no, 1, 0,0,0
    void setOutline(bool on, bool bg, int sz, SDL_Color color);
   
    //this is only a flag no function assosiated
    bool getPlay() const;
    void setPlay(bool p);
   
    const bool* getPlayPtr() const;
   
    //caller data:
   
    //default:4
    void setOctave(int o);
    int getOctave() const;
   
    //set to neg to use channel default
    void setDefChannel(int c);
    int getDefChannel() const;
   
/*   void setDefCtrl(double c);
     double getDefCtrl();*/
   
   
    bool entryIsUsed(uint track, uint row);
   
    //relatively slow, however probably only called at save/load
   
    //total number of "added" data in tracker
    uint getNumEntries(); //same as ret.size()
   
    void clearAllEntries(); //call this first if setEntries() should be the only entries
    void clearTrackEntries(uint track);
   
   
    class StateEntry
    {
      public:
        void zero();
		
        uint track;
        uint row;
	
        uint ch;
        int note;
	
        bool have_note;
        //save ctrl as chars: can be special ctrl i.e 0000L might be linear interpolation from last to next
        char ctrl[6]; //MUST be null terminated!
        bool have_ctrl;
    };
   
   
    //returns all the 'used' entries filled with data+pos info
    //data returned is in sorted order: track0(if exist)[t0_rows(if any)] track1...
    void getEntries(vector<StateEntry>& ret);
   
    //set the entries in 'data' leaves the rest as is
    void setEntries(const vector<StateEntry>& data);
   
    //NOTE PREVIEW
   
    //i.e = no edit
    bool getOnlyPreview();
    void setOnlyPreview(bool v);
   
    uint getPreviewCh() const;
    bool havePreview() const;
    bool havePreviewCtrl() const;
   
    //ctrl preview is always a double
    int getPreviewNote() const;
    double getPreviewCtrl() const;
   
    //for PLAY marker!
    //returns data for selected row(i.e where marker is)
    //CACHED: good if these are used every sample
    //all tracks for selected row are cached
   
    //use these to be able to tell when cahce is updated:
    //check selHaveUpdate() if true => read all data wanted and call flagReadUpdate()
    //affects both have and get
    bool playHaveUpdate(uint track); //cache is updated since last flagReadUpdate
    void playFlagReadUpdate(uint track);//call this after updating external users of this class
   
    bool playHaveNote(uint track) const;
    bool playHaveCtrl(uint track) const;
   
    uint playGetCh(uint track) const;
    int playGetNote(uint track) const; //returns key number(i.e 49=A4 etc)
   
    double playGetCtrlDouble(uint track) const;
    string playGetCtrl(uint track) const;
   
   
  private:
   
    class RowCache
    {
      public:
        uint ch;
	
        bool have_note;
        int note;
	
        bool have_ctrl;
        double ctrl;
        string str_ctrl;
	
        //REMEBER: set differnet initialy
        //difference only iportant: cache_count only incremented when needed
        uint cache_count;
        uint last_read_count;
    };
   
    bool only_preview;
   
    //NOTICE: cache is always for 'c_row'
    vector<RowCache> cache_data; //ntracks big

   
    //make sure to run after everything that changes data, also dont run before init all
    void playCacheRowData();
   
    uint parseCh(const string& str) const;
    int parseNote(const string& str) const; //returns key number(i.e 49=A4 etc)
    double parseCtrl(const string& str) const;
   
    //WARNING: does not re-cache
    void fillDefTrackData(SheetView* sheet, uint start, uint num, uint def_chan);
   
    //builds a string with format: 001 021 etc
    string getZeroPadded(int num, int len);
   
    SheetView* buildLeftSheet();
    void setSheetOpt(SheetView* sheet);
    SheetView* buildTrackSheet(uint def_chan);
   
    void updateView();
    void selPositionMarker(); //after c_row,c_col
    void playPositionMarker(); //after c_row,c_col
   
    virtual void setX(int x);
    virtual void setY(int y);
   
    class SelMarker : public Component
    {
      public:
        SelMarker(uint w=0, uint h=0);
	
        //if fill is on Thickness ignored
        //default:false
        void setFill(bool v);
        void setDim(uint w, uint h);
        void setColor(const SDL_Color& c);
        void setColor(Uint8 r, Uint8 g, Uint8 b);
	
        void setThickness(uint v);
	
        void draw(Screen* s);
	
      private:
        bool fill;
        uint thick;
        SDL_Color color;
    };
   
    uint page_step;
   
    bool have_preview;
    bool have_preview_ctrl;
    int preview_key;
    uint preview_ch;
    double preview_ctrl;
   
   
    int add_step;
    int del_step;
    bool track_sel_wrap;
   
    Container* cont;
    SelMarker* row_marker;
   
    SelMarker* note_marker;
    SelMarker* ctrl_marker;
   
    //inless spec. all c_col/row are for edit(select) marker
    uint c_col;
    uint c_track;
    uint c_row;
   
    //edit and play are separate:
    uint play_row;
    SelMarker* play_marker;
    bool markers_follow;
   
    //row offsets
    uint r_off;
    uint r_sel;
    uint nrows;
    uint urows;
   
    uint r_pre_look;
   
    int xpad;
   
    //c_off:column offset
    uint c_off;
    uint c_sel;
    uint ntracks;
    uint utracks;

    BitmapFont* font;
   
    SheetViewLayout* view;
   
    SheetView* left_sheet;
   
    vector<SheetView*> tracks;
   
    bool outline_fill;
    bool outline;
    int outline_sz;
    SDL_Color c_outline;
   
    //FIXME: name 
    int octave;
    int ch;
   
    //play flag needs to be in this class:entering ctrl column needs to off play
    //double ctrl;
    bool play;
   
    uint subcol;
   
    bool do_ctrl;
    uint do_ctrl_c;
    string do_ctrl_old;
   
    SDL_Color color_noedit;
    SDL_Color color_edit;
   
    static const string empty_ctrl;
    static const string empty_note;
   
    bool first;
    int key_tick;
    int key_d_tick;
   
    int note_key_d_tick;
    int note_key_tick;
   
    bool c_have_key;
    int c_key_press;
   
    //crating a stringstream is TOO EXPENSIVE to be done often(~44100 times a second)
};

typedef Tracker::StateEntry TrackerEntry;

#endif
