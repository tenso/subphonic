#include "gui_select.h"

namespace spl {

/*************/
/*DigitSelect*/
/*************/

DigitSelect::DigitSelect(SDL_Surface* up_u, SDL_Surface* up_d,
SDL_Surface* down_u, SDL_Surface* down_d,
SDL_Surface* back,
BitmapFont* fnt, Action* fire)
{
    dig='0';
    this->fire=fire;
   
    setPos(0,0); //inititaly
   
    Button* up = new Button(up_u, up_d);
    up->setAction(new Up(this));
   
    Button* down = new Button(down_u, down_d);
    down->setAction(new Down(this));
   
    //background
    Pixmap* back_pix = new Pixmap(back);
    add(back_pix);
   
    l = new Label(fnt);
    add(l,0,9);
    l->printF("%c",dig);
   
   
    add(up,0,0);
    add(down,0,18);
   
}

DigitSelect::~DigitSelect()
{
    delete fire;
}


char DigitSelect::getChar()
{
    return dig;
} 


int DigitSelect::getNum()
{
    return dig-'0';
}

void DigitSelect::setChar(char c)
{
    dig=c;
    if(dig>'9')dig='9';
    if(dig<'0')dig='0';
    l->printF("%c",dig);
}


DigitSelect::Up::Up(DigitSelect* s)
{
    src=s;
}

void DigitSelect::Up::action(Component* c)
{
    src->dig++;
    if(src->dig>'9')src->dig='9';
    if(src->fire!=NULL)src->fire->action(src);
    src->l->printF("%c",src->dig);
}

DigitSelect::Down::Down(DigitSelect* s)
{
    src=s;
}

void DigitSelect::Down::action(Component* c)
{
    src->dig--;
    if(src->dig<'0')src->dig='0';
    if(src->fire!=NULL)src->fire->action(src);
    src->l->printF("%c",src->dig);
}




/************/
/*SignSelect*/
/************/

SignSelect::SignSelect(SDL_Surface* up_u, SDL_Surface* up_d,
SDL_Surface* down_u, SDL_Surface* down_d,
SDL_Surface* back,
BitmapFont* fnt, Action* fire)
{
    dig='+';
    this->fire=fire;
   
    setPos(0,0); //inititaly
   
    Button* up = new Button(up_u, up_d);
    up->setAction(new Up(this));
   
    Button* down = new Button(down_u, down_d);
    down->setAction(new Down(this));
   
    //background
    Pixmap* back_pix = new Pixmap(back);
    add(back_pix);
   
    l = new Label(fnt);
    add(l,0,9);
    l->printF("%c",dig);
   
   
    add(up,0,0);
    add(down,0,18);
   
}

SignSelect::~SignSelect()
{
    delete fire;
}


char SignSelect::getChar()
{
    return dig;
} 


void SignSelect::setSign(int s)
{
    sign = s;
   
    if(s<0)
        dig = '-';
    else 
        dig = '+';
   
    l->printF("%c",dig);
}


int SignSelect::getSign()
{
    return sign;
}

SignSelect::Up::Up(SignSelect* s)
{
    src=s;
}

void SignSelect::Up::action(Component* c)
{
    src->setSign(1);
   
    if(src->fire!=NULL)src->fire->action(src);
}

SignSelect::Down::Down(SignSelect* s)
{
    src=s;
}

void SignSelect::Down::action(Component* c)
{
    src->setSign(-1);

    if(src->fire!=NULL)src->fire->action(src);
}





/************/
/*SlotSelect*/
/************/

SlotSelect::SlotSelect(SDL_Surface* slot, SDL_Surface* slot_sel,
uint n, int xindent, bool grow_side)
{
    num=n;
    current=0;
    slots = new Button*[num];
   
    //add(new Pixmap("files/slot_back"));
   
    for(uint i=0;i<num;i++)
    {
        slots[i] = new Button(slot, slot_sel);
        slots[i]->stayPressed(true);
        slots[i]->setAction(new SlotAction(this, i));
        if(!grow_side)add(slots[i], xindent, 3+i*slots[i]->pos.w);
        else add(slots[i], xindent+i*slots[i]->pos.w, 3);
    }
   
}

SlotSelect::~SlotSelect()
{
    delete[] slots;
}


void SlotSelect::setCurrent(uint i)
{
    DASSERT(i<num);
   
    slots[current]->setPressed(false);
    current=i;
    slots[current]->setPressed(true);
   
    DASSERT(slots[current]->getValue());
}

int SlotSelect::getCurrent()
{
    return current;
}

void SlotSelect::resetIndex(uint i)
{
    DASSERT(i<num);
   
    slots[i]->setPressed(false);
}

SDL_Rect SlotSelect::getIndexRect(uint i)
{
    DASSERT(i<num);
   
    return slots[i]->pos;
}

/*
  void SlotSelect::showIndex(uint i)
  {
  DASSERT(i<num);
   
  slots[marker]->setPressed(false);
  marker=i;
  slots[marker]->setPressed(true);
  }*/
}