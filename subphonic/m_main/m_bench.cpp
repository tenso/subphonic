#include "m_bench.h"

ProgramState& MainBench::progstate=ProgramState::instance();

/*************/
/*BenchStates*/
/*************/

BenchStates::BenchStates()
{
}

BenchStates::~BenchStates()
{
    for(state_map_it it=states.begin();it!=states.end();it++)
    {
        delete it->second;
    }
    states.clear();
}


void BenchStates::removeCompStates(SoundComp* parent)
{
    DASSERT(parent!=NULL);
   
    //use find instead of operator[] so as not to add anything not present
    state_map_it it = states.find(parent);
   
    if(it==states.end())
    {
        //DERROR("(W/E) SoundComp's State not found");
        //FIXME: often here when comp have no state
        return;
    }
   
    CompStates* tmp = it->second;
   
    delete tmp;
   
    states.erase(it);
}

unsigned int BenchStates::numCompStates()
{
    return states.size();
}

CompStates*  BenchStates::getCompStates(SoundComp* parent)
{
    state_map_it it = states.find(parent);
   
    if(it==states.end())
    {
        //NOT and error, only not present(yet)
        return NULL;
    }
   
    return it->second;
}

void BenchStates::addCompStates(SoundComp* parent, CompStates* state)
{
    state_map_it it = states.find(parent);
    if(it!=states.end())
    {
        DERROR("comp state already present, overwrite");
    }
   
    states[parent]=state;
}


state_map* BenchStates::getAllStates()
{
    return &states;
}

void BenchStates::applyAll(uint index, bool seek_backward)
{
    for(state_map_it it = states.begin();it!=states.end();it++)
    {
        CompStates* cs = it->second;
        DASSERT(cs!=NULL);
	
        //fetch index of state to the "left" or on this slot
        int tmpi=index;
        if(seek_backward)
        {
            tmpi = cs->lastUsedIndex(index);
        }
	
	
        if(tmpi>=0)cs->applyIndex(tmpi);
	
        //else no state
	
        //cs->applyIndex(index);
    }
}

void BenchStates::resaveAllInCurrLoadVer(int from_version)
{
    ProgramState& progstate = ProgramState::instance();
   
    //save
    int old_ver = progstate.getCurrentLoadVersion();
   
    //over all CompState*
    for(state_map_it it = states.begin();it!=states.end();it++)
    {
        CompStates* compstates = it->second;
        DASSERT(compstates!=NULL);
	
        //over all "index":
        CompStates::s_map& smap = compstates->getStates();
        CompStates::s_map::iterator s_it;
	
        //some error checking:
        uint todo =compstates->getNumUsed();
        uint done=0;
        for(s_it = smap.begin(); s_it != smap.end();s_it++)
        {
            //load state in old_version
            progstate.setCurrentLoadVersion(from_version);
	     
            SCState* state = s_it->second;
            DASSERT(state!=NULL);
	     
            state->applyState();
	     
            //save in new:
            progstate.setCurrentLoadVersion(FILEFORMAT_VERSION);
	     
            //setup with same parent(i.e "resave")
            state->setup(state->getParent());
	     
            state->applyState();
	     
            done++;
        }
	
        DASSERT(todo==done);
    }
   
    progstate.setCurrentLoadVersion(old_ver);
}




/***********/
/*MainBench*/
/***********/

MainBench::MainBench(int x, int y, Screen* s, BitmapFont* f)
{
    SurfaceHoldAutoBuild& pix = SurfaceHoldAutoBuild::instance();
   
    this->s=s;
   
    this->x=x;
    this->y=y;
   
    min_index=0;
    max_index=MAX_STATES;
   
    coff=0;
    cstate=0;
    stopmarker=NUM_SLOTS-1;
    startmarker=0;
   
    slotsel = new SlotSelect(pix["b_slot"], pix["b_slot_sel"], NUM_SLOTS/*, 16*/);
    slotsel->setCurrent(cstate%NUM_SLOTS);
     
    cont = new Container(x,y);
   
    int xoff=14+5*8+2;
   
    Button* left = new Button(pix["leftarrow_up"],pix["leftarrow_down"]);
    left->setStayPressed(false);
    left->setAction(new C_StateOff(this, -1));
    cont->add(left,xoff ,32);
   
    Button* right = new Button(pix["rightarrow_up"],pix["rightarrow_down"]);
    right->setStayPressed(false);
    right->setAction(new C_StateOff(this, 1));
    cont->add(right, xoff+10,32);
   
    //cont->addAbs(new Pixmap(pix["insignia"]), 850, 10);
   
    cont->add(slotsel,0,0);
   
    c_lab = new Label(f);
    cont->add(c_lab, 10,32);
    c_lab->printF("state    : %d", cstate);
   
    c_lab_bg = NULL;
   
    markers_loop=false;
   
    current_state=NULL;
}

MainBench::~MainBench()
{
    delete cont;
}

void MainBench::setStartMarkerIndex(uint m)
{
    /*DASSERT(m<nstates);
      if(m>=nstates)m=nstates-1;*/
   
    startmarker=m;
}

uint MainBench::getStartMarkerIndex()
{
    return startmarker;
}


uint MainBench::getStopMarkerIndex()
{
    return stopmarker;
}

void MainBench::setStopMarkerIndex(uint m)
{
    /*DASSERT(m<nstates);
      if(m>=nstates)m=nstates-1;*/
   
    stopmarker=m;
}


uint MainBench::getCurrentMarkerIndex()
{
    return cstate;
}

void MainBench::setCurrentMarkerIndex(uint m, bool apply_state)
{
    cstate=m;
   
    if(cstate>=MAX_STATES)
    {
        DERROR("truncating cstate");
        cstate=MAX_STATES-1;
    }
   
    coff = NUM_SLOTS*(cstate/NUM_SLOTS);
   
    slotsel->setCurrent(cstate%NUM_SLOTS);
    if(apply_state)applyStates();
   
   
    c_lab->printF("state    : %d", cstate);
}

uint MainBench::getNumSlots()
{
    return NUM_SLOTS;
}

uint MainBench::getMaxStates()
{
    return MAX_STATES;
}

BenchStates* MainBench::getStates()
{
    return &states;
}


bool MainBench::handleReleaseOver()
{
    cont->releaseSelected();
    return true;
}

bool MainBench::handleClickOver(int x, int y, int button, bool l_shift)
{
    if(button==2) //right
    {
        int old = slotsel->getCurrent();
        if(slotsel->clickSelected(x, y) == NULL)return false;
        int sel = slotsel->getCurrent()+coff;
        slotsel->setCurrent(old);
	
        //check if marker already exist
        vector<int>::iterator it;
        it = find(sel_list.begin(), sel_list.end(), sel);
	
        //rem list op
        if(it!=sel_list.end())sel_list.erase(it);
        //add list op
        else sel_list.push_back(sel); 
	
	
        if(l_shift)
        {
            if(sel_list.size()>1)
            {
                sel_list.clear();
                sel_list.push_back(sel);
            }
	     
        }
	
    }
    else if(button==1) //middle
    {
        if(l_shift)
        {
            //set start marker
            int old = slotsel->getCurrent();
            if(slotsel->clickSelected(x, y) == NULL)return false;
	     
            startmarker=slotsel->getCurrent()+coff;
            slotsel->setCurrent(old);
        }
        else
        {
            //set stop marker
            int old = slotsel->getCurrent();
            if(slotsel->clickSelected(x, y) == NULL)return false;
	     
            stopmarker=slotsel->getCurrent()+coff;
            slotsel->setCurrent(old);
        }
	
    }
    else if(button==0)
    {
        //left, 
	
        //set what state
        if(slotsel->clickSelected(x, y) != NULL)
        {
            cstate=slotsel->getCurrent()+coff;
	     
            //cant change coff
            //coff = NUM_SLOTS*(cstate/NUM_SLOTS);
	     

            c_lab->printF("state    : %d", cstate);
	     
            if(l_shift)return true; 
	     
            applyStates();
        }
        else //not slotsel that is selected...
        {
            Component* c = cont->clickSelected(x,y);
            if(c==NULL)return false;
        }
    }
   
    return true;
}

void MainBench::applyStates()
{
    //now set states
    if(progstate.getStateTickAll()) //DO ALL
    {
        states.applyAll(cstate, true/*seek*/);
    }
    else //DO ONE
    {
        applyCurrent(cstate, true);
    }
}


void MainBench::setAllStates()
{
    states.applyAll(cstate); //i.e no seek for step
}

void MainBench::setCurrentState()
{
    CompStates* curr = getCurrentCompStates();
    if(curr==NULL)
    {
        ERROR("currentCompState is NULL");
        return;
    }
   
    //curr->setState(cstate,stopmarker);
    curr->applyIndex(cstate);
}

//FIXME: check this
void MainBench::tickStates()
{
    bool all = progstate.getStateTickAll();
   
    if(markers.size()>0)
    {
        if(c_marker_i>=0 && (unsigned)c_marker_i==markers.size()-1) //UNCHECKED 
        {
            if(markers_loop)
            {
                c_marker=markers.begin();
            }
            else return;
        }
        else
        {
            c_marker++;
            c_marker_i++;
        }
	
        int next = *c_marker;
	
        if((next<min_index || next>max_index))
        {
            DERROR("marker list element: invalid range, removing list");
            voidMarkerList();
            return;
        }
	
        cstate=next;
        coff = NUM_SLOTS*(cstate/NUM_SLOTS);
    }
    else //normal linear step
    {
        cstate++;
	
        if(cstate>stopmarker)
        {
            cstate=startmarker;
        }
        coff = NUM_SLOTS*(cstate/NUM_SLOTS);
	
        //cstate%=(stopmarker+1);
    }
   
    //update slotsel
    slotsel->setCurrent(cstate%NUM_SLOTS);
   
    if(!all)setCurrentState();
    else setAllStates();
   
    c_lab->printF("state    : %d", cstate);
}


void MainBench::handleMouseMotion(int x_m, int y_m)
{
    /*if(out->hasSelected())
      {
      out->giveMouseMotion(in->xMouseMove(),in->yMouseMove());
      out->giveMousePosition(in->xMouse(),in->yMouse());
      }*/
   
}

void MainBench::draw()
{
    //slotsel->showIndex(cstate%NUM_SLOTS);//made before draw, so it's updated before drawn
   
    int dx=4;
    int dy=2;
    /*
      s->box(cont->getPos().x+1-dx,cont->getPos().y+1-dy, 
	  cont->getPos().x+cont->getPos().w-1+dx, cont->getPos().y+cont->getPos().h-1+dy, 
	  s->makeColor(0,0,0));
   
      s->wirebox(cont->getPos().x-dx, cont->getPos().y-dy, 
      cont->getPos().x+cont->getPos().w+dx, cont->getPos().y+cont->getPos().h+dy, 
      s->makeColor(128,128,128));*/
   
    //draw components
    cont->draw(s);
   
    //if(!out->isOn() && old_c>0)old_c--;
   
    //int xindent=96+5;
   
    static Uint32 ecolor = s->makeColor(255,255,255);
    //Uint32 emcolor = s->makeColor(200,200,200);
    static Uint32 emmcolor = s->makeColor(175,175,175);
   
    static Uint32 ucolor = s->makeColor(40,40,40);
    //Uint32 umcolor = s->makeColor(15,15,15);
    static Uint32 ummcolor = s->makeColor(0,0,0);
   
    CompStates* curr = getCurrentCompStates();
   
    //draw states for current selected comp
    if(curr!=NULL)
    {
        Uint32 color;
        for(unsigned int x=0;x<NUM_SLOTS;x++)
        {
            if(curr->indexInUse(x+coff))
            {
                SDL_Rect pos = slotsel->getIndexRect(x);
                color=emmcolor;
                //if(x%4==0)color=emcolor;
                if(x%8==0)color=ecolor;
                s->box(pos.x+1,pos.y+2,pos.x+pos.w-1,pos.y+pos.h-2, color);
            }
            else
            {
                SDL_Rect pos = slotsel->getIndexRect(x);
                color=ummcolor;
                //if(x%4==0)color=umcolor;
                if(x%8==0)color=ucolor;
                s->box(pos.x+1,pos.y+2,pos.x+pos.w-1,pos.y+pos.h-2, color);
            }
        }
    }
    else
    {
        Uint32 color;
        for(unsigned int x=0;x<NUM_SLOTS;x++)
        {
	     
            SDL_Rect pos = slotsel->getIndexRect(x);
            color=ummcolor;
            //if(x%4==0)color=umcolor;
            if(x%8==0)color=ucolor;
            s->box(pos.x+1,pos.y+2,pos.x+pos.w-1,pos.y+pos.h-2, color);
        }
    }
   
    //draw startmarker(if at same off as cstate)
    if(startmarker/NUM_SLOTS == cstate/NUM_SLOTS)
    {
        SDL_Rect pos = slotsel->getIndexRect(startmarker%NUM_SLOTS);
        static Uint32 color = s->makeColor(0,255,255);
        s->wirebox(pos.x+2,pos.y+3,pos.x+pos.w-3,pos.y+pos.h-4, color);
    }
   
   
    //draw stopmarker
    if(stopmarker/NUM_SLOTS == cstate/NUM_SLOTS)
    {
        SDL_Rect pos = slotsel->getIndexRect(stopmarker%NUM_SLOTS);
        static Uint32 color = s->makeColor(255,0,0);
        s->wirebox(pos.x+1,pos.y+2,pos.x+pos.w-2,pos.y+pos.h-3, color);
    }
   
    //draw sel_list
    for(uint i=0;i<sel_list.size();i++)
    {
        //range is:
        if(coff <= sel_list[i] && sel_list[i] < coff+NUM_SLOTS)
        {
            SDL_Rect pos = slotsel->getIndexRect(sel_list[i]%NUM_SLOTS);
            static Uint32 color = s->makeColor(255,0,255);
            s->wirebox(pos.x+1,pos.y+2,pos.x+pos.w-2,pos.y+pos.h-3, color);
        }
	
    }
   
   
}

void MainBench::currentState(SoundComp* comp)
{
    //comp=NULL is a deselect command
    if(comp==NULL)
    {
        setCurrentCompStates(NULL);
        return;
    }
   
    //FIXMENOW2
    //states of size 0 can not be saved:
    //if they would there would be states of size=0 in program for save etc...
   
    SoundCompEvent* e = new SoundCompEvent();
    comp->getState(e);
    uint size = e->getSize();
    delete e;
   
    if(size==0)
    {
        setCurrentCompStates(NULL);
        return;
    }
   
    CompStates* cs = states.getCompStates(comp);
   
    if(cs==NULL)
    {
        //add new state for this comp
	
        CompStates* tmp = new CompStates(comp);
	
        //tmp->saveIndex(0); //save initial state of comp
	
        states.addCompStates(comp, tmp);
    }
   
    setCurrentCompStates(states.getCompStates(comp)); //set current state
}

bool MainBench::saveCurrentStateIndex()
{
    CompStates* curr = getCurrentCompStates();
    if(curr==NULL)return false;
   
    return getCurrentCompStates()->saveIndex(cstate);
}

bool MainBench::saveAllStateIndex()
{
    state_map* allstates = states.getAllStates();
   
    for(state_map_it it = allstates->begin();it!=allstates->end();it++)
    {
        it->second->saveIndex(cstate);
    }
    return true;
}

bool MainBench::delCurrentStateIndex()
{
    CompStates* curr = getCurrentCompStates();
    if(curr==NULL)return false;
   
    getCurrentCompStates()->delIndex(cstate);
   
    return true;
}

//deletes all states that are assosiated with index
bool MainBench::delAllStateIndex()
{
    state_map* allstates = states.getAllStates();
   
    for(state_map_it it = allstates->begin();it!=allstates->end();it++)
    {
        it->second->delIndex(cstate);
    }
   
    return true;
}

bool MainBench::delAllForCurrentStateIndex()
{
    CompStates* curr = getCurrentCompStates();
    if(curr==NULL)return false;
   
    getCurrentCompStates()->delAll();
   
    return true;
}

void MainBench::removeCompState(SoundComp* parent)
{
   
    CompStates* cs = getCurrentCompStates();
    if(cs!=NULL && cs->getParent()==parent)setCurrentCompStates(NULL);
    states.removeCompStates(parent);
   
}

void MainBench::moveCurrentStateIndex(int off)
{
    CompStates* cs = getCurrentCompStates();
   
    if(cs==NULL)
    {
        return;
    }
   
    int done = cs->pushStatesFromIndex(cstate, off, min_index, max_index);
   
    //let marker follow push
    cstate+=done;
    coff = NUM_SLOTS*(cstate/NUM_SLOTS);
    slotsel->setCurrent(cstate%NUM_SLOTS);
}

void MainBench::moveAllStateIndex(int off)
{
    //CompStates* cs = getCurrentCompStates();
    //cs can be NULL
   
    uint done=0;
   
    state_map* allstates = states.getAllStates();
   
    for(state_map_it it = allstates->begin();it!=allstates->end();it++)
    {
        uint d = it->second->pushStatesFromIndex(cstate, off, min_index, max_index);
	
        /*if(cs==it->second)*/done = d;
    }
   
    //let marker follow push
    cstate+=done;
    coff = NUM_SLOTS*(cstate/NUM_SLOTS);
    slotsel->setCurrent(cstate%NUM_SLOTS);
   
    //FIXME: good?
    stopmarker+=done;
}


uint MainBench::getMarkerListIndex()
{
    return c_marker_i;
}


void MainBench::setMarkerListIndex(uint index, bool all, bool apply_now)
{
    if(markers.size()==0)return;
   
    c_marker = markers.begin();
    c_marker_i=0;
   
    for(uint i=0;i<index;i++)
    {
        c_marker++;
        c_marker_i++;
	
        if(c_marker==markers.end())
        {
            break;
        }
    }
   
    if(apply_now)
    {
        cstate=*c_marker;
        coff = NUM_SLOTS*(cstate/NUM_SLOTS);
	
        slotsel->setCurrent(cstate%NUM_SLOTS);
	
        if(!all)setCurrentState();
        else setAllStates();
	
        c_lab->printF("state    : %d", cstate);
    }
    else if(c_marker_i!=-1)
    {
        c_marker--;
        c_marker_i--;
    }
   
}

void MainBench::seekMarkerListIndex(int dir, bool all, bool apply_now)
{
    if(markers.size()==0)return;
   
    if(dir==0)return;
   
    uint aindex=abs(dir);
    bool d = (dir>0) ? 1 : 0;
   
    for(uint i=0;i<aindex;i++)
    {
        if(d)
        {
            if(c_marker_i>=0 && (unsigned)c_marker_i==markers.size()-1) //ok:markers.size()>=1
            {
                break;
            }
	     
            c_marker++;
            c_marker_i++;
        }
        else
        {
            if(c_marker_i==0)
            {
                break;
            }
	     
            c_marker--;
            c_marker_i--;
        }
    }
   
    if(apply_now)
    {
        cstate=*c_marker;
        coff = NUM_SLOTS*(cstate/NUM_SLOTS);
	
        slotsel->setCurrent(cstate%NUM_SLOTS);
	
        if(!all)setCurrentState();
        else setAllStates();
	
        c_lab->printF("state    : %d", cstate);
    }
    else if(c_marker_i!=-1)
    {
        c_marker--;
        c_marker_i--;
    }
   
}


void MainBench::setMarkerList(const list<int>& markers, bool all, bool apply_now)
{
    if(markers.size()==0)return;
   
    this->markers=markers;
   
    c_marker=this->markers.begin();
    c_marker_i=0;
   
    //will set first
    if(apply_now)
    {
        cstate=*c_marker;
        coff = NUM_SLOTS*(cstate/NUM_SLOTS);
	
        slotsel->setCurrent(cstate%NUM_SLOTS);
	
        if(!all)setCurrentState();
        else setAllStates();
	
        c_lab->printF("state    : %d", cstate);
    }
    else
    {
        c_marker--;
        c_marker_i--;
    }
   
}

void MainBench::voidMarkerList()
{
    markers = list<int>();
}

void MainBench::loopMarkerList(bool val)
{
    markers_loop=val;
}




CompStates*  MainBench::getCurrentCompStates()
{
    return current_state;
}

void MainBench::setCurrentCompStates(CompStates* cs)
{
    current_state=cs;
}


void MainBench::applyCurrent(uint index, bool seek_backward)
{
    CompStates* curr = getCurrentCompStates();
    if(curr!=NULL)
    {
	
        int tmpi=index;
        if(seek_backward)tmpi = curr->lastUsedIndex(index);
	
        if(tmpi>=0)curr->applyIndex(tmpi);
    }
}


void MainBench::clearSelList()
{
    sel_list.clear();
}

uint MainBench::inSelList()
{
    return sel_list.size();
}

void MainBench::currentDelSelList()
{
    CompStates* cs = getCurrentCompStates();
    if(cs==NULL)
    {
        ERROR("currentCompState is NULL");
        return;
    }
   
    for(uint i=0;i<sel_list.size();i++)
    {
        int index = sel_list[i];
	
        cs->delIndex(index);
    }
   
}

void MainBench::allDelSelList()
{
    state_map* smap = states.getAllStates();
   
    for(state_map_it it = smap->begin();it!=smap->end();it++)
    {
        CompStates* cs = it->second;
        DASSERT(cs!=NULL);
	
        for(uint i=0;i<sel_list.size();i++)
        {
            int index = sel_list[i];
	     
            cs->delIndex(index);
        }
    }
}

void MainBench::currentCopySelListToMarkerIndex()
{
    sort(sel_list.begin(), sel_list.end());
   
   
    CompStates* cs = getCurrentCompStates();
    if(cs==NULL)
    {
        return;
    }
   
    for(uint i=0;i<sel_list.size();i++)
    {
        int first = sel_list[0];
        int from = sel_list[i];
        int to = cstate+(from-first);
	
        cs->copyIndex(from, to);
    }
   
    applyCurrent(cstate, false/*seek*/);
}

void MainBench::allCopySelListToMarkerIndex()
{
    state_map* smap = states.getAllStates();
   
    sort(sel_list.begin(), sel_list.end());
   
    for(state_map_it it = smap->begin();it!=smap->end();it++)
    {
        CompStates* cs = it->second;
        DASSERT(cs!=NULL);
	
        for(uint i=0;i<sel_list.size();i++)
        {
            int first = sel_list[0];
            int from = sel_list[i];
            int to = cstate+(from-first);
	     
            cs->copyIndex(from, to);
        }
    }
   
    states.applyAll(cstate, false/*seek*/);
   
}



MainBench::C_StateOff::C_StateOff(MainBench* src, int dir)
{
    this->src=src;
    this->dir=dir;
}

void MainBench::C_StateOff::action(Component* c)
{
    if(dir<0)
    {
        if(src->getCurrentMarkerIndex()>=NUM_SLOTS)
            src->setCurrentMarkerIndex(src->getCurrentMarkerIndex()-NUM_SLOTS);
        //FIXME:
        //src->setCurrentMarkerIndex(index, !in.keySet(SDLK_LSHIFT)) ;
    }
    else
    {
        if(src->getCurrentMarkerIndex()<MAX_STATES-NUM_SLOTS)
            src->setCurrentMarkerIndex(src->getCurrentMarkerIndex()+NUM_SLOTS);
    }
   
   
}



/*
  bool MainBench::handleInput(Input& in)
  {
  int x = in.xMouse();
  int y = in.yMouse();
  * 
  bool handled=false;
  * 
  * 
  if(in.mouseReleaseEvent())
  {
  handleReleaseOver();
  * 
  }
  * 
  if(in.mousePressEvent())
  {
  int next=in.nextMButton();
  while(next!=-1)
  {
  handleClickOver(x,y, next, !in.keySet(SDLK_LSHIFT));
  * 
  next=in.nextMButton();
  }
  }
  * 
  handleMouseMotion(x_m, y_m);
  * 
  return handled;
  }*/
