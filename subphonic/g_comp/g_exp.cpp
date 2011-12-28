#include "g_exp.h"

GExp::GExp(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["128x64"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("exponent");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //out
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
      
    InputTaker* in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
  

    uint xoff=10;
    uint yoff=30;
   
    l = new Label(f);
    l->printF("base");
    add(l, xoff, yoff+2);
   
    xoff+=50;
   
    in = new InputTaker(pix["in"], this, 1);
    addInputTaker(in);
    add(in, xoff-8, yoff);
   
    xoff+=10;
   
    l = new Label(f);
    l->printF("2");
    add(l, xoff, yoff+2);
   
    xoff+=10;
   
    b_base = new Button(pix["bs_left"], pix["bs_right"]);
    b_base->setPressed(false);
    b_base->setStayPressed(true);
    b_base->setAction(new C_Update(this));
    add(b_base, xoff, yoff);
   
    xoff+=30;
   
    l = new Label(f);
    l->printF("e");
    add(l, xoff, yoff+2);
   
   
   
   
    sig = new Exponent();
   
    update();
}

GExp::~GExp()
{
    delete sig;
}


Value** GExp::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GExp::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setExponent(out);
    if(fromid==1)sig->setBaseIn(out);
}

void GExp::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setExponent(NULL);
    if(fromid==1)sig->setBaseIn(NULL);
} 

void GExp::update()
{
    if(!b_base->getValue())sig->setBase(2);
    else sig->setBase(M_E);
}

void GExp::getState(SoundCompEvent* e)
{
    State s(b_base->getValue());
    e->copyData((char*)&s, sizeof(State));
}

void GExp::setState(SoundCompEvent* e)
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
   
    b_base->setValue(s->base_no>0);
   
    update();
}

/*callbacks*/

GExp::C_Update::C_Update(GExp* s)
{
    src=s;
}

void GExp::C_Update::action(Component* co)
{
    src->update();
}
