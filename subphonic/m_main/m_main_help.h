#ifndef M_MAINHELP_H
# define M_MAINHELP_H

# include "../util/debug.h"
# include "m_con.h"
# include "../g_comp/g_all.h"
# include "progstate.h"
# include "m_state.h"
# include "m_bench.h"

/*The FILESTRUCTURE:
 * FileHead 
 * COMPONENTS
 * LINES
 * STATE
 * */

class ProgramFile
{
  public:
   
    ProgramFile();
   
    class Head
    {
      public:
        char name[2]; //spells kw
        int ver;
        int state_size; //the size following  (1)
        int unused[4];
    };
   
    //actual data
   
    Head head;
   
    //(1) STATE data
    int current_slot;
    int stop_marker;
    int start_marker;
   

};

/*FILE_STRUCTURE (after ProgramFile) 
 * 
 * -----------------------------------
 * COMPONENTS
 *  int           expected_bytes  - if program dissagree it can still load what it can
 *                                   otherwise file read would get totaly unsynced
 *                                  i.e this is the sizeo of DiskComponent(not including num)
 *  int           num             - number of comps
 *  DiskComponent comps[num]      - see g_common.h
 * 
 *--------------------------------------
 * 
 * LINES
 *  int     expected_bytes        -(not including num)
 *  int     num                   -number of lines(total)
 *  (num* ) linesdata as:
 *    ConPair con                 -g_common.h / the actual lines, try to keep this first so atleast it can be read
 *    Uint8   color[3]            -line color
 *  
 *-------------------------------------
 * 
 * STATE
 *   int unused                            -
 *   int num                               - number of comps that have saved their state
 *   (num *) Component's All States        - 
 *      SCType parent_type                   - this is good for error check
 *      int    p_seqno                       - parents sequence number
 *      int    count                         - number of states for this comp
 *      
 *      (count *) single state                
 *          int   state_size                    - size of state saved(bytes)
 *          int   index                         - state index on bench
 *          char* data[state_size]              - actual state data SoundComp->getState()
 *
 *--------------------------------------
 */ 

enum m_err {OK=0, ERR, FILE_NOTFOUND_ERR, FILE_IO_ERR, FILE_CORRUPT_ERR, FILE_CREATE_ERR, OTHER_ERR, CON_FAIL_ERR, 
            SOME_COMP_FAIL, BRIDGE_NOTFOUND_ERR, COMP_FAIL_ERR, SOME_STATE_FAIL, COMP_NOTFOUND, 
            STATE_SKIP, STATE_PARTIAL_READ, NO_COMP_ADDED, SOME_CON_FAIL, STATE_NOTLOADED};

//CONNECTIONS
m_err loadConnections(Container* cont, Connection* connection, istream* fin, int sc_seq_off);
m_err saveConnections(Connection* connection, ostream* of);
bool addConnection(Container* cont, Connection* connection, ConPair cp);

//COMPONENTS
m_err saveComponents(Container* cont, ostream* of);

//STATES
m_err saveAllStates(BenchStates* states, ostream* out);
m_err appendStateToFile(ostream* out, CompStates* state);

//try force tryes to set comp state even if there is a missmatch from file: this can give strange states
//but SHOULD be OK, this is why new data added to a comp state shold be added last...
m_err loadAllStates(Container* cont, BenchStates* states, istream* in , int sc_seq_off, bool try_force=false);

enum LOADMODE {ALL, SKIP_PROG_SINGLE_COMPS};

m_err loadSingleState(Container* cont, BenchStates* states, istream* in, 
int sc_seq_off, LOADMODE=ALL, bool try_force=false);



/*******************************************/
/*MOUDLEBRIDGE needs some special treatment*/
/*******************************************/

//returns a bridge ready to use in bridge_ret, 
//future load should append it's 'prefix'; module is expected to be in prefix+name
//i.e module name should be relative and prefix makes it "absolute"

//NOTICE: only checks for comps in 'prefix' no further search
m_err loadModuleBridge(const string& name, const string& prefix, BitmapFont* f
, SoundComp** bridge_ret, MIDIcall* midicall, GroundMixer& ground, bool try_force=false);


//returns the bridge's components loaded into 'cont' with the actual bridge returned in 'bridge'
//it also exist in cont(for loading all states it is probably most straightforward way)
//so after loading lines/states etc better remove 'bridge' from cont:
//cont->rem(bridge)
//
//needs 'load_prefix' : this might load bridges, searches for these in load_prefix(all modules from same dir)
//HOWEVER if not found there also check module_path...
//
//needs 'states' : need to make all GStatetick loaded have 'states'
m_err loadComponentsModule(Container* cont, BitmapFont* f, istream* fin, int sc_seq_off
,SoundComp** bridge, MIDIcall* midicall, const string& load_prefix
,BenchStates* states, const ProgramFile& filestate, GroundMixer& ground);


//sel_slot_index is the index for which to append the states to the comps
m_err loadStatesModule(Container* cont, BenchStates* states, istream* in, int sc_seq_off
, int sel_slot_index, bool try_force=false);



/*********/
/*HELPERS*/
/*********/

//makes a new comp and adds it to 'cont'
SoundComp* newComponent(Container* cont, BitmapFont* f, SCType id, int x, int y, GroundMixer& ground);

//returns comp with 'seqno' in 'cont'
SoundComp* findComp(Container* cont, unsigned int seqno);

#endif
