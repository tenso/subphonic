#include "g_octscale.h"

OctScale::OctScale()
{
    noct=4;
    in=NULL;
    level=NULL;
}


void OctScale::setInput(Value** in)
{
    this->in=in;
}


void OctScale::setLevel(Value** in)
{
    this->level=in;
}

void OctScale::setNOct(uint n)
{
    noct=n;
    if(noct==0)noct=1;
}

smp_t OctScale::nextValue()
{
    smp_t val = 0.0;
    if(in!=NULL)val = (*in)->nextValue();
   
    smp_t lvl=1.0;
    if(level!=NULL)lvl = (*level)->nextValue();
   
    if(val<-1.0)val=-1.0;
    if(val>1.0)val=1.0;
   
    return lvl*pow(2.0, val*noct);
}


GOctScale::GOctScale(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["200x84"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("oct scale");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //out
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
    uint xoff=10;
    uint yoff=30;
   
    l = new Label(f);
    l->printF("ctrl");
    add(l, xoff, yoff);
   
    InputTaker* in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, xoff+50, yoff-2);
   
    yoff+=20;
   
    l = new Label(f);
    l->printF("z.lvl");
    add(l, xoff, yoff);
   
    l = new Label(f);
    l->printF("def:1.0");
    add(l, xoff+10, yoff+16);
   
    in = new InputTaker(pix["in"], this, 1);
    addInputTaker(in);
    add(in, xoff+50, yoff-2);
   
    xoff=90;
    yoff=25;
   
    l = new Label(f);
    l->printF("max.oct\n  +/-");
    add(l, xoff, yoff+5);
   
    noct=4;
    ns = new NumberSelect<uint>(f, 2, 0, &noct);
    ns->setMin(1);
    ns->setMax(25);
    add(ns, xoff+60, yoff);
    ns->setAction(new C_Update(this));
   
    sig = new OctScale();
    sig->setNOct(ns->getValue());
}

GOctScale::~GOctScale()
{
    delete sig;
}


Value** GOctScale::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GOctScale::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(out);
    else sig->setLevel(out);
}

void GOctScale::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(NULL);
    else sig->setLevel(NULL);
} 

void GOctScale::update()
{
    sig->setNOct(ns->getValue());
}

void GOctScale::getState(SoundCompEvent* e)
{
    State s(ns->getValue());
    e->copyData((char*)&s, sizeof(State));
}

void GOctScale::setState(SoundCompEvent* e)
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
   
    ns->setValue(s->noct);
   
    update();
}

/*callbacks*/

GOctScale::C_Update::C_Update(GOctScale* s)
{
    src=s;
}

void GOctScale::C_Update::action(Component* co)
{

    src->update();
}
