#include "g_gain.h"

GGain::GGain(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    sgn=1;
    inboost=100;
    gain = new Gain(0);  
    gain->setInput(&empty); 
    gain->setGain(1.0*GAIN*sgn*inboost);
   
   
    //add eveything
    Pixmap* back_pix = new Pixmap(pix["96x96"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("gain");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //inputs
    InputTaker* it = new InputTaker(pix["in"], this, 0);
    addInputTaker(it);
    add(it,5, 3);
   
    it = new InputTaker(pix["in"], this, 1);
    addInputTaker(it);
    add(it, 5, 24);
   
    //output
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, 19, 3);
   
    //amp slider
    amp_slide = new Slider(pix["slide_knob"],pix["slide_under"]);
    amp_slide->setMouseMoveAction(new C_Amp(this));
    amp_slide->setValue(1.0);
    add(amp_slide,16,44);
   
    pmode1 = new Button(pix["bs_up"],pix["bs_down"]);
    pmode1->stayPressed(true);
    pmode1->setAction(new FLIP(this));
    add(pmode1,70,18);
   
    l = new Label(f);
    l->printF("     %%");
    add(l,8,72);
   
    l = new Label(f);
    l->printF("0         1");
    add(l,4,48);
   
    l = new Label(f);
    l->printF("inv ");
    add(l,46,28);
   
    ns = new NumberSelect<smp_t>(f, 4, 0, &inboost);
   
    add(ns, 60, 64); //x=30?
    ns->setAction(new Update(this));
   
}

GGain::~GGain()
{
    delete gain;
}


Value** GGain::getOutput(unsigned int id)
{
    return (Value**)&gain;
}

void GGain::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)gain->setInput(out);
    else 
    {
        gain->setGainIn(out);
    }
   
}

void GGain::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)gain->setInput(&empty);
    else gain->setGainIn(NULL); 
} 

void GGain::updateGain()
{
    gain->setGain(amp_slide->getValue()*GAIN*sgn*inboost);	
}

void GGain::getState(SoundCompEvent* e)
{
    State s(amp_slide->getValue(), inboost, pmode1->isPressed());
    e->copyData((char*)&s, sizeof(State));
}

void GGain::setState(SoundCompEvent* e)
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
   
    amp_slide->setValue(s->slider);
   
    pmode1->setPressed(s->button);
   
    if(s->button)sgn=-1;
    else sgn=1;
   
    inboost=s->iboost;
    ns->setValue(inboost);
   
    updateGain();
}

/*callbacks*/

GGain::C_Amp::C_Amp(GGain* gen)
{
    src = gen;
}

void GGain::C_Amp::action(Component* co) 
{
    src->updateGain();
}

GGain::FLIP::FLIP(GGain* src)
{
    this->src=src;
}

void GGain::FLIP::action(Component* co)
{
    src->sgn=-src->sgn;
   
    //update gain
    src->updateGain();
   
}

GGain::Update::Update(GGain* s)
{
    src=s;
}

void GGain::Update::action(Component* co)
{
    //run after numberselect
    src->updateGain();
}
