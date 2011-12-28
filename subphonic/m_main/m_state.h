#ifndef M_STATE_H
#define M_STATE_H

#include "../g_comp/g_common.h"

/*There can be some confusion:
 * SCState is one state for a specific SoundComp i.e one slot on bench
 * CompStates is all the states for one SoundComp
 * BenchStates(m_bench.*) contains all the SoundComp's CompStates*/

/*wrapper classes to ease state-handling*/

/*single state*/
class SCState
{
  public:
    //if sc!=NULL, it runs setup(sc)
    SCState(SoundComp* sc=NULL);
    ~SCState();
   
    SCState(const SCState& from);
    SCState& operator=(const SCState& from);
   
    bool reset();
   
    //set sc as parent and save that parents state
    bool setup(SoundComp* sc);
   
    //make sure isSetup before this
    //applyes the state hold herein on 'sc' form setup()
    SoundComp* applyState();
   
    bool isSetup();
   
    //make new event with this as base going towards next...
    //int makeInterpolated(SoundCompEvent* dst, SoundCompEvent* next, double pos);
    void setParent(SoundComp* s);
    SoundComp* getParent();
    SoundCompEvent* getEvent();
   
  private:
      
    SoundComp* parent;
    SoundCompEvent* event;
};

/*many states(events) per sound component
 * this takes a parent and manages it's sates, i.e save to and index etc...
 * INDEX here is the "virtual" index the state occupies
 * 
 * always ok to save over old, del many times etc...
 */
class CompStates
{
  public:
    typedef map<int, SCState*>::iterator s_it;
    typedef map<int, SCState*> s_map;
   
    CompStates(SoundComp* parent);
    ~CompStates();
   
    //state saved at index?
    bool indexInUse(int i);
   
    //saves 'parent' current state in index 'i'
    bool saveIndex(int i);
      
    //set's parent state to that in index, or do nothing if state index unset(OK)
    //returns true if state was applyed
    bool applyIndex(int i);
   
    //returns true if del occured
    bool delIndex(int i);
   
    //returns true if del occured
    bool delAll();
   
    //returns index to first setup state beginning with beg and going backwards
    //-1 if no such exist
    //this can be used to find the state to set if applying closest left at unused states
    int lastUsedIndex(int beg);
   
    //returns index to nearest setup state beggining with index going forward
    //-1 if no such exist
    int nextUsedIndex(int index, int stop);
   
    //copy to self ok
    void copyIndex(int from, int to);
   
    /*Moves states starting with 'index':
     * 
     *To the right if 'push'>0; pushes all states infront of it
     *if n to small; statest over last index are discarded, spaces left to the left are freed
     *
     *To the left if 'push'<0 grabbing all to theright with it
     * states to the left are discarded
     */
   
    //returns pushes done, +- will not push and index over min_i/max_i i.e they are discarded
    int pushStatesFromIndex(int index, int push, int min_i, int max_i);
   
   
    /*void setState(int index, int stop);
      void setInterpolated(int first, int next, int current, int stop);*/
   
    void reset();
   
    SoundComp* getParent();
   
    //WARNING: this will make new state if one does not exist
    //check with indexInUse() first if that is not wanted
    SCState* getState(int index);
   
    //SCState* getStateNoCreate(int index);
    //SCState* newState(int index);
   
    unsigned int getNumUsed() const;
   
    s_map& getStates();
   
  private:
    void setIndexTo(int i, SCState* state);
   
    SoundComp* parent;
   
    map<int, SCState*> states;
   
   
};

#endif
