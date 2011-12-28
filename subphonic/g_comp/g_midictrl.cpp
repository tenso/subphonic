#include "g_midictrl.h"


GMIDIctrl::GMIDIctrl(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    value=0;
    sig = new SmpPoint(&value);
   
    sig_gain = new Gain();
    sig_gain->setInput((Value**)&sig);
     
    dorec=false;  
   
    Pixmap* back_pix = new Pixmap(pix["164x128"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("M ctrl");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    og = new OutputGiver(pix["out"], this, 0);
    addOutputGiver(og);
    add(og, 19, 3);
   
    //change shot
    one_shot = new OneShot();
    og_one = new OutputGiver(pix["out"], this, 1);
    addOutputGiver(og_one);
    add(og_one, 100, 105);
   
   
    l = new Label(f);
    l->printF("chng pulse");
    add(l,10,107);
   
    channel=0;
    ns_chan = new NumberSelect<int>(f, 3,0, &channel);
    ns_chan->setValue(channel);
    add(ns_chan,15,30);
   
    controller=1;
    ns_ctrl = new NumberSelect<int>(f, 3,0, &controller);
    ns_ctrl->setValue(controller);
    add(ns_ctrl,55,30);
   

   
    l = new Label(f);
    l->printF("chan");
    add(l,10,20);
   
   
    l = new Label(f);
    l->printF("ctrl");
    add(l,50,20);
   
    l = new Label(f);
    l->printF("rec");
    add(l,90,20);
      
    rec = new Button(pix["up"],pix["down"]);
    rec->stayPressed(true);
    rec->setAction(new REC(this));
    add(rec, 95,32);
   
    //add value sel
    int yoff=-12;
    ns_val=1.0;
    ns_gain = new NumberSelect<smp_t>(f, NUM_WHOLE, NUM_FRAC, &ns_val);
    ns_gain->setAction(new C_GAIN(this));
    add(ns_gain,40,yoff+85);
   
    gain = new KnobSlider(pix["knob16x16"]);
    gain->setMouseMoveAction(new C_GAIN(this));
    add(gain, 10,yoff+90);
    gain->setValue(1.0);
   
    val_l = new Label(f);
    add(val_l,20,60);
   
    updateGain();
}

GMIDIctrl::~GMIDIctrl()
{
    delete one_shot;
    delete sig;
    delete sig_gain;
}


void GMIDIctrl::updateGain()
{
    sig_gain->setGain(gain->getValue()*ns_gain->getValue());
   
    stringstream out;
    out << "out: %." << NUM_FRAC << "f";
    val_l->printF(out.str().c_str(), sig_gain->nextValue());
}


Value** GMIDIctrl::getOutput(unsigned int id)
{
    if(id==0)return (Value**)&sig_gain;
    return (Value**)&one_shot;
}

int GMIDIctrl::getChan()
{
    return channel;
}

int GMIDIctrl::getCtrl()
{
    return controller;
}

void  GMIDIctrl::setChan(int c)
{
    channel=c;
    ns_chan->setValue(channel);
}

void  GMIDIctrl::setCtrl(int c)
{
    controller=c;
    ns_ctrl->setValue(controller);
}

void GMIDIctrl::setValue(smp_t v)
{
    one_shot->set();
   
    value=v;
    updateGain();
}

bool GMIDIctrl::recCtrlChan()
{
    bool t=dorec;
    dorec=false;
    rec->setPressed(false);
    return t;
}

GMIDIctrl::REC::REC(GMIDIctrl* src)
{
    this->src=src;
}

void GMIDIctrl::REC::action(Component* c)
{
   
    src->dorec=src->rec->isPressed();
      
}


GMIDIctrl::C_GAIN::C_GAIN(GMIDIctrl* src)
{
    this->src=src;
}

void GMIDIctrl::C_GAIN::action(Component* c)
{
    src->updateGain();
}

/*******/
/*STATE*/
/*******/

void GMIDIctrl::getState(SoundCompEvent* e)
{
    State s(channel, controller, value, dorec, gain->getValue(), ns_gain->getValue());
   
    e->copyData((char*)&s, sizeof(State));
   
}

void GMIDIctrl::setState(SoundCompEvent* e)
{  
    if(e->empty())return;
   
    if(e->getSize()!=sizeof(State))
    {
        DERROR("size missmatch");
        if(e->getSize() < sizeof(State))return;
	
        DERROR("trying to read...");
    }
   
    const State* s = (const State*)e->getData();
   
   
    setChan(s->channel);
    setCtrl(s->controller);
   
    dorec=s->dorec;
    rec->setPressed(dorec);
   
    gain->setValue(s->gain);
    sig_gain->setGain(gain->getValue());
   
    setValue(s->value);
   
    ns_gain->setValue(s->nsgain);
   
    updateGain();
}

