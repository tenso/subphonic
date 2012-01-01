#include "g_env.h" 

GEnv::GEnv(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    per_v = new Const(0);
   
    converted = new ConvertSec2Len(Convert(progstate.getSampleRate()));
    converted->setInput((Value**)&per_v);
   
    sig = new EnvFollow(EnvFollow::PEAK, 1);
    sig->setPeriod((Value**)&converted);
   
   
    //background
    Pixmap* back_pix = new Pixmap(pix["96x144"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("env f.");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //input
    InputTaker* in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
   
    //output
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
    //levels
    l = new Label(f);
    l->printF("input");
    add(l, 4, 20);
   
    add(new LedsLevel<smp_t>(pix["led_on"], pix["led_off"], 8, false, sig->getInputVal()), 10,31);
   
    l = new Label(f);
    l->printF("envelope");
    add(l, 4, 45);
   
    add(new LedsLevel<smp_t>(pix["led_on"], pix["led_off"], 8, false, sig->getEnvelopeVal()) , 10,57);
   
   
    //period
    l = new Label(f);
    l->printF("cycle (s)");
    add(l, 4, 76);
   
    per_knob = new KnobSlider(pix["knob16x16"]);
    per_knob->setMouseMoveAction(new C_UP_PER(this));
    per_knob->setContinous(0,1.0,1.0/(float)KNOB_NREVS);
    per_knob->setTurnSpeed(KNOB_TURN_FAST);
    per_knob->setValue(0.1);
    per_v->setValue(per_knob->getValue());
   
    int xoff=10;
    int yoff=86+4;
   
    add(per_knob,xoff,yoff);
   
    in = new InputTaker(pix["in"], this, 1);
    addInputTaker(in);
    add(in, xoff+22, yoff+2);
   
    //mode
   
    l = new Label(f);
    l->printF("peak");
    add(l, 4, 124);
   
    mode = new Button(pix["bs_left"],pix["bs_right"]);
    mode->stayPressed(true);
    mode->setAction(new PMODE(this));
    add(mode, 40, 120);
   
    l = new Label(f);
    l->printF("rms");
    add(l, 66, 124);
   
   
    updatePer();
}

GEnv::~GEnv()
{
    delete per_v;
    delete converted;
    delete sig;
}

Value** GEnv::getOutput(unsigned int id)
{
    if(id==0)return (Value**)&sig;
    return NULL;
}

void GEnv::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(out);
    else converted->setInput(out);
}

void GEnv::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(NULL);
    else converted->setInput((Value**)&per_v);
}

void GEnv::updatePer()
{
    smp_t up_per = per_knob->getValue()*MAX_PERIOD;
    per_v->setValue(up_per);
   
    //sig->setUpdatePeriod(up_per);
}


/*state*/

void GEnv::getState(SoundCompEvent* e)
{
    State s(per_knob->getValue(), mode->getValue());
    e->copyData((char*)&s, sizeof(State));
}

void GEnv::setState(SoundCompEvent* e)
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
   
    mode->setValue(s->mode);
   
    if(mode->isPressed())
    {
        sig->setMode(EnvFollow::RMS);
    }
    else
    {
        sig->setMode(EnvFollow::PEAK);
    }
   
    per_knob->setValue(s->per);
    updatePer();
}



/*callbacks*/

GEnv::C_UP_PER::C_UP_PER(GEnv* src)
{
    this->src=src;
}

void GEnv::C_UP_PER::action(Component* c)
{
    src->updatePer();
}

GEnv::PMODE::PMODE(GEnv* src)
{
    this->src=src;
}

void GEnv::PMODE::action(Component* c)
{
    Button* s = (Button*)c;
   
    if(s->isPressed())
    {
        src->sig->setMode(EnvFollow::RMS);
    }
    else
    {
        src->sig->setMode(EnvFollow::PEAK);
    }
   
}
