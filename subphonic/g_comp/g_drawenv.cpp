#include "g_drawenv.h"


//use this for static curve also
//spline mode
GDrawEnv::GDrawEnv(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    v = new ConvertHz2Per(Convert(progstate.getSampleRate()),&empty);
   
    sig = new CPInterpolator();
   
    sig->setInput(NULL);
    sig->setPeriod((Value**)&v);
    sig->setGate(NULL);
    sig->setGateTrigOn(false);
   
    sust_index= (int)(MAX_INDEX*0.5);
    sig->setSustainIndex((int)sust_index);
   
    plane = new SelectPlane(192,96);
    plane->maxWaypoints(MAX_WAYP);
    plane->setValueRange(0, MAX_INDEX, -SMP_MAX, SMP_MAX);
   
    SDL_Color color;
    color.r=0;
    color.g=200;
    color.b=100;
   
    SelectPlane::ValueMarker v;
    v.data=sig->getDataIndexPtr();
    v.w=1;
    v.color=color;
    v.mode=SelectPlane::ValueMarker::LINE;
   
    plane->addValueMarker(v);
   
    color.r=200;
    color.g=0;
    color.b=100;
   
    v.data=&sust_index;
    v.color=color;
   
    plane->addValueMarker(v);
   
   
    plane->setAction(new C_UpdatePAD(this));
    plane->setMouseMoveAction(new C_UpdatePAD(this));
    plane->setFunctionMode();
    plane->setClickMoveDelay(75);
    plane->showLine(true);
   
    ProgramState& ps = ProgramState::instance();
    plane->setLineParam(ps.getGraphColor());
   
    vals.clear();
    sigvals.clear();
   
    plane->getValueVec(vals);
    convertCoords(vals, sigvals);
   
    sig->setData(sigvals);
   
   
   
    Pixmap* back_pix = new Pixmap(pix["320x158"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Pixmap* vec_pix = new Pixmap(pix["vback192x96"]);
   
    int xp=116;
    int yp=24;
    add(vec_pix, xp,yp);
    add(plane,xp,yp);
   
    for(int i=0;i<3;i++)
    {
        in[i] = new InputTaker(pix["in"], this, i);
        addInputTaker(in[i]);
    }
   
    Label* l = new Label(fnt["label"]);
    l->printF("graphical envelope/osc");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
      
    //input
    add(in[0], GCOMP_STDIN_XOFF,GCOMP_STDIN_YOFF);
   
   
    int xoff=12;
    int yoff=-40;
   
    //freq
    l = new Label(f);
    l->printF("frq");
    add(l, -2+xoff, 70+yoff);
   
    add(in[1],36+xoff, 68+yoff);
   
    //GATE
      
    add(in[2],36+xoff,88+yoff);
   
    l = new Label(f);
    l->printF("gate");
    add(l, -2+xoff, 90+yoff);
   

    l = new Label(f);
    l->printF("off");
    add(l, 6+xoff, 105+yoff);
   
    gate_on = new Button(pix["bs_left"],pix["bs_right"]);
    gate_on->stayPressed(true);
    gate_on->setAction(new C_GateOn(this));
    add(gate_on, 30+xoff,100+yoff);
   
    l = new Label(f);
    l->printF("on");
    add(l, 55+xoff, 105+yoff);
   
    LedsLevel<smp_t>* led=new LedsLevel<smp_t>(pix["led_on"],pix["led_off"]);
    led->setData(sig->gateValPtr());
    led->setThreshold(GATE_ZERO_LVL);
    add(led, 52+xoff,81+yoff);
   

    og = new OutputGiver(pix["out"],this,0);
    addOutputGiver(og);
    add(og,GCOMP_STDOUT_XOFF,GCOMP_STDOUT_YOFF);
   
    rem_sel = new Button(pix["up"],pix["down"]);
    rem_sel->stayPressed(false);
    rem_sel->setAction(new C_RemSel(this));
    add(rem_sel, 95,26);
   
      
    sust_slide = new Slider(pix["slide_knobv"], pix["slide_underv"],true);
    sust_slide->setMouseMoveAction(new C_SustPos(this));
    add(sust_slide, 95,46);
    sust_slide->setValue(0.5);
   
    drain = new Drain(this);
    out = new SmpPoint(&drain->cval);
   
    ground.addInput((Value**)&drain);
   
    yoff = 130;
    xoff = 20;
   
    l = new Label(f);
    l->printF("hold end.v");
    add(l, xoff, yoff+5);
   
    holdend = new Button(pix["up"],pix["down"]);
    holdend->stayPressed(true);
    holdend->setAction(new C_HOLDEND(this));
    add(holdend, xoff+90,yoff);
   
    xoff+=140;
   
    l = new Label(f);
    l->printF("fin cycle");
    add(l, xoff, yoff+5);
   
    fincyc = new Button(pix["up"],pix["down"]);
    fincyc->stayPressed(true);
    fincyc->setAction(new C_FINCYC(this));
    add(fincyc, xoff+80,yoff);
   
    sig->setGateFinishCycle(fincyc->getValue());
    sig->setHoldEndValue(holdend->getValue());
}

GDrawEnv::~GDrawEnv()
{
    ground.remInput((Value**)&drain);
   
    delete out;
    delete drain;
    delete sig;
    delete v;

}


void GDrawEnv::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(out);
    if(fromid==1)v->setInput(out);
    if(fromid==2)sig->setGate(out);
}

void GDrawEnv::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(NULL);
    if(fromid==1)v->setInput(NULL);
    if(fromid==2)sig->setGate(NULL);
}

Value** GDrawEnv::getOutput(unsigned int id)
{
    return (Value**)&out;
}

/*******/
/*STATE*/
/*******/

void GDrawEnv::getState(SoundCompEvent* e)
{
    State s(vals, sust_slide->getValue(), gate_on->getValue(),
    holdend->getValue(), fincyc->getValue());

    e->copyData((char*)&s, sizeof(State));
}

void GDrawEnv::setState(SoundCompEvent* e)
{
    DASSERT(e!=NULL);
    if(e->empty())return;
    if(e->getSize()!=sizeof(State))
    {
        DERROR("size missmatch");
        if(e->getSize() < sizeof(State))return;
	
        DERROR("trying to read...");
    }
    const State* s = (const State*)e->getData();
   
    sust_slide->setValue(s->sust);
    sust_index = sust_slide->getValue()*MAX_INDEX;
    sig->setSustainIndex((int)sust_index);
   
    gate_on->setValue(s->b_gate);
    sig->setGateTrigOn(gate_on->isPressed());

    //set data
    vals.clear();
    for(unsigned int i=0;i<s->n_wp;i++)vals.push_back(s->data[i]);
   
    plane->setWaypoints(vals);
   
    sigvals.clear();
    convertCoords(vals, sigvals);
   
    sig->setData(sigvals);
   
    holdend->setValue(s->holdend);
    fincyc->setValue(s->fincyc);
   
    sig->setGateFinishCycle(fincyc->getValue());
    sig->setHoldEndValue(holdend->getValue());
}


/***********/
/*callbacks*/
/***********/

void GDrawEnv::C_UpdatePAD::action(Component* co)
{
    //SelectPlane* s = (SelectPlane*)co;

    src->vals.clear();
    src->plane->getValueVec(src->vals);
   
    src->sigvals.clear();
    convertCoords(src->vals, src->sigvals);
   
    src->sig->setData(src->sigvals);
   
   
    /*   for(int i=0;i<src->vals.size();i++)
         {
         cout << src->vals[i].x << " " << src->vals[i].y << endl;
         }*/
   
    /*if(src->plane->hasLastSelected())
      {
      //Coord c = s->getValue(s->getLastSelected());
      //cout << c.x << " " << c.y << endl;
      }
    */
}

void GDrawEnv::C_RemSel::action(Component* co)
{
    if(src->plane->hasLastSelected() && src->plane->numWaypoints()>2)
    {
        src->plane->remWaypoint(src->plane->getLastSelected());
	
        //Coord c = src->plane->getValue(0);
    }
}

void GDrawEnv::C_GateOn::action(Component* co)
{
    Button* b = (Button*)co;
    src->sig->setGateTrigOn(b->isPressed());
}

void GDrawEnv::C_SustPos::action(Component* co)
{
    Slider* s = (Slider*)co;
    src->sust_index = s->getValue()*MAX_INDEX;
    src->sig->setSustainIndex((int)src->sust_index);
}


/*DRAIN*/

GDrawEnv::Drain::Drain(GDrawEnv* src)
{
    this->src=src;
}

smp_t GDrawEnv::Drain::nextValue()
{
    cval = src->sig->nextValue();
    return 0;
}


void GDrawEnv::C_HOLDEND::action(Component* co)
{
    src->sig->setHoldEndValue(src->holdend->getValue());
}

void GDrawEnv::C_FINCYC::action(Component* co)
{
    src->sig->setGateFinishCycle(src->fincyc->getValue());
}

