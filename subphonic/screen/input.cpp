#include "input.h"
#include "../util/debug.h"

namespace spl
{

Input::Input()
{
    nbuttons = 5; //FIXME: default
    buttons = new bool[nbuttons];
    buttons_up = new bool[nbuttons];
    new_buttons = new bool[nbuttons];
   
    uni_on=false;
    uni_maxlen=100;
   
    reset();
}

Input::~Input()
{
    delete[] buttons;
    delete[] buttons_up;
    delete[] new_buttons;
}

bool Input::setEvent(SDL_Event& event)
{
   
    bool ret=true;
    keyevent=false;
   
    //KEYS
    if(event.type==SDL_KEYDOWN)
    {
        keyevent=true;
        keys[event.key.keysym.sym] = true;
	
        keys_press[event.key.keysym.sym] = true;
        num_keys_press++;
	
        keymod[event.key.keysym.sym]=event.key.keysym.mod;
	
        allkeymod = (SDLMod)(event.key.keysym.mod);
	
        if(uni_on)
        {
            if(uni_str.length()>=uni_maxlen)
            {
                ERROR("input char str overflows...(reset): left setUnicode() on?");
                uni_str.clear();
            }
	     
            Uint16 c = event.key.keysym.unicode;
            if(c!=0)
            {
                if( (c&0xFF80) == 0)
                {
                    //ASCII
                    char a = c&0x7F;
                    uni_str.insert(uni_str.end(), a);
                }
                else 
                {
                    //international
		       
                    char a = c&0x7F;
                    uni_str.insert(uni_str.end(), a);

                    //cout << "unimplemented, got: " << c << endl;
                }
            }
        }
    }
   
    else if(event.type==SDL_KEYUP)
    {
        keyevent=true;
        keys[event.key.keysym.sym] = false;
	
        keys_rel[event.key.keysym.sym] = true;
        num_keys_rel++;
	
        keymod[event.key.keysym.sym]=event.key.keysym.mod;
	
        allkeymod = (SDLMod)(event.key.keysym.mod);
    }
   
   
    //MOUSE
    else if(event.type==SDL_MOUSEBUTTONUP)
    {
        mouse_press=false;
        mouse_rel=true;
	
        /*FIXME: does this always work?*/
        D_ASSERT(event.button.button<=nbuttons && event.button.button>0);
        buttons[event.button.button-1]=false;
        buttons_up[event.button.button-1]=true;
	
        new_buttons[event.button.button-1]=false;
    }
    else if(event.type==SDL_MOUSEBUTTONDOWN)
    {
        mouse_press=true;
        mouse_rel=false;
	
        D_ASSERT(event.button.button<=nbuttons && event.button.button>0);
        buttons[event.button.button-1]=true;
        buttons_up[event.button.button-1]=false;

        new_buttons[event.button.button-1]=true;
	
        if(event.button.button==SDL_BUTTON_WHEELUP 
        || event.button.button==SDL_BUTTON_WHEELDOWN)
        {
            //WARNING: button 4,5 is mwheel in SDL
            /*if(event.button.button>=4)*/
            mwheel[event.button.button-4]++;
	     
            new_buttons[event.button.button-1]=true;
        }
	
    }
    else if(event.type==SDL_MOUSEMOTION)
    {
        mouse_x=event.motion.x;
        mouse_y=event.motion.y;
        mouse_rx=event.motion.xrel;
        mouse_ry=event.motion.yrel;
    }
    //joystick
    else if(event.type==SDL_JOYAXISMOTION)
    {
        D_ASSERT_M(joysticks[event.jaxis.which].open(), "event for unopened joystick!");
	
        joysticks[event.jaxis.which].joyevent=true;
	
        //calc diff from last
        joysticks[event.jaxis.which].joy_move[event.jaxis.axis] = 
            event.jaxis.value - joysticks[event.jaxis.which].joy_pos[event.jaxis.axis];
	
        joysticks[event.jaxis.which].joy_pos[event.jaxis.axis]=event.jaxis.value;
    }
    else if(event.type==SDL_JOYBUTTONDOWN)
    {
        D_ASSERT_M(joysticks[event.jaxis.which].open(), "event for unopened joystick!");
	
        joysticks[event.jbutton.which].joyevent=true;
        joysticks[event.jbutton.which].joy_buttons[event.jbutton.button]=true;
    }
    else if(event.type==SDL_JOYBUTTONUP)
    {
        D_ASSERT_M(joysticks[event.jaxis.which].open(), "event for unopened joystick!");
	
        joysticks[event.jbutton.which].joyevent=true;
        joysticks[event.jbutton.which].joy_buttons[event.jbutton.button]=false;
    }
    else ret=false;
         
    return ret;
}

void Input::resetEvent()
{
    allkeymod=KMOD_NONE;
   
    keyevent=false;
   
    mouse_press=false;
    mouse_rel=false;
   
    for(int i=0;i<MAX_JOYSTICKS;i++)joysticks[i].joyevent=false;
}

void Input::reset()
{
    for(int i=0;i<SDLK_LAST;i++)keys[i]=false;
    mouse_rx=0;
    mouse_ry=0;
   
    for(int i=0;i<nbuttons;i++) buttons[i]=false;
    for(int i=0;i<nbuttons;i++) buttons_up[i]=false;
    mwheel[0]=mwheel[1]=0;
   
    keyevent=false;
    for(int i=0;i<SDLK_LAST;i++)keymod[i]=KMOD_NONE;
   
    allkeymod=KMOD_NONE;
   
    for(int i=0;i<MAX_JOYSTICKS;i++)joysticks[i].joyevent=false;
   
    startnextmb=0;
   
    resetKeyPressRel();
}


//query
bool Input::keySet(SDLKey k, bool reset)
{
    bool val=keys[k];
    if(reset)keys[k]=false;
   
    return val;
}

char Input::firstAsciiKey()
{
    for (int i=0;i<SDLK_LAST;i++)
    {
        if (keys[i])
        {
            keys[i] = false;
            return (char)i;
        }
    }
    return 0;
}

bool Input::keyEvent(bool reset) //did a key event happen just now
{
    bool ret=keyevent;
    if(reset)keyevent=false;
    return ret;
}

int Input::numKeyPress()
{
    return num_keys_press;
}

int Input::numKeyRel()
{
    return num_keys_rel;
}

bool Input::keyPress(SDLKey k)
{
    return keys_press[k];
}

bool Input::keyRel(SDLKey k)
{
    return keys_rel[k];
}

//NOTICE: keySet() not affected!
void Input::resetKeyPressRel()
{
    for(uint i=0;i<SDLK_LAST;i++)
    {
        keys_press[i]=false;
        keys_rel[i]=false;
    }
    num_keys_press=0;
    num_keys_rel=0;
}



//default: false
void Input::setUnicode(bool val)
{
    //0:disabled
    int last =  SDL_EnableUNICODE(-1);
   
    if(val)
    {
        if(!last)
        {
            SDL_EnableUNICODE(1);
        }
    }
    else
    {
        if(last)
        {
            SDL_EnableUNICODE(0);
        }
    }
}

bool Input::getUnicode()
{
    return (bool)SDL_EnableUNICODE(-1);
}

void Input::setUseInputStr(bool val)
{
    uni_on=val;
}

bool Input::getUseInputStr()
{
    return uni_on;
}


int Input::inputStr(string& ret)
{
    if(!uni_on)
    {
        ERROR("use disabled");
        return -1;
    }
    if(!getUnicode())
    {
        ERROR("unicode disabled");
        return -1;
    }
   
   
    ret=uni_str;
    uni_str.clear();
   
    return ret.size();
}

void Input::setMaxStrLen(uint len)
{
    uni_maxlen=len;
}

uint Input::getMaxStrLen()
{
    return uni_maxlen;
}

void Input::setKeyRepeat(int delay, int interval)
{
    int err = SDL_EnableKeyRepeat(delay, interval);
   
    if(err!=0)
	{
		ERROR("failed");
	}
}


bool Input::keyModSet(SDLKey k, SDLMod mod, bool reset)
{
    SDLMod v = keymod[k];
    if(reset)keymod[k]=KMOD_NONE;
   
    return (v&mod)!=KMOD_NONE;
}

bool Input::anyKeyModSet(SDLKey k, bool reset)
{
    SDLMod v = keymod[k];
    if(reset)keymod[k]=KMOD_NONE;
   
    return v!=KMOD_NONE;
}

SDLMod Input::getKeyMod(SDLKey k, bool reset)
{
    SDLMod v = keymod[k];
    if(reset)keymod[k]=KMOD_NONE;
   
    return v;
}

bool Input::keyModSet(SDLMod mod, bool reset)
{
    SDLMod v = allkeymod;
    if(reset)allkeymod=KMOD_NONE;
   
    return (v&mod)!=KMOD_NONE;
}

bool Input::anyKeyModSet(bool reset)
{
    SDLMod v = allkeymod;
    if(reset)allkeymod=KMOD_NONE;
   
    return v!=KMOD_NONE;
}

SDLMod Input::getKeyMod(bool reset)
{
    SDLMod v = allkeymod;
    if(reset)allkeymod=KMOD_NONE;
   
    return v;
}


int Input::xMouse()
{
    return mouse_x;
}

int Input::yMouse()
{
    return mouse_y;
}

int Input::xMouseMove(bool reset)
{
    int t = mouse_rx;
    if(reset)mouse_rx=0;
    return t;
}

int Input::yMouseMove(bool reset)
{
    int t = mouse_ry;
    if(reset)mouse_ry=0;
    return t;
}

bool Input::leftMB(bool reset)
{
    bool val = buttons[0];
    if(reset)buttons[0]=false;
    return val;
}

bool Input::middleMB(bool reset)
{
    bool val = buttons[1];
    if(reset)buttons[1]=false;
    return val;
}

bool Input::rightMB(bool reset)
{
    bool val = buttons[2];
    if(reset)buttons[2]=false;
    return val;
}


bool Input::mButton(int n, bool reset)
{
    bool val = buttons[n];
    if(reset)buttons[n]=false;
    return val;
}

bool Input::mButtonUp(int n, bool reset)
{
    bool val = buttons_up[n];
    if(reset)buttons_up[n]=false;
    return val;
}

void Input::startNextMButton()
{
    startnextmb=0;
}


int Input::nextMButton()
{
    for(int i=startnextmb;i<nbuttons;i++)
    {
        if(!new_buttons[i])continue;
	
        new_buttons[i]=false;
        startnextmb=i;
        return i;
    }
   
    return -1;
}


int Input::mWheelUp(bool reset)
{
    int ret = mwheel[0];
    if(reset)mwheel[0]=0;
    return ret;
}
int Input::mWheelDown(bool reset)
{
    int ret = mwheel[1];
    if(reset)mwheel[1]=0;
    return ret;
}


bool Input::mousePressEvent(bool reset)
{
    bool tmp = mouse_press;
    if(reset)mouse_press=false;
    return tmp;
}

bool Input::mouseReleaseEvent(bool reset)
{
    bool tmp = mouse_rel;
    if(reset)mouse_rel=false;
    return tmp;
}


string Input::joyInfo(int id) const
{
    ostringstream out;
   
    if(!SDL_JoystickOpened(id))
    {
        out << "joy: " << id << " not open";
    }
   
   
    out << "opened joystick: " << SDL_JoystickName(joysticks[id].joy_id) << endl;
    out << "  axes: " <<  SDL_JoystickNumAxes(joysticks[id].joy) << endl;
    out << "  buttons: " <<  SDL_JoystickNumButtons(joysticks[id].joy) << endl;
    out << "  hats: " << SDL_JoystickNumHats(joysticks[id].joy);
   
    return out.str();
}


bool Input::openJoy(int id)
{
    if(SDL_NumJoysticks()<=id)
    {
        cout << "Input:: no joystick found, id: " << id << " -SDL_INIT_JOYSTICK ?" << endl;
        return false;
    }
   
    if(SDL_JoystickOpened(id))
    {
        ERROR("Input:: joystick already open");
        return false;
    }
   
    // Open joystick
    D_ASSERT(!joysticks[id].open());
    SDL_Joystick* joy = SDL_JoystickOpen(id);
   
    if(joy==NULL)
    {
        cout << "error opening joystick " << id <<endl;
        return false;
    }
    joysticks[id].init(id, SDL_JoystickNumAxes(joy), SDL_JoystickNumButtons(joy), joy);
   
   
    //enable SDL joy events
    SDL_JoystickEventState(SDL_ENABLE);
   
    return true;
}

void Input::closeJoy(int id)
{
    // Close if opened
    if(!SDL_JoystickOpened(id))
    {
        cout << "Input:: joystick " << id << " not open" << endl;
        return;
    }
   
    SDL_JoystickClose(joysticks[id].joy);
    joysticks[id].close();
   
}


int Input::joyAxisPos(int axis, bool reset, int id)
{
    D_ASSERT(joysticks[id].open());
   
    int ret=joysticks[id].joy_pos[axis];
    if(reset)joysticks[id].joy_pos[axis]=0;
    return ret;
}

double Input::joyAxisPosNormal(int axis, bool reset, int id)
{
    D_ASSERT(joysticks[id].open());
   
    double ret=joysticks[id].joy_pos[axis]/(double)JOY_MAX;
    if(reset)joysticks[id].joy_pos[axis]=0;
    return ret;
}


int Input::joyAxisMove(int axis, bool reset, int id)
{
    D_ASSERT(joysticks[id].open());
   
    int ret=joysticks[id].joy_move[axis];
    if(reset)joysticks[id].joy_move[axis]=0;
    return ret;
}


bool Input::joyButton(int button, bool reset, int id)
{
    D_ASSERT(joysticks[id].open());
   
    bool tmp = joysticks[id].joy_buttons[button];
    if(reset)joysticks[id].joy_buttons[button]=false;
    return tmp;
}

uint Input::nJoyButton(int id)
{
    if(!joyActive(id))
    {
        ERROR("joystick not active");
        return 0;
    }
   
    return joysticks[id].joy_nbutton;
}

uint Input::nJoyAxis(int id)
{
    if(!joyActive(id))
    {
        ERROR("joystick not active");
        return 0;
    }
    return joysticks[id].joy_naxis;
}


uint nJoyAxis(int id=0);

void Input::Joystick::init(int id, int naxis, int nbutton, SDL_Joystick* sdljoy)
{
    D_ASSERT_M(joy==NULL,"already initialized");
   
    joy=sdljoy;
    joy_id=id;
    joy_naxis=naxis;
    joy_nbutton=nbutton;
    joy_pos = new int[naxis];
    joy_move= new int[naxis];
    joy_buttons = new bool[nbutton];
   
    memset(joy_pos,0,sizeof(int)*naxis);
    memset(joy_move,0,sizeof(int)*naxis);
   
    memset(joy_buttons,false,sizeof(bool)*nbutton);
}

bool Input::joyEvent(int id, bool reset)
{
    bool ret=joysticks[id].joyevent;
    if(reset)joysticks[id].joyevent=false;
    return ret;
}

void Input::Joystick::close()
{
    D_ASSERT(joy!=NULL);
    joy=NULL;

    delete[] joy_pos;
    delete[] joy_move;
    delete[] joy_buttons;
}

}
