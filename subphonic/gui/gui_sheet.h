#ifndef GUI_SHEET_H
# define GUI_SHEET_H

#include "gui.h"

#include <vector>

namespace spl {

using namespace std;

//this class used to display a spreadsheet like structure

class SheetView : public Component
{
  public:
   
    typedef uint id;
   
    SheetView(BitmapFont* font, uint nrows=10);
    ~SheetView();
   
    void clearCols();
   
    void draw(Screen* screen);

    //FIXME: no remCol provided
    id addCol(uint nchar_v);
    uint getNCols();
   
    //default: bg=0,0,0 lines = 255,255,255 selected=0,128,0 delim=128,128,128
    void setBGColor(const SDL_Color& bg);
    void setLineColor(const SDL_Color& lines);
    void setSelColor(const SDL_Color& selected);
    void setDelimColor(const SDL_Color& delim);
   
    //will draw every cell with bg color=delim every m'th row, starting with 0, 4, 8 etc
    //default:4, neg==off
    void setDelimMod(int m);
    int getDelimMod() const;
   
    //default: false,false
    void setShowGrid(bool vert, bool horiz);
    //bool getShowGrid() const;
   
    //default:true
    void setOutline(bool v);
    bool getOutline() const;
   
    //num = -1 to show all remaining after start
    //default: show all
    //returns true if actually set
    //use 0,0 to draw nothing
    bool setView(uint start, int num);
    void getView(uint* start, int* num) const;
   
    //set the number of rows current columns have; also all added will have this num rows
    //all columns must have same num rows
    //WARNING: view is thrashed after this, reset it!
    //default: 10
    void setNRows(uint n);
    uint getNRows() const;
   
    SDL_Rect getCellPos(uint r, uint c) const;
   
    uint getCellW(uint col) const;
    uint getCellH() const;
   
    uint getCharW() const;
    uint getCharH() const;
   
    //returns false if 'r','c' out of bounds
    //truncated set
    bool setEntry(uint r, uint c, const string& s);
    bool getEntry(uint r, uint c, string& s) const;
   
    //moves all columns, up/down unfortunately this class cannot know what
    //a empty entry looks like so it just clears;
    //use setEntry for all cleared rows(to "user empty")
    //returns num moves
    int moveEntries(uint from_row, int dir);
   
    //default: 2,2
    void setSpacing(uint x, uint y);
    uint getSpacingX() const;
    uint getSpacingY() const;
   
    class Column
    {
      public:
        Column(uint nchar_v, uint rows);
	
        //if expanding new elms will be empty
        //if shrinking last elms will be discarded
        void setNRows(uint rows);
        uint getNRows() const;
	
        //trying to set a string longer than nchar_v will truncate
        void setNChar(uint nchar_v);
        uint getNChar() const;
	
        //will reset ret
        bool getEntry(uint n, string& ret) const;
	
        //returns false if out of bounds
        bool setEntry(uint n, const string& s);
	
        //moves all entries staring with 'from', 'dir' steps(i.e dir=-1 moves all up)
        //overwrites/discards when pushing over
        //return: number ov moved(neg is up pos is down)
        int moveEntries(uint from, int dir);
	
      private:
	
        //returns true if move done
        bool moveEntriesUp(uint from);
        bool moveEntriesDown(uint from);
	
        typedef vector<string>::iterator v_it;
	
        uint w; //in chars
        vector<string> entries;
    };
   
   
  private:
   
    bool outline;
    bool showgrid_v;
    bool showgrid_h;
   
    BitmapFont* font;
   
    //FIXME: do these
    SheetView& operator=(const SheetView&);
    SheetView(const SheetView&);
  
    typedef vector<Column>::const_iterator lc_it;
    typedef vector<Column>::iterator l_it;
    vector<Column> cols;
   
    uint nrows;
    uint urows;
    uint start;
    uint num;
      
    uint xinc;
    uint yinc;
   
    int delim_mod;
    SDL_Color c_bg;
    SDL_Color c_lines;
    SDL_Color c_sel;
    SDL_Color c_delim;
};

//sheets added loose their positions and are layed out in the order they are added
//only used to lay sheets from left to right, and view subset(discrete)
class SheetViewLayout : public Component
{
  public:
    typedef uint id;
   
    SheetViewLayout();
   
    //default:0
    void setSpacing(uint xspace);
   
    void clearSheets();
   
    id addSheet(SheetView* sheet);
    SheetView* remSheet(id i);
   
    SheetView* getSheet(id i);
    uint numSheets() const;
   
    //basically same as SheetView
    bool setView(uint start, int num);
    void getView(uint* start, int* num) const;
   
    void draw(Screen* s);
    virtual void setPos(int x, int y);
   
    //this must be called if sheets added ever change size indirectly i.e by pointer
    void recalcSize();
   
  private:
    void recalcPos();
   
    vector<SheetView*> sheets;
   
    uint usheets;
    uint xspace;
   
    uint start;
    uint num;
};

}

#endif
