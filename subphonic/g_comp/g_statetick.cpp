#include "g_statetick.h"

GStateTick::GStateTick(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    ticker_bench = NULL;
    ticker_states = NULL;
   
    Pixmap* back_pix = new Pixmap(pix["96x64"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    add(addInputTaker(new InputTaker(pix["in"], this, 0)), 5, 3);
   
   
    //inverted
    on_button = new Button(pix["bs_right"], pix["bs_left"]);
    on_button->setPressed(true);
    on_button->stayPressed(true);
    //on_button->setAction(new C_FLIP(&ticker->on));
   
    Label* l = new Label(fnt["label"]);
    l->printF("statetick");
    add(l, 20, GCOMP_LABEL_YOFF);
   
    l = new Label(f, string("on"));
    add(l, 10, 30);
   
    l = new Label(f, string("off"));
    add(l, 60, 30);
   
    add(on_button, 33,25);
   
   
    int xoff=10;
    int yoff=48;
   
    l = new Label(f, "index");
    add(l, xoff, yoff);
   
    add(addInputTaker(new InputTaker(pix["in"], this, 1)), xoff+45, yoff-2);
}

GStateTick::~GStateTick()
{
    //this is important, must remove self from ground before free!
    if(ticker_bench!=NULL)
    {
        ground.remInput((Value**)&ticker_bench);
        delete ticker_bench;
    }
   
    if(ticker_states!=NULL)
    {
        ground.remInput((Value**)&ticker_states);
        delete ticker_states;
    }
}
void GStateTick::setBench(MainBench* b)
{
    DASSERT(ticker_bench==NULL && ticker_states==NULL);
   
    ticker_bench = new StateTickerBench(this);
    ticker_bench->setBench(b);
   
    ground.addInput((Value**)&ticker_bench);
}

void GStateTick::setBenchStates(BenchStates* bs, int current_slot, int start_marker,
int stop_marker, int max_slots)
{
    DASSERT(ticker_bench==NULL && ticker_states==NULL);
   
    ticker_states = new StateTickerStateOnly(this);
    ticker_states->setBenchStates(bs, current_slot, start_marker,
    stop_marker,  max_slots);
   
    ground.addInput((Value**)&ticker_states);
}


void GStateTick::addInput(Value** out, unsigned int fromid)
{
    if(ticker_bench!=NULL)
    {
        if(fromid==0)ticker_bench->setTrig(out);
        else ticker_bench->setSel(out);
    }
    else if(ticker_states!=NULL)
    {
        if(fromid==0)ticker_states->setTrig(out);
        else ticker_states->setSel(out);
    }
   
}

void GStateTick::remInput(Value** out, unsigned int fromid)
{
    if(ticker_bench!=NULL)
    {
        if(fromid==0)ticker_bench->setTrig(NULL);
        else ticker_bench->setSel(NULL);
    }
    else if(ticker_states!=NULL)
    {
        if(fromid==0)ticker_states->setTrig(NULL);
        else ticker_states->setSel(NULL);
    }
   
}

/*******/
/*state*/
/*******/


void GStateTick::getState(SoundCompEvent* e)
{
    State s(on_button->getValue());
    e->copyData((char*)&s, sizeof(State));
}

void GStateTick::setState(SoundCompEvent* e)
{
    if(e->empty())return;
   
    if(e->getSize()!=sizeof(State))
    {
        DERROR("size missmatch");
        if(e->getSize() < sizeof(State))return;
	
        DERROR("trying to read...");
    }
   
    const State* s = (const State*)e->getData();
   
   
    on_button->setValue(s->on);
   
}


/******************/
/*STATETICKERBENCH*/
/******************/

GStateTick::StateTickerBench::StateTickerBench(GStateTick* src)
{
    this->src=src;
    bench=NULL;
    trig=NULL;
    sel_lvl=NULL;
    retrig=true;
   
}

void GStateTick::StateTickerBench::setBench(MainBench* b)
{
    bench=b;
}

void GStateTick::StateTickerBench::setTrig(Value** v)
{
    trig=v;
}

void GStateTick::StateTickerBench::setSel(Value** v)
{
    sel_lvl=v;
}

smp_t GStateTick::StateTickerBench::nextValue()
{
    //still drain inputs...
    smp_t tr=0;
    if(trig!=NULL)tr = (*trig)->nextValue();
   
    //EITHER
    if(sel_lvl!=NULL)
    {
        uint lvl = (uint)(fabs((*sel_lvl)->nextValue()));
	
        if(!src->on_button->getValue())
        {
            return 0;
        }
	
        uint c_lvl = bench->getCurrentMarkerIndex();
	
	
        if(lvl!=c_lvl)
        {
            if(lvl >= bench->getMaxStates())
            {
                lvl = bench->getMaxStates()-1;
            }
	     
            bench->setCurrentMarkerIndex(lvl, true);
	     
        }
	
        return 0;
    }
   
   
    //OR
   
   
    if(src->on_button->getValue())
    {
        if(tr > GATE_ZERO_LVL)
        {
            if(retrig)
            {
                DASSERTP(bench!=NULL, "(E) bench not set"); //if this happens something is really wrong
                bench->tickStates();
                retrig=false;
            }
        }
        else retrig=true;
    }
    else 
    {
        retrig=true;
    }
   
   
    return 0;
}




/******************/
/*STATETICKERSTATE*/
/******************/

GStateTick::StateTickerStateOnly::StateTickerStateOnly(GStateTick* src)
{
    this->src=src;

    trig=NULL;
    sel_lvl=NULL;
    retrig=true;
   
    bs=NULL;
}

void GStateTick::StateTickerStateOnly::setBenchStates(BenchStates* bs, int current_slot, int start_marker,
int stop_marker, int max_slots)
{
    this->bs=bs;
    this->current_slot=current_slot;
    this->start_marker=start_marker;
    this->stop_marker=stop_marker;
    this->max_slots=max_slots;
}

void GStateTick::StateTickerStateOnly::setTrig(Value** v)
{
    trig=v;
}

void GStateTick::StateTickerStateOnly::setSel(Value** v)
{
    sel_lvl=v;
}

smp_t GStateTick::StateTickerStateOnly::nextValue()
{
    //still drain inputs...
    smp_t tr=0;
    if(trig!=NULL)tr = (*trig)->nextValue();
   
    //EITHER
    if(sel_lvl!=NULL)
    {
        uint lvl = (uint)(fabs((*sel_lvl)->nextValue()));
	
        if(!src->on_button->getValue())
        {
            return 0;
        }
	
        uint c_lvl = current_slot;
	
	
        if(lvl!=c_lvl)
        {
            if(lvl >= max_slots)
            {
                lvl = max_slots-1;
            }
	     
            current_slot=lvl;
	     
            //this means module only applies all or nothing
            bs->applyAll(current_slot, true/*seek*/);
        }
	
        return 0;
    }
   
   
    //OR
   
   
    if(src->on_button->getValue())
    {
        if(tr > GATE_ZERO_LVL)
        {
            if(retrig)
            {
                DASSERTP(bs!=NULL, "(E) benchstate not set"); //if this happens something is really wrong
		  
                current_slot++;
		  
                if(current_slot>stop_marker)
                {
                    current_slot=start_marker;
                }
		  
                bs->applyAll(current_slot, false/*no seek*/);
		  
                retrig=false;
            }
        }
        else retrig=true;
    }
    else 
    {
        retrig=true;
    }
   
   
    return 0;
}
