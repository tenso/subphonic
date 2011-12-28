#ifndef GUI_SELECT_H
# define GUI_SELECT_H

#include <vector>
#include <sstream>
#include <string>

#include "gui.h"
#include "../screen/screen.h"
#include "bg_make.h"

namespace spl {

using namespace std;

//dont use this, use NumberSelect
class DigitSelect : public Container
{
  public:
    //up_d = uparrow_down etc..
    //fire will be called with 'this' as argument
    DigitSelect(SDL_Surface* up_u, SDL_Surface* up_d,
    SDL_Surface* down_u, SDL_Surface* down_d,
    SDL_Surface* back,
    BitmapFont* fnt, Action* fire=NULL);
   
    ~DigitSelect(); //will delete 'fire'
   
    void setChar(char c);
    char getChar();
   
    int getNum();
      
  private:
    class Up : public Action
    {
      public:
        Up(DigitSelect* src);
        void action(Component* c);
	
        DigitSelect* src;
    };
   
    class Down : public Action
    {
      public:
        Down(DigitSelect* src);
        void action(Component* c);
	
        DigitSelect* src;
    };
   
  private:
    Label* l;
    Action* fire;
    char dig;
};

//dont use this, use NumberSelect
class SignSelect : public Container
{
  public:
    //up_d = uparrow_down etc..
    //fire will be called with 'this' as argument
    SignSelect(SDL_Surface* up_u, SDL_Surface* up_d,
    SDL_Surface* down_u, SDL_Surface* down_d,
    SDL_Surface* back,
    BitmapFont* fnt, Action* fire=NULL);
   
    ~SignSelect(); //will delete 'fire'
   
    void setSign(int s);
    int getSign();
    char getChar();
      
  private:
    class Up : public Action
    {
      public:
        Up(SignSelect* src);
        void action(Component* c);
	
        SignSelect* src;
    };
   
    class Down : public Action
    {
      public:
        Down(SignSelect* src);
        void action(Component* c);
	
        SignSelect* src;
    };
   
  private:
    Label* l;
    Action* fire;
    char dig;
   
    int sign;
};



/*This will update 'data' with what is selected
 * ex: num_whole.num_frac : 1.23
 */

template<class T>
class NumberSelect : public Container
{
  public:
   
    enum SIGN_PLACEMENT {LEFT, OVER, UNDER};
   
    //FIXME: use_sign should come after num_frac...(so many using this...)
    //it is NOT possible to update 'data' from outside, use setValue()
    //when using 'neg' a +/- select will be added infront of digits(only way to have neg numbers)
    //data=NULL makes it use internal data
    NumberSelect(BitmapFont* font, unsigned int num_whole, unsigned int num_frac=0, T* data=NULL, 
    bool use_sign=false, SIGN_PLACEMENT place = LEFT);
   
    ~NumberSelect();
   
    //REMEMBER: action() is run on number press
   
   
    //void setData(T* d);
   
    //data clamped to min/max
    void setValue(T val);
    void setValue(const string& str);
   
    T getValue();
         
    void setMin(T m);
    T getMin();
    void setMax(T m);
    T getMax();
   
    void setRange(T min, T max)
    {
        setMin(min);
        setMax(max);
    }
   
   
    string getStr();
   
   


  private:
      
    class C_Fire : public Action
    {
      public:
	
        C_Fire(NumberSelect* s);
        void action(Component* co);
      
      private:
        NumberSelect* src;
    };
   
    class C_Sign : public Action
    {
      public:
	
        C_Sign(NumberSelect* s);
        void action(Component* co);
      
      private:
        NumberSelect* src;
    };
   
    T clip(T v);
   
    T max;
    T min;
   
    string str;
   
    T* data;
   
    unsigned int n;
    unsigned int nw;
    unsigned int nf;
   
    bool use_sign;
   
    SignSelect* sign_char;
   
    DigitSelect** digits_w;
    DigitSelect** digits_f;
   
    string str_rep;
   
    T internal_data;
};

#include "gui_select_T.h"

//FIXME: this is not so good
class SlotSelect : public Container
{
  public:
    SlotSelect(SDL_Surface* slot, SDL_Surface* slot_sel, uint n, int xindent=0, bool grow_side=true);
    ~SlotSelect();
   
    void setCurrent(uint i);
    int getCurrent();
   
    void resetIndex(uint i);
   
    SDL_Rect getIndexRect(uint i);
   
    //void showIndex(uint i);
      
  private:
    class SlotAction : public Action
    {
      public:
        SlotAction(SlotSelect* parent, uint i)
        {
            index=i;
            this->parent=parent;
        }
	
        void action(Component* c)
        {
            parent->setCurrent(index);
        }
	
        uint index;
        SlotSelect* parent;
    };
   
    //uint marker;
    Button** slots;
    uint current;
    uint num;
};

}

#endif
