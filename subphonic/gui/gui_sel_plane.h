#ifndef GUI_SELPLANE_H
# define GUI_SELPLANE_H

#include <vector>
#include <sstream>
#include <string>

#include "gui.h"
#include "../screen/screen.h"

namespace spl {

class SelectPlane : public Component
{
  public:
   
    class Coord
    {
      public:
        Coord(coord_t x=0,coord_t y=0)
        {
            this->x=x;
            this->y=y;
        }
	
        coord_t x;
        coord_t y;
    };
   
   
    class ValueMarker
    {
      public:
        enum MODE {LINE, POINT};
	
        coord_t* data;
        SDL_Color color;
        int w;
	
        MODE mode;
    };
   
    //default returns values between -1,1 for both x and y
    //i.e maps screen_x:0 to -1 screen_x:w to 1
    //         screen_y:0 to 1 screen_y:h to -1
    //use setValueRange()
    SelectPlane(int w, int h);
    ~SelectPlane();
   
    //get all values
    void getValueVec(vector<Coord>& ret);
   
    //resets
    void setWaypoints(const vector<Coord>& d);
      
    //shows a vertical line at the x-coord corresponding to *val 
    //must be in [xmin, xmax]
    void addValueMarker(const ValueMarker& vm);
   
    //xmin left xmax right ymin bottom ymax top
    void setValueRange(coord_t xmin, coord_t xmax, coord_t ymin, coord_t ymax);
   
    //delay between select marker and possible move, 100ms default...
    void setClickMoveDelay(Uint32 d);
    Uint32 getClickMoveDelay();
   
    void setMarkColor(const SDL_Color& c);
    void setSelectedColor(const SDL_Color& c);
   
    //without, a square is used
    void setMarkerPixmap(SDL_Surface* m);
    void setSelectedMarkerPixmap(SDL_Surface* m);
   
    //used for drawing connections between waypoints
    void setLineParam(const SDL_Color& color, int width=2);
    void showLine(bool v);
   
    /*This is one way, after this it's always in this mode.
     * Calling this will reset anything added
     *Adds 2xendpoints that must always be present and can only be moved in y-dir
     *curve will be one-one(a function) and connected by increasing x, i.e whenever a point p is added
     * between two others: ... p'p p''... it will alywas come after p' and before p'' when getValue() is called
     * */

    void setFunctionMode();
   
    //returns the currently selected waypoint's index
    bool hasSelected();
    int getSelected();
   
    //returns last WP selected
    bool hasLastSelected();
    int getLastSelected();
   
    int numWaypoints();
   
    //returns the value of waypoint 'n'
    Coord getValue(unsigned int n);
   
    //return false if coord does not exist
    bool setValue(unsigned int n, const Coord& c);
   
    bool addWaypoint(const Coord& c);
    void remWaypoint(unsigned int n);
   
    //FIXMENOW: TEST THIS
    //default: 1
    //-1 for unlimited
    //shrinking: last points discarded(either last added or last in vector(oneone))
    int maxWaypoints();
    void maxWaypoints(int n);
      
    //setMouseMoveAction() used
    virtual void mouseMove(int x, int y);
    virtual void mousePos(int x, int y);
   
   
    //setAction() used
    virtual void onClick(int x, int y);
    virtual void onRelease();
   
    virtual void draw(Screen* screen);

  private:
   
    SDL_Rect clipToDim(const SDL_Rect& s);
    Coord screenToVal(const SDL_Rect& s);
   
    Coord clipVal(const Coord& s);
    SDL_Rect valToScreen(const Coord& c);
   
    class WPoint
    {
      public:
	
        WPoint(const SDL_Rect& s, const Coord& v);
        WPoint(int screen_x=0, int screen_y=0, int val_x=0, int val_y=0);
	
        //compare on s.x
        friend bool operator<(const WPoint& wp1, const WPoint& wp2)
        {
            if(wp1.s.x<wp2.s.x)return true;
            return false;
        }
	
	
        SDL_Rect s;
        Coord v;
    };
   
    class FindCloseby : public unary_function<const WPoint&, bool>
    {
      public:
        //if coord is closer than +- span_X return true on compare
        FindCloseby(const WPoint& rf, int span_x, int span_y);
        result_type operator()(argument_type r);
      private:
        const WPoint& ref;
        int span_x;
        int span_y;
    };
     
    int search_span_x, search_span_y;
    int w,h;
      
    SDL_Color vm_color;
    int vm_w;
   
    SDL_Color line_color;
    SDL_Color sel_color;
    SDL_Color mark_color;
   
    coord_t move_speed;
   
    int line_w;
    bool show_line;
   
    bool have_clc;
    Uint32 c_line_color;
   
    SDL_Surface* marker;
    SDL_Surface* sel_marker;
   
    int selected;
    int last_sel;
    int max;
   
    Uint32 delay_click_move;
    Uint32 just_click_time;
    bool do_move;
   
    //if in OneOne, wayp[0] is start and wayp[1] is end
    std::vector<WPoint> wayp;
   
    bool oneone;
   
    coord_t xmin;
    coord_t xmax;
    coord_t ymin;
    coord_t ymax;
   
    std::vector<ValueMarker> vmarks;
};
}
#endif
