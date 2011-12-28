#include "g_abs.h"

GAbs::GAbs(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    sig = new Rectify();
   
    Pixmap* back_pix = new Pixmap(pix["112x38"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
    Label* l = new Label(fnt["label"]);
    l->printF("abs");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    uint xoff=10;
    uint yoff=20;
    add(addInputTaker(new InputTaker(pix["in"], this)), xoff, yoff);
   
    xoff+=20;

    l = new Label(f);
    l->printF("Rf   Ct");
    add(l,xoff,yoff+2);
   
    xoff+=17;
    b_mode = new Button(pix["bs_left"],pix["bs_right"]);
    b_mode->stayPressed(true);
    b_mode->setAction(new C_Mode(this));
    add(b_mode, xoff,yoff-2);
   
    xoff+=50;
   
    add(addOutputGiver(new OutputGiver(pix["out"], this)), xoff-4, yoff);
}

GAbs::~GAbs()
{
    delete sig;
}


Value** GAbs::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GAbs::addInput(Value** out, unsigned int fromid)
{
    sig->setInput(out);
}

void GAbs::remInput(Value** out, unsigned int fromid)
{
    sig->setInput(NULL);
}

void GAbs::getState(SoundCompEvent* e)
{
    State s(b_mode->getValue());
    e->copyData((char*)&s, sizeof(State));
}

void GAbs::setState(SoundCompEvent* e)
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
   
   
    b_mode->setValue(s->mode);
    sig->setHalf(b_mode->getValue());
}

GAbs::C_Mode::C_Mode(GAbs* src)
{
    this->src=src;
}

void GAbs::C_Mode::action(Component* c)
{
    src->sig->setHalf(src->b_mode->getValue());
}
