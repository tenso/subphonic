#ifndef M_BENCH_H
#define M_BENCH_H

#include <map>
#include <queue>

#include "../gui/gui.h"
#include "../gui/gui_select.h"
#include "m_state.h"
#include "progstate.h"

class MainBench;

typedef map<SoundComp*, CompStates*> state_map;
typedef state_map::iterator state_map_it;

//this contains all the components 'CompStates'
//i.e all states for all comps
class BenchStates
{
  public:
    BenchStates();
   
    //will free all remaining states
    ~BenchStates(); 
   
    //returns NULL of not present
    CompStates* getCompStates(SoundComp* parent);
   
    void addCompStates(SoundComp* parent, CompStates* state);
    void removeCompStates(SoundComp* parent);
   
    unsigned int numCompStates();//i.e number of components that have saved state
   
    state_map* getAllStates();
   
    //goes through all components in class and applies the state at index in CompStates
    //seek_backward= applies state found to the "left" of 'index' if it is empty
    //otherwise apllies *index'
    //FIXME: probably best without this here
    void applyAll(uint index, bool seek_backward=false);

   
    //this is a special: used by file loading
    //WARNING: leaves currently applied state in a mess: caller must reset to what it was before calling this
    void resaveAllInCurrLoadVer(int from_version);
   
  private:
    state_map states; //contains all component's CompState(holder of all states)
};

/*
  class BenchMarker
  {
  public:
  enum OPTION{NORMAL, NOP, REP};
   
  BenchMarker()
  {
  d=0;
  o=NORMAL;
  }
   
   
  //NORMAL: 'd' is index 
  //NOP: do nothing
  //REP: repeat last 'd' more times
   
  int d;
  opt o;
  };*/

//per "screen"

//FIXME: class not in best shape(esp INPUT handling...)

class MainBench
{
    //FIXME: make this optional
    static const uint NUM_SLOTS=112;
   
  public:
    static const uint MAX_STATES=10000;
   
    MainBench(int x, int y, Screen* s, BitmapFont* f);
    ~MainBench();
   
    bool handleReleaseOver();
    bool handleClickOver(int x, int y, int button, bool l_shift);
   
    //applies state with index=getCurrentMarkerIndex(), for all comps
    void setAllStates();
    void setCurrentState();
   
    //states++:
    //checks progstate for getStateTickAll()
    void tickStates();
   
    Sample* makeSample();
    void handleMouseMotion(int x_m, int y_m);
    void draw();
   
    //sets current state to 'comp's (new if no one present)
    void currentState(SoundComp* comp);
   
    //FIXME: change name for these; NOT clear what they do
    //they save the state for the CURRENT selected index on the bench
    //for ALL or the CURRENTLY selected component
    bool saveCurrentStateIndex();
    bool saveAllStateIndex();
    bool delCurrentStateIndex();
    bool delAllStateIndex();
    bool delAllForCurrentStateIndex();
   
    //moves states starting with getCurrentMarkerIndex() to the right if 'off'>0; pushes all statest infront
    //if bench to small; statest over last index are discarded, spaces left to the left are free
    //to the left if 'off'<0 grabbing all to theright, states to the left are discarded
    void moveCurrentStateIndex(int off);
    void moveAllStateIndex(int off);
   
   
    /*****************************/
    /*list(many indices) copy/del*/
    /*****************************/
    void clearSelList();
    uint inSelList(); //set/unset in handleClickOver +(SHIFT)
    void currentDelSelList();
    void allDelSelList();
    void currentCopySelListToMarkerIndex();
    void allCopySelListToMarkerIndex();
    /*end*/
   
   
   
    //deletes all states that are assosiated with soundcomp
    void removeCompState(SoundComp* parent);
      
    uint getNumSlots();
    uint getMaxStates();
   
    BenchStates* getStates();
   
    //MARKER
    void setStartMarkerIndex(uint m);
    uint getStartMarkerIndex();
   
    void setStopMarkerIndex(uint m);
    uint getStopMarkerIndex();
   

    uint getCurrentMarkerIndex();
    //checks progstate for getStateTickAll()
    void setCurrentMarkerIndex(uint m, bool apply_state=true);
   
   
    /*******************/
    /*Marker sequencing*/
    /*******************/
   
    //FIXME: names to 'seq'SetM... etc
   
    //will set state to first mark and then every tickStates will follow list
    void setMarkerList(const list<int>& markers, bool all, bool apply_now);
   
    void loopMarkerList(bool val);
   
    uint getMarkerListIndex();
   
    //sets to to end if index>max
    void setMarkerListIndex(uint index, bool all, bool apply_now);
   
    //only seeks to ends
    void seekMarkerListIndex(int dir, bool all, bool apply_now);
   
    //resume normal op
    void voidMarkerList();
   

   
  private:
   
    void applyStates(); //small helper to set either all or current according to progstate
   
    void applyCurrent(uint index, bool seek_backward);
    CompStates* getCurrentCompStates();
    void setCurrentCompStates(CompStates* cs);
   
    CompStates* current_state;
   
   
    class C_StateOff : public Action
    {
      public:
        C_StateOff(MainBench* src, int dir);
        void action(Component* c);
	
      private:
        MainBench* src;
        int dir;
    };
   
   
    uint nstates;
    uint cstate;
    uint coff; //current visual off
   
    int startmarker;
    uint stopmarker;
    SlotSelect* slotsel;
    BenchStates states;
         
    Screen* s;
    Container* cont;
   
    Label* c_lab;
    Pixmap* c_lab_bg;
   
    int x,y;
   
    static ProgramState& progstate;
   
    int min_index;
    int max_index;
   
    int c_marker_i;
    bool markers_loop;
    list<int> markers;
    list<int>::iterator c_marker;
   
    vector<int> sel_list;
};

#endif
