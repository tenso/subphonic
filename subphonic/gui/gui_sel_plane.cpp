#include "gui_sel_plane.h" 

namespace spl {

SelectPlane::SelectPlane(int w, int h)
{
    this->w=w;
    this->h=h;
   
    pos.w=w;
    pos.h=h;
   
    marker=NULL;
    sel_marker=NULL;
   
    line_w=2;
    line_color.r=4;
    line_color.g=238;
    line_color.b=253;
    have_clc=false;
    show_line=false;
   
    move_speed=2;
   
    search_span_x=3;
    search_span_y=3;
   
    sel_color.r=240;
    sel_color.g=0;
    sel_color.b=0;
   
    mark_color.r=4;
    mark_color.g=238;
    mark_color.b=253;
   
    last_sel=-1;
    selected=-1;
   
    max=1;
   
    delay_click_move=50;
    oneone=false;
   
    xmin=-1;
    xmax=1;
    ymin=-1;
    ymax=1;
   
    vm_color=mark_color;
    vm_color.r=255;
    vm_w=2;
}

void SelectPlane::setWaypoints(const vector<SelectPlane::Coord>& d)
{
    wayp.clear();
   
    uint num = d.size();
    if(max>=0)
    {
        uint umax = (unsigned)max; //ok: >=0
	
        DASSERT(d.size()<umax);
	
        num = ( umax < d.size() ) ? umax : d.size();
    }
   
   
    //make WP's
    for(uint i=0;i < num;i++)
    {
        WPoint wp;
        wp.v=clipVal(d[i]);
        wp.s=valToScreen(wp.v);
	
        wayp.push_back(wp);
    }
   
    if(oneone) //make sure its sorted
    {
        stable_sort(wayp.begin(),wayp.end());
        if(wayp.size()<2)
        {
            DERROR("function mode need atleast start/end wp, making default values...");
            setFunctionMode();
        }
        if(wayp[0].s.x!=0)
        {
            DERROR("function mode need first point to be at s.x=0, making default values...");
            setFunctionMode();
        }
        if(wayp.back().s.x!=w)
        {
            DERROR("function mode needs last point to be at s.x=w, making default values...");
            setFunctionMode();
        }
    }
}


void SelectPlane::getValueVec(vector<SelectPlane::Coord>& ret)
{
    DASSERT(ret.size()==0);
   
    for(unsigned int i=0;i<wayp.size();i++)
    {
        ret.push_back(wayp[i].v);
    }
   
}


void SelectPlane::addValueMarker(const ValueMarker& vm)
{
    vmarks.push_back(vm);
}


void SelectPlane::setValueRange(coord_t xmin, coord_t xmax, coord_t ymin, coord_t ymax)
{
    this->xmin=xmin;
    this->xmax=xmax;
    this->ymin=ymin;
    this->ymax=ymax;
}


SelectPlane::~SelectPlane()
{
}

bool SelectPlane::hasLastSelected()
{
    return last_sel>-1;
}

int SelectPlane::getLastSelected()
{
    return last_sel;
   
}


SelectPlane::WPoint::WPoint(const SDL_Rect& s, const Coord& v)
{
    this->s=s;
    this->v=v;
}

SelectPlane::WPoint::WPoint(int screen_x, int screen_y, int val_x, int val_y)
{
    s.x=screen_x;
    s.y=screen_y;
    s.w=s.h=0;
   
    v.x=val_x;
    v.y=val_y;
}


//without it a square is used
void SelectPlane::setMarkerPixmap(SDL_Surface* m)
{
    marker=m;
}

void SelectPlane::setSelectedMarkerPixmap(SDL_Surface* m)
{
    sel_marker=m;
}


//used for connecting waypoints
void SelectPlane::setLineParam(const SDL_Color& color, int width)
{
    line_color=color;
    line_w=width;
    have_clc=false;
}

void SelectPlane::setMarkColor(const SDL_Color& c)
{
    mark_color=c;
}

void SelectPlane::setSelectedColor(const SDL_Color& c)
{
    sel_color=c;
}


//returns the currently selected waypoint's index
bool SelectPlane::hasSelected()
{
    return selected!=-1;
}

int SelectPlane::getSelected()
{
    return selected;
}

int SelectPlane::numWaypoints()
{
    return wayp.size();
}

void SelectPlane::remWaypoint(unsigned int n)
{
    DASSERT(n<wayp.size());
    if(n>=wayp.size())return;
   
    if(oneone)
    {
        //cant delete ends
        if(n==0 || n==wayp.size())return;
    }
   
   
    if((signed)n==selected)selected=-1;
    if((signed)n==last_sel)last_sel=-1;
   
    wayp.erase(wayp.begin()+n);
   
}

bool SelectPlane::addWaypoint(const SelectPlane::Coord& c)
{
    if(max>=0)
    {
        if(wayp.size()>=(unsigned)max)
        {
            DERROR("max limit");
            return false;
        }
    }
   
   
    WPoint wp;
    wp.v=clipVal(c);
    wp.s=valToScreen(wp.v);
   
    wayp.push_back(wp);
   
    if(oneone)stable_sort(wayp.begin(),wayp.end());
   
    return true;
}

void SelectPlane::showLine(bool v)
{
    show_line=v;
}


SelectPlane::Coord SelectPlane::getValue(unsigned int n)
{
    DASSERT(n< wayp.size());
   
    return wayp[n].v;
}

bool SelectPlane::setValue(unsigned int n, const Coord& c)
{
    DASSERT(n<wayp.size());
    if(n>=wayp.size())
    {
        DERROR("range");
        return false;
    }
   
    //overwrite
    WPoint& wp = wayp[n];
    wp.v=clipVal(c);
    wp.s=valToScreen(wp.v);
    if(oneone)stable_sort(wayp.begin(),wayp.end());
   
    return true;
}



int SelectPlane::maxWaypoints()
{
    return max;
}

void SelectPlane::maxWaypoints(int n)
{
    if(n>=0)
    {
        uint un = (unsigned)n;
	
        if(!oneone)
        {
            if(wayp.size()>un)
            {
                //drop last added
                wayp.resize(un);
            }
        }
        else
        {
            if(n<2)
            {
                DERROR("function mode req max>=2");
                n=2;
                setFunctionMode(); //reset to std
            }
            else
            {
                //save last(end point)
                WPoint c = wayp.back();
                wayp.resize(n);
                wayp.back() = c;
            }
        }
    }
   
    max=n;
}


void SelectPlane::setFunctionMode()
{
    wayp.clear();
   
    //start
    SDL_Rect s;
    s.w=s.h=0;
   
    s.x=0;
    s.y=h>>1;
    Coord v = screenToVal(s);
    wayp.push_back(WPoint(s,v));
   
    //end
    s.x=w;
    s.y=h>>1;
    v = screenToVal(s);
   
    wayp.push_back(WPoint(s,v));
   
    if(max>=0 && max<2)max=2;
    oneone=true;
}


Uint32 SelectPlane::getClickMoveDelay()
{
    return delay_click_move;
}


void SelectPlane::setClickMoveDelay(Uint32 d)
{
    delay_click_move=d;
}

//unused
void SelectPlane::mouseMove(int x, int y)
{
}

void SelectPlane::mousePos(int x, int y)
{
    if(!do_move)
    {
        Uint32 ticks = SDL_GetTicks();
	
        if( (ticks-just_click_time) < delay_click_move)
        {
            return;
        }
        do_move=true;
    }
   
   
    if(selected==-1)return;
   
    //move the point
    WPoint& wp = wayp[selected];
   
   
    if(!oneone)
    {
        wp.s.x=x-pos.x;
        wp.s.y=y-pos.y;
    }
    else
    {
        //have atleast 2 elms
        if(selected==0 || selected==(signed)wayp.size()-1)
        {
            wp.s.y=y-pos.y;
        }
        else
        {
            SDL_Rect want=wp.s;
            want.x=x-pos.x;
            want.y=y-pos.y;
	     
            //dont let caller move point out from left and right points
            if(want.x <= wayp[selected-1].s.x || want.x >= wayp[selected+1].s.x)
            {
                //clip x coord
                if(want.x <= wayp[selected-1].s.x)want.x = wayp[selected-1].s.x+1;
                if(want.x >= wayp[selected+1].s.x)want.x = wayp[selected+1].s.x-1;
            }
	     
            wp.s=want;
        }
    }
   
    wp.s = clipToDim(wp.s);
    wp.v = screenToVal(wp.s);
   
    if(mousemove_act!=NULL)mousemove_act->action(this);
}

SDL_Rect SelectPlane::clipToDim(const SDL_Rect& s)
{
    SDL_Rect r=s;
   
    if(s.x<0)r.x=0;
    if(s.x>w)r.x=w;
   
    if(s.y<0)r.y=0;
    if(s.y>h)r.y=h;
   
    return r;
}

SelectPlane::Coord SelectPlane::clipVal(const SelectPlane::Coord& s)
{
    Coord r=s;
   
    if(r.x<xmin)r.x=xmin;
    if(r.x>xmax)r.x=xmax;
   
    if(r.y<ymin)r.y=ymin;
    if(r.y>ymax)r.y=ymax;
   
    return r;
}


SelectPlane::Coord SelectPlane::screenToVal(const SDL_Rect& s)
{
    Coord v;
   
    coord_t vx = s.x/(coord_t)w;
    coord_t vy = s.y/(coord_t)h;
    //now between 0-1.0
   
    vx=xmin+(xmax-xmin)*vx;
    vy=ymax+(ymin-ymax)*vy;
   
    v.x=vx;
    v.y=vy;
   
    return v;
}


SDL_Rect SelectPlane::valToScreen(const SelectPlane::Coord& c)
{
    SDL_Rect r;
   
    r.x = (int)(w*(c.x-xmin)/(xmax-xmin));
    r.y = (int)(h*(c.y-ymax)/(ymin-ymax));
    r.w=0;
    r.h=0;
   
    return r;
}


//setAction() used
void SelectPlane::onClick(int x, int y)
{
    SDL_Rect p;
   
    p.x = x-pos.x;
    p.y = y-pos.y;
    p.w=p.h=0;
   
    p = clipToDim(p);
   
    Coord v = screenToVal(p);
   
    WPoint wp(p,v);
   
    //check if there is one to select
    //otherwise create new
   
    //vector<WPoint>::iterator it = find_if(wayp.begin(),wayp.end(), FindCloseby(wp, search_span_x, search_span_y));
    int found=-1;
    FindCloseby op(wp, search_span_x,search_span_y);
   
    for(unsigned int i=0;i<wayp.size();i++)
    {
        if(op(wayp[i]))
        {
            found=i;
            break;
        }
    }
   
    if(found==-1)
    {
        selected=-1;
        last_sel=-1;
	
        //create new
        if(max<0 || wayp.size()<(unsigned)max)
        {
            if(!oneone)
            {
                wayp.push_back(wp);
            }
            else
            {
                if(wp.s.x>0 && wp.s.x<w)
                {
                    //dont add on first's place
                    wayp.push_back(wp);
                }
            }
	     
            //FIXME: how to select?
            //selected=wayp.size()-1;
        }
        if(oneone)
        {
            //stable_sort is important
            stable_sort(wayp.begin(), wayp.end());
        }
    }
    else
    {
        //select
        selected=found;
        last_sel=selected;
    }
   
    just_click_time=SDL_GetTicks();
    do_move=false;
   
    if(act!=NULL)act->action(this);
}

void SelectPlane::onRelease()
{
    selected=-1;
}


void SelectPlane::draw(Screen* screen)
{
    if(wayp.size()==0)return;
   
    int mark_size=2;
    SDL_Rect start, end;
    start=wayp[0].s;
    start.x+=pos.x;
    start.y+=pos.y;
   
    //draw lines first for nicer output
    if(show_line)
    {
        for(unsigned int i=1;i<wayp.size();i++)
        {
            if(!have_clc)
            {
                c_line_color = screen->makeColor(line_color);
                have_clc=true;
            }
	     
            end=wayp[i].s;
            end.x+=pos.x;
            end.y+=pos.y;
	     
            screen->line(start,end, c_line_color, line_w);
	     
            start=end;
	     
        }
    }
   
    for(unsigned int i=0;i<wayp.size();i++)
    {
        SDL_Rect sp=wayp[i].s;
        sp.x+=pos.x;
        sp.y+=pos.y;
	
        if((signed)i==last_sel)
        {
            if(sel_marker==NULL)
            {
                screen->box(sp.x-mark_size,sp.y-mark_size,sp.x+mark_size, sp.y+mark_size, screen->makeColor(sel_color));
            }
            else screen->blit(sel_marker, &sp);
        }
        else
        {
            if(marker==NULL)
            {
                screen->box(sp.x-mark_size,sp.y-mark_size,sp.x+mark_size, sp.y+mark_size,screen->makeColor(mark_color));
            }
            else screen->blit(marker, &sp);
        }
    }
   
    vector<ValueMarker>::iterator it;
    it=vmarks.begin();
   
    while(it!=vmarks.end())
    {
        ValueMarker& v = *it;
        SDL_Rect s = valToScreen(Coord(*v.data,0));
        s.x+=pos.x;
        s.y=pos.y;
	
        SDL_Rect e = s;
        e.y+=h;
	
        screen->line(s, e, screen->makeColor(v.color), v.w);
	
        it++;
    }
   
}

SelectPlane::FindCloseby::FindCloseby(const WPoint& rf, int span_x, int span_y) : ref(rf)
{
    this->span_x=span_x;
    this->span_y=span_y;
}

SelectPlane::FindCloseby::result_type
SelectPlane::FindCloseby::operator()(argument_type r)
{
    if( r.s.x >= ref.s.x-span_x &&  r.s.x <= ref.s.x+span_x &&
    r.s.y >= ref.s.y-span_y &&  r.s.y <= ref.s.y+span_y)
        return true;
   
    return false;
}

}