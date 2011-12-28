#ifndef GUI_H
# define GUI_H

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <SDL.h>
#include <cmath>

#include "../util/types.h"
#include "../util/debug.h"
#include "../util/file_io.h"
#include "../screen/all.h"

namespace spl {

class Component;
typedef std::list<Component*> comp_list;
typedef std::list<Component*>::iterator comp_list_it;
typedef std::list<Component*>::reverse_iterator comp_list_rev_it;

//FIXME: move to better place
typedef double coord_t;

/*"LOW LEVEL" GUI HELPERS:
  aplication needs to handle most logic
*/

//NOTICE:
//SDL_BlitSurface() does not take const src so surfaces "can not" be explicit const; BUT implicit const for all SDL_Surface's
//in here, it is considered an ERROR if a GUI comp somehow changes a surface!
//same for BitmapFont(this could be fixed)



//FIXME: alot of things lack pos.w, pos.h: //e.g: Label, all Leds----
// but probably only important for Cull in Container  (for now!)
//FIXME: what to do with compID()...

//OBSERVE:
/*A: NOTHING takes over a pointer to a SDL_Surface* caller must delete*/

/*B: setValues() does not call action's() only key-presses etc*/

/*gui::draw(Screen*): pointer is good for one thing: it is easy to have multiple render targets
 * just send in wanted target: e.g a Screen* could be a RenderToSurface : public Screen.
 * Singleton makes this harder but both designs have arguments*/

typedef int GUI_COMPID;

#define UNNAMEDCOMP 0
#define COMP_SLIDER 1
#define COMP_KNOBSLIDER 2
#define GUI_LASTID 2

//using namespace std;

static const coord_t DEF_KNOB_TURN_SPEED=0.02;

class Action //derive this for callback in action() and mouseMoveAction()
{
  public:
    virtual void action(Component* c) = 0;
    virtual ~Action()
    {
    }
};

/*TOP OBJECT in GUI hierarchy*/

class Component
{
  public:
    Component();
    virtual ~Component(); //frees action(s)
   
    virtual void setPos(const SDL_Rect& p)
    {
        setPos(p.x, p.y);
    }
   
    virtual SDL_Rect getPos() const
    {
        return pos;
    }
   
    virtual void setPos(int x, int y);
   
    //moves relative to current pos, i.e pos.x+x
    virtual void relMove(int x, int y);
   
    virtual GUI_COMPID compID() //poor man's RTTI
    {
        return UNNAMEDCOMP;
    }
     
   
    /*ACTIONS
     * there is actually no preset behaviour for action() or mouseMoveAction() etc
     * the derived classes: Slider, Button etc decides what to do with them
     * so Button "could" call mouseMoveAction() in onClick() for example, but that would NOT
     * be smart so the praxis are:
     * 
     * action() performed in onClick() i.e comp clicked
     * mouseMoveAction() performed in mouseMove() and/or mousePos()
     * 
     * (WARN) Actions sent in are assumed to be taken over by Component so it is deleted
     * this means that remAction must be used not setAction(NULL)
     * */
   
    //action performed by action()
    void setAction(Action* a);
    void remAction();
   
    //calls setAction's Action::action(this)
    void action();
   
    //action performed by mouseMoveAction()
    void setMouseMoveAction(Action* a);
    void remMouseMoveAction();
   
    //calls mouse_action with 'this' as argument
    void mouseMoveAction();
   
    //action performed by keyAction()
    //deriver decides if called after each keypress or after all keys done
    //deriver should also provide a function to get the key(s) pressed if needed
    void setKeyAction(Action* a);
    void remKeyAction();
   
    //calls key_action with 'this' as argument
    void keyAction();
   
    //FIXME: add joyAction
   
    //checks if arg(x,y) is over this component and if it is returns <this> otherwise NULL;
    //also sets selected accordingly
    virtual Component* checkSelection(int x, int y);
    bool getSelected(); //if checkSelection() over this
    void setSelected(bool v);
   
    //default: false
    //only a flag aplication can use anyway it wants
    //BUT if true: the comp should be selected until another or no comp is selected
    bool getStaySelected();
    void setStaySelected(bool v);
   
    /*BEHAVIOUR
     * for derived comps. see ACTIONS what callbacks should be called when
     * */
   
    //what should comp do on incoming mouse move/pos
    //mousePos(x,y) is in screen coordinates
   
    //it is enough to derive these if giveInput() is left alone
   
    //FIXME: should these be move to protected?
    virtual void mouseMove(int x, int y){}
    virtual void mousePos(int x, int y) {}
   
    //if special input is wanted i.e keyboard/joystick etc input
    //this must be overwritten and MOUSE handled in the derived(or Component::giveInput() called)
    //(this should return true if some action were taken) <- hm
    //remember that MouseMove must not be reset if other want it
    virtual bool giveInput(Input& in)
    {
        mousePos(in.xMouse(), in.yMouse());
        mouseMove(in.xMouseMove(false), in.yMouseMove(false));
	
        return false;
    }
   
			
    //FIXME: this needs a button id
   
    //what should comp do on click/realease
    //some need coords(x,y) some dont; thouse who don't should just overload onClick() and ignore
    //onClick(int, int)
    //called by Container::clickSelected()
    //NOTICE: can't have def values, diff num args
    virtual void onClick(){}
    virtual void onClick(int x, int y)
    {
        onClick();
    }
    virtual void onClick(int x, int y, int button) //can't have def values
    {
        onClick(x,y);
    }
    virtual void onRelease(){}

    //draw comp
    virtual void draw(Screen* screen)
    {
    }
   
   
    //'home' comp that embeds all other, useful in gui when using
    // Container::checkSelection and getting embeded comp and 'home' is still needed for some reason
    // ex: Continer::add( ...) sets home to container
    virtual void setHome(Component* home)
    {
        this->home=home;
    }
   
    Component* getHome()
    {
        return home;
    }
   
    //FIXME: move this
    SDL_Rect pos;
   
  protected:
   
    //used by setPos() as standard, so one or both of these might be
    //enough to overload
    virtual void setX(int x);
    virtual void setY(int y);
   
    bool selected;
    bool stay_sel;
   
    Action* act;
    Action* mousemove_act;
    Action* key_act;
    Component* home;

};

/*Add this to a container to enable mouse movement of all comps in 'moveobj'
 * so probably: in caller component: add(new MoveBorder(this, ...*/

class MoveBorder : public Component
{
  public:
    MoveBorder(Component* moveobj);
    MoveBorder(Component* moveobj, int w, int h, int speed=2);
   
    void setSize(int w, int h);
    void mousePos(int x, int y);
   
    void onClick();
    void onRelease();
    void draw(Screen* s); //dont use this as picture
   
  private:
    SDL_Rect start;
    bool ffirst;
    int speed;
    Component* obj;
};

/*MOST important component(after Component), PROBABLY caller's basic building block for user components
 *this has an selected comp that can resive input etc...
 * 
 *CHECK: the way things are made(m_main etc) a Container is kept from ever getting giveInput directly,
 * only added Components will end up recieving input...CHECK THIS
 * 
 */

class Container : public Component
{
  public:
   
    Container(int x=0, int y=0, int w=0, int h=0);
   
    virtual ~Container();
   
    Component* getLast();
   
    //FIXME: size is defunct: when adding neg offsets; fix this
   
    //x,y relative to container so x=0,y=0 is upper left corner of container
    //this function will grow the size of the container to fit all added
    //savehome presidends resetHome(), home set to this container
    void add(Component* c, int x=0, int y=0, bool savehome=false);
   
    //same as add but adds in absolute coordinates
    void addAbs(Component* c, int x=0, int y=0, bool savehome=false);
   
   
    //FIXME: WARNING: size not updated...
    Component* rem(Component* c); //returns comp removed, caller must free it
   
    //replaces 't' with 'w' (same order in container) returns 't'
    //remember 't' wont be freed anymore(not in container)
    //same_pos as 't' e.g x,y
    //WARNING: container only grows, i.e never gets any smaller from a replace
    //WARNING: 'w' inherits 't' home iff resetHome(true)
    Component* replace(Component* t, Component* w, bool same_pos=false);
   
    //if something is 'rem' or 'replace' or changes size this must be run
    //FIXME: this cancels any initial w,h container had
    void recalcSize();
   
    //recursive down to all comps in this
    void setHome(Component* home);
   
    //if a component is found click it, and return it
    //also set getSelectedComp() etc to it(i.e checkSelection() -> onClick()) 
    Component* clickSelected(int x, int y, int button=0); //BUTTONS: 0=left, 1=middle, 2=right
   
    //returns NULL if no selected
    Component* getSelectedComp();
    bool haveSelected();
    bool releaseSelected();
   
    //WARNING: only gives to the selected component
    virtual bool giveInput(spl::Input& in);
   
    //FIXME: remove
    /*void giveMouseMotion(int x, int y);
      void giveMousePosition(int x, int y);*/
   
    //checks x,y coord for and returns first comp found at that pos
    //needs to be overridden from basic Component class
    virtual Component* checkSelection(int x, int y);
   
    virtual void draw(Screen* screen);
   
    comp_list* getList()
    {
        return &comps;
    }
   
    //automatically sets added Comp's home to this container
    //unless 'savehome'=true
    //def: true
    void resetHome(bool v)
    {
        resethome=v;
    }
   
    void relMove(int x, int y);
    void setPos(int x, int y);
   
    void moveToTop(Component* c);
   
    //default: true
    void setCompDrawCull(bool v);
    bool getCompDrawCull();
   
  private:
    bool do_cull;
    comp_list comps;
    bool resethome;
    Component* active_comp;
    int w,h;
};



class Pixmap : public Component
{
  public:
   
    Pixmap(SDL_Surface* pixmap=NULL);
    ~Pixmap();
   
    void change(SDL_Surface* pixmap);
  
    void draw(Screen* screen);
   
    SDL_Surface* getUseSurf()
    {
        return back;
    }
      
  private:
    SDL_Surface* back;
};



class Label : public Component
{
  public:
    Label(BitmapFont* font, const string& s=string(""));
    ~Label();

    void printF(const char* fmt, ...);
         
    //-1 is unlimited
    void setMaxChar(int max);
    int getMaxChar() const;
   
    Component* checkSelection(int x, int y);
    void draw(Screen* screen);
   
    uint getStrLen() const;
   
    //TO SLOW TO USE...
    void remColor();
    void setColor(const SDL_Color& c);
    void setColor(Uint8 r, Uint8 g, Uint8 b);
    SDL_Color getColor();
   
  private:
   
    bool use_color;
    SDL_Color color;
   
    SDL_Rect bpos;
    BitmapFont* font;
   
    char* str;
    int strl;
   
    int maxchar;
};



class Button : public Component
{
  public:
    Button(SDL_Surface* upbitmap, SDL_Surface* downbitmap);
    ~Button()
    {
    }
   
    bool isOk();
    void onClick();
    void onRelease();
   
    //FIXME: remove
    //default:false
    void stayPressed(bool val);
   
    //default:false
    void setStayPressed(bool val);
    bool getStayPressed();
   
    void draw(Screen* screen);
   
    bool getValue();
   
    //does not run action()
    void setValue(bool val);
   
    //same two functions again
    bool isPressed();
    void setPressed(bool val);
      
    int getW();
    int getH();
   
  private:
   
    SDL_Surface* up;
    SDL_Surface* down;
    bool pressed;
    bool ok;
    bool stay_pressed;
};

/*have n-states not just up/down*/
class NButton : public Component
{
  public:
    //will copy bitmaps so caller must free
    NButton(uint n, SDL_Surface** pix);
    ~NButton();
   
    bool isOk();
    void onClick();
    void onRelease();

    void draw(Screen* screen);
   
    //return 0 for first 1 for second up to n-1 i.e [0,1,...,n-1]
    uint getValue();
    void setValue(uint val);
   
    int getW();
    int getH();
   
  private:
   
    SDL_Surface** bitmaps;
   
    uint n;
    uint current;
    bool ok;
};


//uses mouseMoveAction
class Slider : public Component
{
  public:
    Slider(SDL_Surface* knob_bitmap, SDL_Surface* under_bitmap, bool vertical=false);
    ~Slider()
    {
    }
   
    //centers knob bitmap over under bitmap, otherwise it is at relative 0,0
    void centerKnob(bool v);
   
    GUI_COMPID compID()
    {
        return COMP_SLIDER;
    }
   
    bool isOk();
   
    //slider needs to be able to signal if it is pressed to user
    void onClick();
    void onRelease();
    bool isPressed();
   
    void draw(Screen* screen);
   
    void setX(int x);
    void setY(int y);
    //void mouseMove(int x, int y);
    void mousePos(int x, int y);
   
    coord_t getValue();
    void setValue(coord_t v);
   
    //always in [0 1]
    coord_t getNormValue();
    void setNormValue(coord_t v);
   
    //void relMove(int x, int y);
   
    //default:0,1
    //linear interpolation between
    //left is also down when used as horiz
    void setLimits(coord_t left, coord_t right);
   
    void setVert(bool v);
    bool getVert();
   
  private:
   
    void setSliderPos(coord_t val);

    bool vert;
    coord_t val;
    int max;
    int min;
   
    SDL_Rect knobpos;
    SDL_Rect knoboff;
   
    SDL_Surface* knob;
    SDL_Surface* under;
    bool pressed;
    bool ok;
   
    coord_t left;
    coord_t right;
    coord_t min_v;
    coord_t max_v;
};


//uses mouseMoveAction
class KnobSlider : public Component
{
  public:
    KnobSlider(SDL_Surface* knob_bitmap);
    ~KnobSlider()
    {
    }
   
    GUI_COMPID compID()
    {
        return COMP_KNOBSLIDER;
    }
   
    bool isOk();
    void onClick();
    void onRelease();
    void draw(Screen* screen);
    bool isPressed();
    void mouseMove(int x, int y);
   
    coord_t getValue();
    void setValue(coord_t v);
   
    //always in [0 1]
    coord_t getNormValue();
    void setNormValue(coord_t v);
   
    //it is possible to make knob "be" in lower half plane by neg angles [-pi,0] is lower
    //while [pi,0] is upper
    void setAngs(coord_t start, coord_t stop);
    void setStartAng(coord_t rad);
    void setStopAng(coord_t rad);
   
    //default=0,1
    void setLimits(coord_t left, coord_t right);
   
    void setTurnSpeed(coord_t r);
   
    //other wise turn knob on horizontal motion
    void setVertTurn(bool v);
   
    //spin knob round many times between min max, making 'increase' in a revolution
    //starts at startAng which also becomes end
    //FIXME: this is oneway for now, i.e no turning back to def mode
    void setContinous(coord_t left, coord_t right, coord_t increase);
   
  private:
    SDL_Color mark_color;
    SDL_Surface* knob;
    coord_t r;
    coord_t turn;
    coord_t start_ang;
    coord_t stop_ang;
   
    bool vert;
    coord_t val;
   
    coord_t min_v;
    coord_t max_v;
   
    coord_t right;
    coord_t left;
   
    bool pressed;
    bool ok;
   
    bool cont;
    coord_t inc_rev;
    coord_t inv_range;
};


/******/
/*MENU*/
/******/

//FIXME: add to self as submenu => segfault

class Menu : public Component
{
  public:
    //use maxchar < 0 for unlimited
    //subback==NULL gives subback=back
    Menu(SDL_Surface* back, SDL_Surface* selback, BitmapFont* f, int maxchar=-1, 
    SDL_Surface* subback=NULL, SDL_Surface* subback_sel=NULL);
   
    virtual ~Menu();
   
    //FIXME: remItem() ...
   
    //action 'a' run on mouse release over 'name'
    virtual void addItem(const string& name, Action* a);
    virtual void addSubMenu(const string& name, Menu* menu);
   
    //set this before entering mousePos(), onClick() etc...
    //so that menu knows where it was entered
    void setActionPos(SDL_Rect r)
    {
        actionpos=r;
    }
    SDL_Rect getActionpos()
    {
        return actionpos;
    }
   
   
    //std interface
    virtual void draw(Screen* s);
    virtual void onClick(); //unused
    virtual void onRelease();
    virtual void mousePos(int x, int y);
   
   
   
    //should be run everytime menu is opened
    virtual void onOpen()
    {
    }
    //--||-- closed
    virtual void onClose()
    {
    }
   
  protected:
   
    virtual bool mouseSet(int x, int y);
   
    Pixmap* back;
    Pixmap* selback;
    Pixmap* subback;
    Pixmap* subback_sel;
   
    int n;
    int nsel;
   
    Button* button;
    Label* label;
   
    int maxchar;
   
    vector<Action*> actions;
    vector<string> names;
   
     
    int selected;
    BitmapFont* f;
   
    int xmax;
    int ymax;

    int ypos;
    int xpos;
    SDL_Rect actionpos;
   
    vector<bool> is_submenu;
   
    vector<string> subnames;
    vector<Menu*> submenus;
   
    int submenu_open;
    bool submenu_entered;
   
    int ch_yoff;
};

/*USED by DirReaderMenu, shared action i.e same for all entries*/
class DirReaderAction : public Action
{
  public:
   
    //when this is called action_name is set
    virtual void action(Component* c) = 0;
    virtual ~DirReaderAction()
    {
    }
   
    void setActionName(const string& s)
    {
        action_name=s;
    }
    void setActionDir(const string& s)
    {
        action_dir=s;
    }
   
    string getActionName()
    {
        return action_name;
    }
    string getActionDir()
    {
        return action_dir;
    }
   
  private:
    string action_name;
    string action_dir;
};

//FIXME:REDO, this was not so  great: havily uses data and func from parent(clash with delete etc...
//also to specific for kw...
class DirReaderMenu : public Menu
{
  public:
    //use maxchar < 0 for unlimited
    //subback==NULL gives subback=back
    DirReaderMenu(SDL_Surface* back, SDL_Surface* selback, BitmapFont* f, int maxchar=-1, 
    SDL_Surface* subback=NULL, SDL_Surface* subback_sel=NULL);
   
    virtual ~DirReaderMenu();
   
    /*FOR this only*/
   
    //MUST BE RUN before usage
    //action 'a' run on mouse release over 'name'
    void setDir(const string& dir );
   
    //takes over
    void setDirAction(DirReaderAction* a);
   
    void setOnlyPostfix(const string& postfix);
   
    //skips file that begin with prefix; empty=off
    //def:off
    void setSkipPrefix(const string& prefix);
   
    //def:true
    void setDisplayPostfix(bool v);
    bool getDisplayPostfix();
   
    //subdirs are created on the fly for recursive directories(WARNING: ln loops)
   
    /*UNSUED*/
    virtual void addItem(const string& name, Action* a)
    {
        ERROR("unused");
    }
    virtual void addSubMenu(const string& name, Menu* menu)
    {
        ERROR("unused");
    }
         
   
    virtual void onOpen();
   
    virtual void onClose()
    {
    }
   
   
    virtual void onRelease();
      
    virtual void draw(Screen* s);
   
  protected:
   
    //for submenus
    void copyDirAction(DirReaderAction* a);
    bool action_is_copy;
   
    void clearItems();
    void addFileItem(const string& name);
    void addSubDirReaderMenu(const string& name, DirReaderMenu* menu);
   
    virtual bool mouseSet(int x, int y);
      
    string dir; 
    DirReaderAction* dir_action;
   
    //these are updated once per initial open
    vector<DirReaderMenu*> dir_submenus;
   
    string skip_prefix;
    string postfix;
   
    bool disp_postfix;
};

}

#endif
