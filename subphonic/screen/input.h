#ifndef SPL_INPUT_H
#define SPL_INPUT_H

#include <SDL.h>

#include <string.h>
#include <string>
#include <sstream>

#include <iostream>

#include "../util/types.h"

namespace spl
{

using namespace std;

#define JOY_XAXIS 0
#define JOY_YAXIS 1
#define JOY_MAX 32767

//WARNING: FIXME: make list!
#define MAX_JOYSTICKS 2

#define NUM_SDLKEYS SDLK_LAST-SDLK_FIRST

/*FIXME: 
  JOY HAT NOT IMPLEMENTED!
  KEEP keyModSet() etc? Better alt exist.
  * */

/*IF events cant be missed, i.e count the number of times SDLK_a is pressed
 *-------------------------- 
 * while(SDL_PollEvent(&ev))
 * {
 *   in.setEvent(ev);
 *   if(in.keySet(SDLK_a))do something
 * }
 *--------------------------
 *IF only current event state is important, i.e SDLK_a is down; dont care how many times it was pressed
 *-------------------------- 
 * while(SDL_PollEvent(&ev))in.setEvent(ev);
 * 
 * if(in.keySet(SDLK_a))do something
 * -------------------------
 */

/*to read input as string(i.e text input):
 * enableUnicode(); (for now)
 * setEvent() - as usual
 * inputStr() - 
 *              
 */

class Input
{
  public:
    Input();
    ~Input();
   
    //totaly resets structure
    void reset();
   
    //use this to update this class from SDL_Event
    //return true if event handled, if false caller need to  handle it
    bool setEvent(SDL_Event& event);
   
    //if using keyEvent/mouseEvenet/joyEvent, this is needed to reset
    //the flags if an event happended unless reset=true in calls to funcs
    //FIXME: change to keyPressEvent() etc...
    void resetEvent();
    bool keyEvent(bool reset=true); //did a key event happen just now
    bool joyEvent(int id=0, bool reset=true);
    bool mousePressEvent(bool reset=true);
    bool mouseReleaseEvent(bool reset=true);
      
   
    /*KEYBOARD*/
   
    //reset=false =>generate 'pressed' state untill key released
    //reset=true => one press -> one state | after return no more untill release+press again
    //(same for mouse/joy buttons)
   
    //FIXME: a new key presses(as nextMButton())
   
    //this is set untill release unless reset=true
    bool keySet(SDLKey k, bool reset=false);
   
    //these are set untill resetKeyPressRel() or reset()
    //example code:
    /*main{
     * for(events)in.setEvent(ev)
     * if(in.keyPress(SDLK_a))start_key_action
     * if(in.keyRel(SDLK_a))stop_key_action
     * in.resetKeyPressRel()
     * }
     */
    //yet another key handeling structure
    int numKeyPress();
    int numKeyRel();
    bool keyPress(SDLKey k);
    bool keyRel(SDLKey k);
    void resetKeyPressRel();//NOTICE: keySet() not affected!
   

    //gets 
    //int getNewPress(list<SDLKey>& ret);
   
    //this shold not be on if char's are not supposed to be read: otherwise they will be saved
    //every call to setEvent() [upto MaxStrlen]
    //default: false
    void setUseInputStr(bool val);
    bool getUseInputStr();
   
    //make sure to use setUnicode(true) before SDL_PollEvent()
    //and setUseInputStr(true)
    //returns -1 with error
    //or length of ret
    //returns input since last call (append this to total input if needed)
    //NOTICE: will return '\n','\t' etc
    int inputStr(string& ret);
   
    //default: 100
    void setMaxStrLen(uint len);
    uint getMaxStrLen();
   
    //global, hence static
    static void setUnicode(bool val);
    static bool getUnicode();
   
    //delay is initial before starting to repeat, 'interval' apart thereafter
    //this repeats keydown(no keyup between) so without reset and with keySet() this 
    //does nothing; however with reset it does, 
    //ALSO inputStr() is always repeted
    //input: milliseconds
    //set delay to 0 to disable
    //default: off(delay=0)
    //defines: SDL_DEFAULT_REPEAT_DELAY,  SDL_DEFAULT_REPEAT_INTERVAL
    //NOTICE: this is a global(hence static)
    static void setKeyRepeat(int delay, int interval);
   
    /*instead of modifiers this is probably cleaner:
     *if(keySet(SDLK_LCTRL))
     *  {
     *	if(keySet(SDLK_a))do something
     *  }
     */
   
    //modefiers: KMOD_NONE,KMOD_LSHIFT,KMOD_RSHIFT,KMOD_LCTRL,KMOD_RCTRL,KMOD_LALT, KMOD_RALT,
    //KMOD_LMETA, KMOD_RMETA, KMOD_NUM, KMOD_CAPS, KMOD_MODE, KMOD_CTRL, KMOD_SHIFT, KMOD_ALT, KMOD_META
   
    //returns true if any mod in one_off is present:
    //haveKeyMod(SDLK_x, KMOD_LSHIFT&KMOD_LCTRL, false); checks for left-shift and left-ctrl
    bool keyModSet(SDLKey k, SDLMod mod, bool reset=false);
      
    //returns true if any mod is present i.e even CAPS, NUMLOCK etc
    bool anyKeyModSet(SDLKey k, bool reset=false); 
   
    //returns current modifier
    SDLMod getKeyMod(SDLKey k, bool reset=false);
   
    //same as above but 'global', i.e all modefiers down
    //better use reset=true or run reset() to reset
    bool keyModSet(SDLMod mod, bool reset=false);
    bool anyKeyModSet(bool reset=false); 
    SDLMod getKeyMod(bool reset=false);
   
    /*MOUSE*/
   
    //position of mouse in app-window
    int xMouse();
    int yMouse();
    //relative motion of mouse since last event, 
    //reset=true sets it to 0 until next setEvent, reset=false keeps value
    int xMouseMove(bool reset=true);
    int yMouseMove(bool reset=true);
   
    //'std' buttons
    bool leftMB(bool reset=true);
    bool middleMB(bool reset=true);
    bool rightMB(bool reset=true);
   
    //same as above but any button
    //n=0 left ;  n=1 middle ;  n=2 right
    bool mButton(int n, bool reset=true);
    bool mButtonUp(int n, bool reset=true);
   
    int mWheelUp(bool reset=true);
    int mWheelDown(bool reset=true);
   
   
    /*IS THIS GOOD? : + for keyEvent() */
    /*EVENT QUEUE*/
    /*enum MODE {PRESS, RELEASE};
      enum BUTTON{LEFT=0, MIDDLE, RIGHT, WHEEL_UP, WHEEL_DOWN};
      typedef pair<BUTTON, MODE> mb_ev;
   
      //dont forget this if using queue
      void setUseEventQueue(bool v);
      bool getUseEventQueue();
   
      //def: 100
      void setEventQueueSize(uint n);
      void clearEventQueue();
   
      //this is a fifo-queue, if it is not polled it will fill up,
      //when max is reached oldest are dropped
      mb_ev nextMouseEvent();*/
   
    /***************/
   
   
    //CHECK NEW PRESSES
   
    //this will step through all newly pressed buttons in order and return -1 when no more pressed buttons
    //i.e only signals a press, not hold(use mButton with reset=false)
    //ALWAYS run startNextMButton() before starting step
    //if not called before button is released, event is missed
   
    /*	startNextMButton();
		int next=in.nextMButton();
		while(next!=-1){next=in.nextMButton();}*/
   
    void startNextMButton();
    int nextMButton();
   
   
    //did it happen?
    /*bool mousePress(int n, bool reset=true);
      bool mouseRelease(int n, bool reset=true);*/
   
  
   
    /*JOYSTICK*/
   
    void printJoysticks()
    {
        cout << "joysticks: " << endl;
        for(int i=0;i<numJoysticks();i++)
            cout << "#" << i << " : " << SDL_JoystickName(i) << endl;
    }
    string joyInfo(int id) const;
   
    //int numJoyActive();
   
    int numJoysticks()
    {
        return SDL_NumJoysticks();
    }
   
    //if using many joysticks dont forget id param
    bool openJoy(int id=0);   
    void closeJoy(int id=0);
    bool joyActive(int id=0)
    {
        if(id>=MAX_JOYSTICKS)
        {
            cout << "num joysticks > 2 not implemented. fixme asap!" << endl;
        }
	
        return joysticks[id].open();
    }
   
    //return current position of axis
    //for "digital" joystick moves try this with reset on and check if pos is JOY_MAX
    //in screen coord, i.e. y-axis "up" is negative
    int joyAxisPos(int axis, bool reset=false, int id=0);
   
    //i.e -1,1
    double joyAxisPosNormal(int axis, bool reset=false, int id=0);
   
    //how axis moved just now:, joyMove()/(float)JOY_MAX for range[-1.0,1.0]
    int joyAxisMove(int axis, bool reset=false, int id=0);
   
    bool joyButton(int button, bool reset=false, int id=0);
   
    uint nJoyButton(int id=0);
    uint nJoyAxis(int id=0);
   
  private:
    uint uni_maxlen;
    bool uni_on;
    string uni_str;
      
    bool mouse_press,mouse_rel;
    bool keys[SDLK_LAST]; //NUM_SDLKEYS should be here, but new direct mapping: keys[SDLK_x]
    SDLMod keymod[SDLK_LAST]; //maps same as keys
    SDLMod allkeymod;
   
    int mouse_x;
    int mouse_y;
    int mouse_rx;
    int mouse_ry;
    bool* buttons;
    bool* buttons_up;
    bool* new_buttons;
    int startnextmb;
    int   nbuttons;
    int mwheel[2];
   
    bool keyevent;
   
    class Joystick
    {
      public:
        Joystick()
        {
            joy_id=-1;
            joy=NULL;
            joyevent=false;
        }
	
        //FIXME: if you call init better call close()
        void init(int id, int naxis, int nbutton, SDL_Joystick* sdljoy); 
        bool open()
        {
            return joy!=NULL;
        }
        void close();
      
        //FIXME:  private:
        int* joy_pos; //depends on num axis
        int* joy_move;
        bool* joy_buttons;
        int joy_naxis;
        int joy_nbutton;
        bool joyevent;
        int joy_id;
        SDL_Joystick* joy;
	
    };
    Joystick joysticks[MAX_JOYSTICKS];
   
    bool keys_press[SDLK_LAST];
    bool keys_rel[SDLK_LAST];
    int num_keys_press;
    int num_keys_rel;
};

}
#endif
