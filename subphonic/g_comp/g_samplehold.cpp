#include "g_samplehold.h" 

GSampleAndHold::GSampleAndHold(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    //"defines"
    const smp_t MIN_HZ=0;
    const smp_t MAX_HZ=10;
    const smp_t MIN_LEN=1;
    const smp_t MAX_LEN=progstate.getSampleRate();
    const smp_t KNOB_NREV=2;
    const smp_t KNOB_NREV_LEN=4;
   
    //background
    Pixmap* back_pix = new Pixmap(pix["128x136"]);
   
    hz = new Const(0);
    cvrt = new ConvertHz2Per(Convert(progstate.getSampleRate()));
    cvrt->setInput((Value**)&hz);
   
    out = new SampleAndHold();
    out->setPeriod((Value**)&cvrt);
   
   
    //add eveything
    add(back_pix,0,0);
    add(new MoveBorder(this, back_pix->pos.w,back_pix->pos.h),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("smp&hold");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
   
    og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
    int xoff=10;
    int yoff=25;
   
    l = new Label(f);
    l->printF("clock");
    add(l,xoff, yoff);
   
    yoff+=15;
    xoff+=15;
    l = new Label(f);
    l->printF("int");
    add(l,xoff, yoff);
   
    c_int=true;
    c_ext=false;
   
    led_int = new LedsLevel<bool>(pix["led_on"],pix["led_off"]);
    led_int->setData(&c_int);
    led_int->setThreshold(0);
    add(led_int, xoff-15, yoff-9);
   
    xoff+=40;
    //yoff+=15;
   
    knob_hz = new KnobSlider(pix["knob16x16"]);
    knob_hz->setMouseMoveAction(new C_UPD(this));
    knob_hz->setContinous(MIN_HZ, MAX_HZ, (MAX_HZ-MIN_HZ)/KNOB_NREV);
    knob_hz->setTurnSpeed(KNOB_TURN_FAST);
    add(knob_hz, xoff, yoff-4);
   
    in_hz = new InputTaker(pix["in"], this, 1);
    addInputTaker(in_hz);
    add(in_hz, xoff+20, yoff-2);
   
    l = new Label(f);
    l->printF("hz");
    add(l,xoff+36, yoff+2);
   
    yoff+=25;
   
    xoff=25;
   
    l = new Label(f);
    l->printF("ext");
    add(l,xoff, yoff);
   
    led_ext = new LedsLevel<bool>(pix["led_on"],pix["led_off"]);
    led_ext->setData(&c_ext);
    led_ext->setThreshold(0);
    add(led_ext, xoff-15, yoff-9);
   
    in_smp = new InputTaker(pix["in"], this, 2);
    addInputTaker(in_smp);
    add(in_smp, xoff+60, yoff-3);
   
    yoff+=30;
   
    xoff=10;
   
    l = new Label(f);
    l->printF("sync");
    add(l,xoff, yoff);
   
    og_clock = new OutputGiver(pix["out"],this, 1);
    addOutputGiver(og_clock);
    add(og_clock, xoff+10, yoff+14);
   
    //yoff+=16;
   
    xoff+=50;
   
    l = new Label(f);
    l->printF("s.len");
    add(l,xoff, yoff);
   
    knob_len = new KnobSlider(pix["knob16x16"]);
    knob_len->setMouseMoveAction(new C_UPD(this));
   
   
   
    knob_len->setContinous(MIN_LEN, MAX_LEN, (MAX_LEN-MIN_LEN)/KNOB_NREV_LEN);
    knob_len->setTurnSpeed(KNOB_TURN_FAST);
    add(knob_len, 5+xoff,yoff+12);
   
    in = new InputTaker(pix["in"], this, 3);
    addInputTaker(in);
    add(in, xoff+25, yoff+15);
   
    convert_sec = new ConvertSec2Len(Convert(progstate.getSampleRate()));
   
    update();
}


void GSampleAndHold::update()
{
    hz->set(knob_hz->getValue());
    out->setClockLen(knob_len->getValue());
}


GSampleAndHold::~GSampleAndHold()
{
    delete hz;
    delete cvrt;
    delete out;
}


Value** GSampleAndHold::getOutput(unsigned int id)
{
    if(id==0)return (Value**)&out;
    return out->getClockOut();
}

void GSampleAndHold::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)this->out->setInput(out);
    else if(fromid==1)cvrt->setInput(out);
    else if(fromid==2)
    {
        this->out->setClockIn(out);
        c_ext=true;
        c_int=false;
    }
    else
    {
        convert_sec->setInput(out);
        this->out->setClockLenIn((Value**)&convert_sec);
    }
   
}

void GSampleAndHold::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)this->out->setInput(NULL);
    else if(fromid==1)cvrt->setInput((Value**)&hz);
    else if(fromid==2)
    {
        this->out->setClockIn(NULL);
        c_ext=false;
        c_int=true;
    }
    else
    {
        convert_sec->setInput(NULL);
        this->out->setClockLenIn(NULL);
    }
}


GSampleAndHold::C_UPD::C_UPD(GSampleAndHold* s)
{
    src = s;
}

void GSampleAndHold::C_UPD::action(Component* co)
{
    src->update();
}

void GSampleAndHold::getState(SoundCompEvent* e)
{
    State s(knob_hz->getValue(), knob_len->getValue());
    e->copyData((char*)&s, sizeof(State));
}

void GSampleAndHold::setState(SoundCompEvent* e)
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
   
    knob_hz->setValue(s->hz);
    knob_len->setValue(s->len);
   
    update();
   
}

