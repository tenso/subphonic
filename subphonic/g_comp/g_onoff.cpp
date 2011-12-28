#include "g_onoff.h"

GOnOff::GOnOff(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    sig = new OnOff();
    v_onoff = new Const(0);
    sig->setOnOff((Value**)&v_onoff);
   
    Pixmap* back_pix = new Pixmap(pix["128x64"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("onoff");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
   
   
    //in/out
    InputTaker* it = new InputTaker(pix["in"], this, 0);
    addInputTaker(it);
    add(it, GCOMP_STDIN_XOFF,GCOMP_STDIN_YOFF);
      
    OutputGiver* og = new OutputGiver(pix["out"], this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF,GCOMP_STDOUT_YOFF);
   
   
    //button
    int xoff=10;
    int yoff=25;
   
    l = new Label(f);
    l->printF("on/off");
    add(l, xoff, yoff);
   
    yoff+=10;
    xoff+=5;
    b_onoff = new Button(pix["up"],pix["down"]);
    b_onoff->setValue(0);
    b_onoff->stayPressed(true);
    b_onoff->setAction(new C_OnOff(this));
    add(b_onoff, xoff,yoff);
    xoff+=25;
    it = new InputTaker(pix["in"], this, 1);
    addInputTaker(it);
    add(it, xoff, yoff);
   
   
}

GOnOff::~GOnOff()
{
    delete sig;
    delete v_onoff;
}


Value** GOnOff::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GOnOff::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(out);
    if(fromid==1)sig->setOnOff(out);
}

void GOnOff::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(NULL);
    if(fromid==1)sig->setOnOff((Value**)&v_onoff);
}



void GOnOff::getState(SoundCompEvent* e)
{
    State s(b_onoff->getValue());
    e->copyData((char*)&s, sizeof(State));
}

void GOnOff::setState(SoundCompEvent* e)
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
   
   
    b_onoff->setValue(s->mode);
    v_onoff->setValue(b_onoff->getValue());
}




GOnOff::C_OnOff::C_OnOff(GOnOff* s)
{
    src=s;
}

void GOnOff::C_OnOff::action(Component* c)
{
    src->v_onoff->setValue(src->b_onoff->getValue());
}
