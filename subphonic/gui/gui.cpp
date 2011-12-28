#include "gui.h"

#include "../math/mathutil.h"

namespace spl {

using namespace std;

Component::Component()
{
    act=NULL;
    mousemove_act=NULL;
    key_act=NULL;
   
    selected=false;
    pos.x=0;
    pos.y=0;
    pos.w=0;
    pos.h=0;
   
    stay_sel=false;
}

Component::~Component()
{
    delete act;
    act=NULL;
    delete mousemove_act;
    mousemove_act=NULL;
    delete key_act;
    key_act=NULL;
}

void Component::setPos(int x, int y)
{
    setX(x);
    setY(y);
}

void Component::relMove(int x, int y)
{
    setPos(pos.x+x,pos.y+y);
}


void Component::setX(int x)
{
    pos.x=x;
}

void Component::setY(int y)
{
    pos.y=y;
}

void Component::setAction(Action* a)
{
    act=a;
}

void Component::remAction()
{
    delete act;
    act=NULL;
}


void Component::setMouseMoveAction(Action* a)
{
    mousemove_act=a;
}

void Component::remMouseMoveAction()
{
    delete mousemove_act;
    mousemove_act=NULL;
}

void Component::action()
{
    if(act!=NULL)act->action(this);
}

void Component::mouseMoveAction()
{
    if(mousemove_act!=NULL)mousemove_act->action(this);
}

void Component::setKeyAction(Action* a)
{
    key_act=a;
}

void Component::remKeyAction()
{
    delete key_act;
    key_act=NULL;
}

void Component::keyAction()
{
    if(key_act!=NULL)key_act->action(this);
}



Component* Component::checkSelection(int x, int y)
{
    if(x>=pos.x && x<=pos.x+pos.w && y>=pos.y && y<=pos.y+pos.h)
    {
        selected=true;
        return this;
    }
   
    else selected=false;
    return NULL;
}

bool Component::getSelected()
{
    return selected;
}

void Component::setSelected(bool v)
{
    selected=v;
}

bool Component::getStaySelected()
{
    return stay_sel;
}

void Component::setStaySelected(bool v)
{
    stay_sel=v;
}




/************/
/*MoveBorder*/
/************/

MoveBorder::MoveBorder(Component* moveobj)
{
    obj=moveobj;
}

MoveBorder::MoveBorder(Component* moveobj, int w, int h, int speed)
{
    obj=moveobj;
    setSize(w,h);
    this->speed=speed;
    ffirst=false;
}

void MoveBorder::setSize(int w, int h)
{
    pos.w=w;
    pos.h=h;
}


void MoveBorder::mousePos(int x, int y)
{
    if(ffirst)
    {
        //where did click happen, save it
        start.x=x-pos.x;
        start.y=y-pos.y;
        ffirst=false;
    }
    obj->relMove(x-start.x-pos.x, y-start.y-pos.y);
   
   
}

void MoveBorder::onClick()
{
    ffirst=true;
    selected=true;
}
void MoveBorder::onRelease()
{
    selected=false;
}

void MoveBorder::draw(Screen* s) //dont use this as picture
{
}



/***********/
/*CONTAINER*/
/***********/

Container::Container(int x, int y, int w, int h)
{
    pos.x=x;
    pos.y=y;
    this->w=w;
    this->h=h;
    active_comp=NULL;
    resethome=true;
   
    //FIXME: cant do this: all gui stuff does not have a w/h...
    //e.g: Label, all Leds----
    setCompDrawCull(false/*true*/);
}

Container::~Container()
{
    for(comp_list_it it = comps.begin();it!=comps.end();it++)
    {
        delete *it;
    }
    comps.clear();
}

void Container::setCompDrawCull(bool v)
{
    do_cull=v;
}

bool Container::getCompDrawCull()
{
    return do_cull;
}


void Container::relMove(int x, int y)
{
    //bool first=true;
    int xmov=x;
    int ymov=y;
   
    for(comp_list_it it = comps.begin();it!=comps.end();it++)
    {
        Component* c = *it;
	
        c->relMove(xmov,ymov);
    }
    pos.x+=xmov;
    pos.y+=ymov;
}


void Container::setPos(int x, int y)
{
   
    //cant just setPos(x,y) on contained comps because that would destroy relative positions
    //of how the comps where added, so calc movement and use relPos...
    int dx = x-pos.x;
    int dy = y-pos.y;
   
    for(comp_list_it it = comps.begin();it!=comps.end();it++)
    {
        Component* c = *it;
	
        c->relMove(dx,dy);
    }
   
   
    pos.x=x;
    pos.y=y;
   
   
}

void Container::moveToTop(Component* c)
{
    if(c==NULL)
    {
        DERROR("comp NULL");
        return;
    }
   
    comp_list_it t = find(comps.begin(),comps.end(), c);
   
    if(t==comps.end())
    {
        DERROR("(W) comp not found");
        return;
    } 
   
    comps.erase(t);
    comps.push_back(c);  
}

void Container::add(Component* c, int x, int y, bool savehome)
{
    //FIXME: what if x, y neg!
   
    //get width
    if(x+c->pos.w>pos.w)pos.w=x+c->pos.w;
    if(y+c->pos.h>pos.h)pos.h=y+c->pos.h;
   
   
    //update position to be relative to container
    c->setPos(x+pos.x,y+pos.y);
   
    //set components home
    if(resethome && !savehome)c->setHome(this);
   
    comps.push_back(c);
}

void Container::addAbs(Component* c, int x, int y, bool savehome)
{
    //FIXME:
    if(c->pos.w>pos.w)pos.w=c->pos.w;
    if(c->pos.h>pos.h)pos.h=c->pos.h;
   
    c->setPos(x, y);
   
    //set components home
    if(resethome && !savehome)c->setHome(this);
   
    comps.push_back(c);
}


Component* Container::rem(Component* c)
{
    comp_list_it t = find(comps.begin(),comps.end(), c);
    if(t==comps.end())
    {
        DERROR("(W) comp not found");
        return NULL;
    }
   
    Component* ret = *t;
    comps.erase(t);
   
    //WARNING: what to do here...
    if(active_comp!=NULL && active_comp->getHome()==c)
    {
        active_comp=NULL;
    }
    //active_comp=NULL;
   
    return ret;
}

Component* Container::replace(Component* t, Component* w, bool same_pos)
{
    comp_list_it it = find(comps.begin(),comps.end(), t);
   
    if(it==comps.end())
    {
        DERROR("(W) comp not found");
        return NULL;
    }
   
    if(same_pos)
    {
        w->setPos(t->getPos());
    }
   
    if(resethome)
    {
        w->setHome(t->getHome());
    }
   
    int incont_x = w->pos.x-pos.x;
    int incont_y = w->pos.y-pos.y;
   
    if(incont_x + w->pos.w > pos.w)pos.w=incont_x+w->pos.w;
    if(incont_y + w->pos.h > pos.h)pos.h=incont_y+w->pos.h;
   
    comps.insert(it, w); //inserts before 't'
    comps.erase(it);
   
    return t;
}

void Container::recalcSize()
{
    pos.w=0;
    pos.h=0;
   
    for(comp_list_it it = comps.begin();it!=comps.end();it++)
    {
        Component* c = *it;
	
        int dx = c->pos.x-pos.x;
        int dy = c->pos.y-pos.y;
	
        if(dx + c->pos.w > pos.w)pos.w=dx+c->pos.w;
        if(dy + c->pos.h > pos.h)pos.h=dy+c->pos.h;
    }
}


Component* Container::getLast()
{
    if(comps.size()==0)return NULL;
    return comps.back();
}


Component* Container::clickSelected(int x, int y, int button)
{
    Component* c = checkSelection(x,y);
    if(c==NULL)return c;
   
    active_comp=c;
    c->onClick(x, y, button);
    return c;
}

bool Container::haveSelected()
{
    return active_comp!=NULL;
}

bool Container::releaseSelected()
{
    if(active_comp==NULL)return false;
    active_comp->onRelease();
    active_comp->setSelected(false);
    active_comp=NULL;
    return true;
}

Component* Container::getSelectedComp()
{
    return active_comp;
}


bool Container::giveInput(spl::Input& in)
{
    if(active_comp==NULL)
    {
        return false;
    }
   
    return active_comp->giveInput(in);
}

/*
  void Container::giveMouseMotion(int x, int y)
  {
  DASSERT(active_comp!=NULL);
  if(active_comp==NULL)
  {
  DERROR("no active comp to give motion to");
  return;
  }
  * 
  active_comp->mouseMove(x,y);
  }
  * 
  void Container::giveMousePosition(int x, int y)
  {
  DASSERT(active_comp!=NULL);
  if(active_comp==NULL)
  {
  DERROR("no active comp to give pos to");
  return;
  }
  active_comp->mousePos(x,y);
  }*/

Component* Container::checkSelection(int x, int y)
{
    //check selecton backwards(pixmap's are added first but should be checked last)
   
    for(comp_list_rev_it it = comps.rbegin();it!=comps.rend();it++)
    {
        Component* c = *it;
	
        Component* sel = c->checkSelection(x, y);
	
        if(sel!=NULL)
        {
            return sel;
        }
	
    }
    return NULL;
}


void Container::setHome(Component* home)
{
    Component::setHome(home);
   
    for(comp_list_it it = comps.begin();it!=comps.end();it++)
    {
        Component* c = *it;
        c->setHome(home);
    }
}

void Container::draw(Screen* screen)
{
    for(comp_list_it it = comps.begin();it!=comps.end();it++)
    {
        Component* c = *it;
        
        D_ASSERT(c != NULL);
        if(c == NULL)continue;

        //SDL handles this fine(takes away ~500)
        if(do_cull)
        {
            //check bounds
            SDL_Rect p = c->getPos();
            //SDL_Rect o = getPos();
            //p.x += o.x;
            //p.y += o.y;
	     
            if(p.x > screen->W())continue;
            if(p.y > screen->H())continue;
            if(p.x+p.w < 0)continue;
            if(p.y+p.h < 0)continue;
	     
        }
	
        c->draw(screen);
    }
}



/********/
/*PIXMAP*/
/********/

Pixmap::Pixmap(SDL_Surface* pixmap)
{
    back = pixmap;
   
    if(back==NULL)
    {
        pos.w=0;
        pos.h=0;
        return;
    }
   
    pos.w=back->w;
    pos.h=back->h;
}

Pixmap::~Pixmap()
{
}

void Pixmap::change(SDL_Surface* pixmap)
{
    back = pixmap;
   
    if(back==NULL)
    {
        pos.w=0;
        pos.h=0;
        return;
    }
   
    pos.w=back->w;
    pos.h=back->h;
}

void Pixmap::draw(Screen* screen)
{
    if(back!=NULL)screen->blit(back, &pos);
}



/*******/
/*LABEL*/
/*******/

Label::Label(BitmapFont* font, const string& s)
{
    str=NULL;
    this->font = font;
    maxchar=-1;
    printF("%s", s.c_str());
    use_color=false;
}

Label::~Label()
{
    delete[] str;
}

void Label::setMaxChar(int max)
{
    maxchar=max;
}

int Label::getMaxChar() const
{
    return maxchar;
}

void Label::printF(const char* fmt, ...)
{
    //FIXME: this is not good
    delete[] str;
   
    va_list ap;
   
    va_start(ap,fmt);
    int len = vsnprintf(NULL,0,fmt,ap)+1;
    va_end(ap);
   
    str=new char[len];
   
    va_start(ap,fmt);
    int tmp = vsnprintf(str,len,fmt,ap);
    va_end(ap);
   
    str[len-1]='\0';
    strl=len;
    DASSERT(tmp+1==len);
   
}

uint Label::getStrLen() const 
{
    return strlen(str);
}


Component* Label::checkSelection(int x, int y)
{
    return NULL;
}

void Label::draw(Screen* screen)
{
    int dolen=strl;
    if(maxchar>-1)
    {
        if(strl>maxchar)dolen=maxchar;
    }
   
    if(use_color)
    {
        SDL_Color old = font->getColor();
        font->setColor(color);
	
        font->printStr(pos.x, pos.y, str, dolen);
	
        font->setColor(old);
    }
    else	font->printStr(pos.x, pos.y, str, dolen);
}

void Label::remColor()
{
    use_color=false;
}

void Label::setColor(Uint8 r, Uint8 g, Uint8 b)
{
    use_color=true;
    color.r=r;
    color.g=g;
    color.b=b;
}


void Label::setColor(const SDL_Color& c)
{
    use_color=true;
    color=c;
}

SDL_Color Label::getColor()
{
    return color;
}

/********/
/*BUTTON*/
/********/

Button::Button(SDL_Surface* upbitmap, SDL_Surface* downbitmap)
{
    pressed=false;
    ok=false;
   
    //load gfx
    up = upbitmap;
    down = downbitmap;
   
    if(up==NULL || down==NULL)
    {
        cout << "(E)  bitmap NULL" << endl;
        return;
    }
   
    pos.w=up->w;
    pos.h=up->h;
   
    ok=true;
    stay_pressed=false;
}

bool Button::isOk()
{
    return ok;
}

void Button::onClick()
{
    if(stay_pressed)
    {
        pressed=!pressed;
        action();
        return;
    }
   
    pressed=true;
    action();
}

void Button::onRelease()
{
    if(stay_pressed)return;
    pressed=false;
}

void Button::stayPressed(bool val)
{
    stay_pressed=val;
}

void Button::setStayPressed(bool val)
{
    stay_pressed=val;
}

bool Button::getStayPressed()
{
    return stay_pressed;
}


void Button::draw(Screen* screen)
{
    if(pressed)screen->blit(down, &pos);
    else screen->blit(up, &pos);
}

bool Button::getValue()
{
    return pressed;
}

void Button::setValue(bool val)
{
    if(val>1)val=true;
    pressed=val;
}


bool Button::isPressed()
{
    return getValue();
}
void Button::setPressed(bool val)
{
    setValue(val);
}

int Button::getW()
{
    return up->w;
}
int Button::getH()
{
    return up->h;
}


/*********/
/*NButton*/
/*********/

NButton::NButton(uint n, SDL_Surface** pix)
{
    current=0;
   
    this->n=n;
   
    bitmaps = new SDL_Surface*[n];
    for(uint i=0;i<n;i++)bitmaps[i]=pix[i];
   
   
    pos.w=bitmaps[current]->w;
    pos.h=bitmaps[current]->h;
   
    ok=true;
   
}

NButton::~NButton()
{
    delete bitmaps;
}


bool NButton::isOk()
{
    return ok;
}

void NButton::onClick()
{
    current++;
    current%=n;
   
    action();
}

void NButton::onRelease()
{
   
}

void NButton::draw(Screen* screen)
{
    screen->blit(bitmaps[current], &pos);
}

uint NButton::getValue()
{
    return current;
}

void NButton::setValue(uint val)
{
    val%=n;
    current=val;
}

int NButton::getW()
{
    return bitmaps[current]->w;
}

int NButton::getH()
{
    return bitmaps[current]->h;
}

/********/
/*SLIDER*/
/********/

Slider::Slider(SDL_Surface* knob_bitmap, SDL_Surface* under_bitmap, bool vertical)
{
    vert=vertical;
    pressed=false;
    ok=false;
   
    //load gfx
    knob  = knob_bitmap;
    under = under_bitmap;
   
    if(knob==NULL || under==NULL)
    {
        cout << "(E)  bitmap NULL" << endl;
        return;
    }
   
    pos.w=under->w;
    pos.h=under->h;
   
    knobpos.w=knob->w;
    knobpos.h=knob->h;
   
    knoboff.x=0;
    knoboff.y=0;
    knoboff.w=0;
    knoboff.h=0;
   
    val=0.0;
    ok=true;
   
    setLimits(0,1);
}

bool Slider::isOk()
{
    return ok;
}

void Slider::onClick()
{
    pressed=true;
    action();
}

void Slider::onRelease()
{
    pressed=false;
}

void Slider::centerKnob(bool v)
{
    if(vert)
    {
        if(v)
        {
            int diff = knobpos.w-pos.w;
            diff /= 2;
            knoboff.x=-diff;
        }
        else
        {
            knoboff.x=0;
        }
    }
    else
    {
        if(v)
        {
            int diff = knobpos.h-pos.h;
            diff /= 2;
            knoboff.y=-diff;
        }
        else
        {
            knoboff.y=0;
        }
    }
   
}


void Slider::draw(Screen* screen)
{
    SDL_Rect k_pos = knobpos;
    k_pos.x+=knoboff.x;
    k_pos.y+=knoboff.y;
   
    screen->blit(under, &pos);
    screen->blit(knob, &k_pos);
}

bool Slider::isPressed()
{
    return pressed;
}

void Slider::setX(int x)
{
    knobpos.x=x;
    pos.x=x;
    if(!vert)
    {
        max = pos.x+under->w;
        min = pos.x;
    }
   
    setSliderPos(val);
}

void Slider::setY(int y)
{
    knobpos.y=y;
    pos.y=y;
   
    if(vert)
    {
        max = pos.y;
        min = pos.y+under->h;
    }
   
    setSliderPos(val);
}

void Slider::setLimits(coord_t left, coord_t right)
{
    this->left=left;
    this->right=right;
   
    min_v = (left<right) ? left : right;
    max_v = (left>right) ? left : right;
   
    if(val<min_v)val=min_v;
    if(val>max_v)val=max_v;
}

/*
  void Slider::relMove(int x, int y) //needed so this does not reset knobpos
  {
  * 
  pos.x+=x;
  pos.y+=y;
  knobpos.x+=x;
  knobpos.y+=y;
  * 
  if(!vert)
  {
  max = pos.x+under->w;
  min = pos.x;
  }
  else
  {
  max = pos.y;
  min = pos.y+under->h;
  }
  * 
  setSliderPos(val);
  }*/

void Slider::mousePos(int x, int y)
{
    if(vert)
    {
        knobpos.y=y-knob->h/2;
        if(knobpos.y+knobpos.h>min)knobpos.y=min-knobpos.h;
        if(knobpos.y<max)knobpos.y=max;
	
        coord_t d_val = 1.0-(knobpos.y-pos.y)/(coord_t)(under->h-knob->h);
	
        val = left + d_val*(right-left);
    }
    else
    {
        knobpos.x=x-knob->w/2;
        if(knobpos.x+knobpos.w>max)knobpos.x=max-knobpos.w;
        if(knobpos.x<min)knobpos.x=min;
	
        coord_t d_val = (knobpos.x-pos.x)/(coord_t)(under->w-knob->w);
	
        val = left + d_val*(right-left);
    }
    mouseMoveAction();
}

void Slider::setVert(bool v)
{
    vert=v;
}

bool Slider::getVert()
{
    return vert;
}


coord_t Slider::getValue()
{
    return val;
}

void Slider::setValue(coord_t v)
{
    DASSERT(!isnan(v));
   
    val=v;
    if(val<min_v)val=min_v;
    if(val>max_v)val=max_v;
   
    setSliderPos(val);
}

coord_t Slider::getNormValue()
{
    return (val-left)/(right-left);
}

void Slider::setNormValue(coord_t v)
{
    if(v<0.0)v=0.0;
    if(v>1.0)v=1.0;
   
    coord_t actual = left + v*(right-left);
    setValue(actual);
}


//private
void Slider::setSliderPos(coord_t val)
{
    coord_t d_val = (val-left)/(right-left);
   
    if(vert)knobpos.y = (int)((1.0-d_val)*(under->h-knob->h)+pos.y);
    else knobpos.x = (int)(d_val*(under->w-knob->w)+pos.x);
}


/******/
/*KNOB*/
/******/


KnobSlider::KnobSlider(SDL_Surface* knob_bitmap)
{
    cont=false;
    vert=true;
    pressed=false;
    ok=false;
   
    //load gfx
    knob = knob_bitmap;
    if(knob==NULL)
    {
        cout << "(E) bitmap NULL" << endl;
        ok=false;
    }
   
    //SDL_SetColorKey(knob, SDL_SRCCOLORKEY, SDL_MapRGB(knob->format,0xff,0xff,0xff));
   
    pos.w=knob->w;
    pos.h=knob->h;
   
    r=knob->w*0.5f;
    setStartAng(5*M_PI/4.0);
    setStopAng(-M_PI/4.0);
    setTurnSpeed(DEF_KNOB_TURN_SPEED);
    setLimits(0, 1.0);
    val=left;
   
    ok=true;
   
    /*   mark_color.r=0;
         mark_color.g=0xff;
         mark_color.b=0xf1;*/
    mark_color.r=0xff;
    mark_color.g=0xff;
    mark_color.b=0xff;
}

void KnobSlider::setStartAng(coord_t rad)
{
    start_ang=rad;
}

void KnobSlider::setStopAng(coord_t rad)
{
    stop_ang=rad;
}

void KnobSlider::setAngs(coord_t start, coord_t stop)
{
    setStartAng(start);
    setStopAng(stop);
}


void KnobSlider::setLimits(coord_t left, coord_t right)
{
    this->left=left;
    this->right=right;
   
    min_v = spl::minOf(left, right);
    max_v = spl::maxOf(left, right);
   
    inc_rev=right-left;
   
    if(inc_rev!=0)inv_range=1.0/(inc_rev);
    else inv_range=0;
   
    if(val<min_v)val=min_v;
    if(val>max_v)val=max_v;
}

void KnobSlider::setTurnSpeed(coord_t r)
{
    turn=r;
}

void KnobSlider::setVertTurn(bool v)
{
    vert=v;
}


bool KnobSlider::isOk()
{
    return ok;
}

void KnobSlider::onClick()
{
    pressed=true;
    action();
}

void KnobSlider::onRelease()
{
    pressed=false;
}

void KnobSlider::draw(Screen* screen)
{
    Uint32 color = screen->makeColor(mark_color);
    screen->blit(knob, &pos);
   
    coord_t rad;
    if(!cont)
    {
        rad = start_ang+(val-left)*inv_range*(stop_ang-start_ang);
    }
    else rad = start_ang-M_2PI*val*inv_range;
   
    float r0=r*0.3;
   
    screen->line((int)(pos.x+pos.w*0.5+r0*cos(rad)),
    (int)(pos.y+pos.h*0.5-r0*sin(rad)),
    (int)(pos.x+pos.w*0.5+r*cos(rad)),
    (int)(pos.y+pos.h*0.5-r*sin(rad)), color,2);
}

bool KnobSlider::isPressed()
{
    return pressed;
}


void KnobSlider::setContinous(coord_t left, coord_t right, coord_t inc_per_rev)
{
    cont=true;
   
    setLimits(left,right);
   
    inc_rev=inc_per_rev;
    if(inc_rev!=0)inv_range=1.0/(inc_rev);
    else inv_range=0;
}


void KnobSlider::mouseMove(int x, int y)
{
    if(vert)
    {
        val-=y*turn*inc_rev;
    }
    else
    {
        val-=x*turn*inc_rev;
    }
    if(val < min_v)val=min_v;
    if(val > max_v)val=max_v;
   
    mouseMoveAction();
}

coord_t KnobSlider::getValue()
{
    return val;
}

void KnobSlider::setValue(coord_t v)
{
    DASSERT(!isnan(v));
   
    val=v;
   
    if(val<min_v)val=min_v;
    if(val>max_v)val=max_v;
   
}


coord_t KnobSlider::getNormValue()
{
    return (val-left)*inv_range;
}

void KnobSlider::setNormValue(coord_t v)
{
    if(v<0.0)v=0.0;
    if(v>1.0)v=1.0;
   
    coord_t actual = left + v*inc_rev;
    setValue(actual);
}




/******/
/*MENU*/
/******/

Menu::Menu(SDL_Surface* back, SDL_Surface* selback, BitmapFont* f, int maxchar, 
SDL_Surface* subback, SDL_Surface* subback_sel)
{
    this->maxchar=maxchar;
    this->f=f;
   
    //pixmaps
    this->back = new Pixmap(back);
    this->selback = new Pixmap(selback);
   
    if(subback==NULL)this->subback=new Pixmap(back);
    else this->subback=new Pixmap(subback);
   
    if(subback_sel==NULL)this->subback_sel=new Pixmap(selback);
    else this->subback_sel=new Pixmap(subback_sel);
    //end
   
    ch_yoff=this->back->getPos().h - f->charH();
    ch_yoff /= 2;
    if(ch_yoff<0)ch_yoff=0;
   
    label = new Label(f);
    if(maxchar > -1)label->setMaxChar(maxchar);
   
    n=0;
    nsel=-1;
   
    ypos=0;
    xpos=0;
   
    ymax=0;
    xmax=this->selback->getPos().w;
   
    submenu_open=-1;
    submenu_entered=false;
}

Menu::~Menu()
{
   
    for(uint i=0;i<submenus.size();i++)
    {
        delete submenus[i];
    }
    for(uint i=0;i<actions.size();i++)
    {
        delete actions[i];
    }
   
   
    delete back;
    delete selback;
    delete subback;
    delete subback_sel;
   
    delete label;
}

void Menu::addItem(const string& name, Action* a)
{
   
    names.push_back(name);
   
    actions.push_back(a);
   
    n++;
    ymax=n*back->pos.h;
   
    is_submenu.push_back(false);
    submenus.push_back(NULL);
}

void Menu::addSubMenu(const string& name, Menu* menu)
{
   
    names.push_back(name);
   
   
    actions.push_back(NULL);
   
    n++;
    ymax=n*back->pos.h;
   
    is_submenu.push_back(true);
    submenus.push_back(menu);
}


void Menu::draw(Screen* s)
{
    for(int i=0;i<n;i++)
    {
        selback->pos.x = pos.x;
        selback->pos.y = i*back->pos.h + pos.y;
        subback_sel->pos.x = pos.x;
        subback_sel->pos.y = i*back->pos.h + pos.y;
	
        if(selback->pos.y+back->pos.h > s->H()-1)break;
	
        back->pos.x = pos.x;
        back->pos.y = i*back->pos.h+pos.y;
        subback->pos.x = pos.x;
        subback->pos.y = i*back->pos.h+pos.y;
	
        if(nsel>=0 && i==nsel)
        {
            if(is_submenu[i])subback_sel->draw(s);
            else selback->draw(s);
        }
        else 
        {
            if(is_submenu[i])subback->draw(s);
            else back->draw(s);
        }
	
	
        label->setPos(pos.x, pos.y+i*back->pos.h + ch_yoff);
        label->printF("%s",names[i].c_str());
        label->draw(s);
    }
   
    if(nsel>=0 && is_submenu[nsel])
    {
        DASSERT(submenus[nsel]!=NULL);
        if(submenus[nsel]==NULL)return;
	
        submenus[nsel]->draw(s);
    }
   
}

void Menu::onClick()
{
    //FIXME: needed?
    selected=true;
}

void Menu::onRelease()
{
    if(submenu_open!=-1 && submenu_entered)
    {
        DASSERT(submenus[nsel]!=NULL);
        submenus[nsel]->onRelease();
	
        //close submenu for next time
        submenu_open=-1;
        submenu_entered=false;
    }
    else
    {
        if(nsel>=0 && actions[nsel]!=NULL)actions[nsel]->action(this);
    }
   
   
}

void Menu::mousePos(int x, int y)
{
    mouseSet(x,y);
}


bool Menu::mouseSet(int x, int y)
{
    if(submenu_open!=-1)
    {
        DASSERT(submenus[nsel]!=NULL);
	
        if(submenus[nsel]==NULL)
        {
            cout << nsel << endl;
            submenu_open=-1;
            return false;
        }
	
	
        bool ent = submenus[nsel]->mouseSet(x,y);	
        if(ent)
        {
            submenu_entered=true;
        }
        else
        {
            if(submenu_entered)
            {
                submenu_open=-1;
            }
	     
        }
    }
   
    //return if submenu open: it was handled above
    if(submenu_open!=-1 && submenu_entered)return true;
   
    //check what item is selected and/or enter submenu
   
    xpos = x-pos.x;
    ypos = y-pos.y;
   
    if(n==0)return false;
   
    //strict < is necessary: menus must overlap otherwise it might deselct menu when giong to sub
    if(xpos<0)
    {
        nsel=-1;
    }
    else if(xpos>=xmax)
    {
        return true;
        //nsel=-1;
    }
    else if(ypos>=ymax)
    {
        return true;
        //nsel=-1;
    }
    else if(ypos<0)
    {
        return true;
        //nsel=-1;
    }
    else 
    {
        nsel = ypos/back->pos.h;
        DASSERT(nsel>=0 && nsel<n);
	
        if(is_submenu[nsel])
        {
            //this locks nsel to the submenu
            DASSERT(submenus[nsel]!=NULL);
	     
            submenu_entered=false;
	     
            int center_y = submenus[nsel]->back->pos.h*(int)(submenus[nsel]->n/2);
	     
            SDL_Rect subpos;
            subpos.x=pos.x+xmax;
            subpos.y=-center_y+pos.y+nsel*back->pos.h;
	     
            submenus[nsel]->setPos(subpos);
            submenus[nsel]->setActionPos(actionpos);
	     
            //---only open when needed...
            if(submenu_open != nsel)submenus[nsel]->onOpen();
            submenu_open=nsel;
        }
        else submenu_open=-1;
	
        return true;
    }
   
    submenu_open=-1;
   
    return false;
}

/***************/
/*DIRREADERMENU*/
/***************/

DirReaderMenu::DirReaderMenu(SDL_Surface* back, SDL_Surface* selback, BitmapFont* f,
int maxchar, SDL_Surface* subback, SDL_Surface* subback_sel)
  
    : Menu(back, selback, f, maxchar,  subback,  subback_sel)
{
   
    dir_action=NULL;
    action_is_copy=false;
   
    setDisplayPostfix(true);
}

DirReaderMenu::~DirReaderMenu()
{
    //items persist even after release, must clear
    clearItems();
   
    if(!action_is_copy)delete dir_action;
}

void DirReaderMenu::setDir(const string& dir)
{
    this->dir=dir;
   
}

void DirReaderMenu::setDirAction(DirReaderAction* a)
{
    dir_action=a;
}

void DirReaderMenu::copyDirAction(DirReaderAction* a)
{
    dir_action=a;
    action_is_copy=true;
}

void DirReaderMenu::addFileItem(const string& name)
{
    names.push_back(name);
   
    n++;
    ymax=n*back->pos.h;
   
    is_submenu.push_back(false);
    dir_submenus.push_back(NULL);
}

void DirReaderMenu::addSubDirReaderMenu(const string& name, DirReaderMenu* menu)
{
   
    names.push_back(name);
   
   
    n++;
    ymax=n*back->pos.h;
   
    is_submenu.push_back(true);
    dir_submenus.push_back(menu);
}

void DirReaderMenu::clearItems()
{
    for(uint i=0;i<dir_submenus.size();i++)
    {
        delete dir_submenus[i];
    }
   
    names.clear();
    dir_submenus.clear();
    is_submenu.clear();
   
    n=0;
}


void DirReaderMenu::setSkipPrefix(const string& prefix)
{
    skip_prefix=prefix;
}

void DirReaderMenu::setOnlyPostfix(const string& postfix)
{
    this->postfix=postfix;
}


void DirReaderMenu::setDisplayPostfix(bool v)
{
    disp_postfix=v;
}

bool DirReaderMenu::getDisplayPostfix()
{
    return disp_postfix;
}


void DirReaderMenu::onOpen()
{
    clearItems();
   
    if(dir.length()==0)
    {
        //empty
        return;
    }
   
    //read files
    FileDir fd;
    //FIXME: make this optional
    bool ok = fd.read(dir, false, postfix, false);
    if(!ok)
    {
        cout << "read failed:" << dir << endl;
        return;
    }
   
    vector<string> ret;
    fd.getFiles(ret);
   
    vector<FileDir::TYPE> ret_t;
    fd.getTypes(ret_t);
   
   
    for(uint i=0;i<ret.size();i++)
    {
        if(skip_prefix.size()>0)
        {
            size_t pf = ret[i].find_first_of(skip_prefix);
	     
            if(pf==0)
            {
                continue;
            }
        }
	
	
        if(ret_t[i]==FileDir::T_FILE)
        {
            addFileItem(ret[i]);
        }
	
        else //is dir
        {
            DirReaderMenu* sub = new DirReaderMenu(back->getUseSurf(), selback->getUseSurf(), f, 
            maxchar,  subback->getUseSurf(),  subback_sel->getUseSurf());
	     
            //NOTICE: dirnames does not contain a trailing / 
            //FIXME: never?
            string s_dir = dir+ret[i];
            if(s_dir.at(s_dir.size()-1) != '/')
            {
                s_dir+="/";
            }
	     
            sub->setDir(s_dir);
            sub->copyDirAction(dir_action);
            sub->setOnlyPostfix(postfix);
            sub->setSkipPrefix(skip_prefix);
            sub->setDisplayPostfix(getDisplayPostfix());
	     
	     
            addSubDirReaderMenu(ret[i], sub);
        }
	
    }
}

void DirReaderMenu::onRelease()
{
    DASSERT(dir_action!=NULL);
   
    if(submenu_open!=-1 && submenu_entered)
    {
        DASSERT(dir_submenus[nsel]!=NULL);
        dir_submenus[nsel]->onRelease();
	
        //close submenu for next time
        submenu_open=-1;
        submenu_entered=false;
    }
    else
    {
        if(nsel>=0)
        {
            if(is_submenu[nsel])return;
	     
            dir_action->setActionDir(dir);
            dir_action->setActionName(names[nsel]);
            dir_action->action(this);
        }
    }
}

bool DirReaderMenu::mouseSet(int x, int y)
{
    if(submenu_open != -1)
    {
        DASSERT(dir_submenus[nsel]!=NULL);
	
        if(dir_submenus[nsel]==NULL)
        {
            cout << nsel << endl;
            submenu_open=-1;
            return false;
        }
	
	
        bool ent = dir_submenus[nsel]->mouseSet(x,y);	
        if(ent)
        {
            submenu_entered=true;
        }
        else
        {
            if(submenu_entered)
            {
                submenu_open=-1;
            }
	     
        }
    }
   
    //return if submenu open: it was handled above
    if(submenu_open!=-1 && submenu_entered)return true;
   
    //check what item is selected and/or enter submenu
   
    xpos = x-pos.x;
    ypos = y-pos.y;
   
   
    if(n==0)
    {
        return false;
    }
   
   
    if(xpos<0)
    {
        nsel=-1;
    }
    else if(xpos>=xmax)
    {
        return true;
        //nsel=-1;
    }
    else if(ypos>=ymax)
    {
        return true;
        //nsel=-1;
    }
    else if(ypos<0)
    {
        return true;
        //nsel=-1;
    }
    else 
    {
        nsel = ypos/back->pos.h;
        DASSERT(nsel>=0 && nsel<n);
	
        if(is_submenu[nsel])
        {
            //this locks nsel to the submenu
            DASSERT(dir_submenus[nsel]!=NULL);
	     
            int center_y = dir_submenus[nsel]->back->pos.h*(int)(dir_submenus[nsel]->n/2); 
	     
            submenu_entered=false;
	     
            SDL_Rect subpos;
            subpos.x=pos.x+xmax;
            subpos.y=-center_y+pos.y+nsel*back->pos.h;
	     
            dir_submenus[nsel]->setPos(subpos);
            dir_submenus[nsel]->setActionPos(actionpos);
	     
            if(submenu_open != nsel)dir_submenus[nsel]->onOpen();
            submenu_open=nsel;
        }
        else  submenu_open=-1;
        return true;
    }
    submenu_open=-1;
    return false;
}

void DirReaderMenu::draw(Screen* s)
{
    for(int i=0;i<n;i++)
    {
        selback->pos.x = pos.x;
        selback->pos.y = i*back->pos.h + pos.y;
        subback_sel->pos.x = pos.x;
        subback_sel->pos.y = i*back->pos.h + pos.y;
	
        if(selback->pos.y+back->pos.h > s->H()-1)break;
	
        back->pos.x = pos.x;
        back->pos.y = i*back->pos.h+pos.y;
        subback->pos.x = pos.x;
        subback->pos.y = i*back->pos.h+pos.y;
	
        if(nsel>=0 && i==nsel)
        {
            if(is_submenu[i])subback_sel->draw(s);
            else selback->draw(s);
        }
        else 
        {
            if(is_submenu[i])subback->draw(s);
            else back->draw(s);
        }
	
        string l_str = names[i];
        if(!disp_postfix)
        {
            l_str = l_str.substr(0,l_str.find_last_of("."));
        }
        label->setPos(pos.x, pos.y+i*back->pos.h + ch_yoff);
        label->printF("%s",l_str.c_str());
        label->draw(s);
    }
   
    if(nsel>=0 && is_submenu[nsel])
    {
        DASSERT(dir_submenus[nsel]!=NULL);
        if(dir_submenus[nsel]==NULL)return;
	
        dir_submenus[nsel]->draw(s); 
    }
   
}

}