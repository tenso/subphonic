#ifndef M_CON_H
#define M_CON_H

#include "../g_comp/g_common.h"
#include "progstate.h"

//FIXME?: this class now has two responsibilities;
//take care of establishing ongoing in/out connection AND
//save/draw all visual 'lines' between comps


typedef list<LineData*> con_list;
typedef list<LineData*>::iterator con_list_it;
typedef list<LineData*>::reverse_iterator con_list_rev_it;

class Connection
{
  public:
    Connection();
   
    ~Connection();
     
    void reset();
   
    //frees data
    bool removeLine(LineData* line);
   
    void drawPatchCord(int x0, int y0, int x1, int y1, double tension, Screen* s, 
    const SDL_Color& s_color, const SDL_Color& e_color, unsigned int line_w=2);
   
    void draw(Screen* s, bool fancy, unsigned int line_w=2);
   
    //handles everything for connections use start for out and finish on in
    //(a connection must keep the giver/taker pair so that when removeLine is run
    //it can flag the soundcomps)
   
    //use these when get7set happens at different times(e.g doing con by mouse)
   
    //removes any connection already comming from 'o'
    void startCon(OutputGiver* o);
   
    //removes any connection already going to 'i'
    void finishCon(InputTaker* i);
   
    //use this when get/set at same time
    void makeCon(OutputGiver* o, InputTaker* i)
    {
        startCon(o);
        finishCon(i);
    }
   
    void defaultAddColor(); //takes color from ProgramState
    void addColor(const SDL_Color& c);
   
    bool doingCon()
    {
        return give;
    }
   
    con_list* getList()
    {
        return &lines;
    }
   
   
    //private:
    SDL_Rect outpos;
    SDL_Rect inpos;
   
  private:
   
    LineData* saveLine();
   
    bool default_color;
    SDL_Color color;
   
    con_list lines;
    bool give;
    OutputGiver* out;
    InputTaker* in;
};

#endif
