#include "g_bitcrush.h" 

GBitCrush::GBitCrush(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    sig = new BitCrush();
         
    Pixmap* back_pix = new Pixmap(pix["128x64"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w, back_pix->pos.h), 0,0);
    Label* l = new Label(fnt["label"]);
    l->printF("bitcrush");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
      
    //signal in/out
    add(addInputTaker(new InputTaker(pix["in"], this, 0)), GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
    add(addOutputGiver(new OutputGiver(pix["out"], this, 0)), GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
    int xoff=10;
    int yoff=30;
   
    //MAX LEN
    l = new Label(f);
    l->printF("value bits");
    add(l, xoff, yoff);
   
    xoff+=90;
    yoff-=5;
   
    nbit=32;
    ns_nbit = new NumberSelect<smp_t>(f, 2, 0, &nbit);
    ns_nbit->setRange(1,32);
    ns_nbit->setAction(new C_Update(this));
    ns_nbit->action(); //sets def value
    add(ns_nbit,xoff,yoff);
   
    v_bits = new SmpPoint(&nbit);
    sig->setNBitsIN((Value**)&v_bits);
   
    xoff-=60;
    yoff+=18;
    add(addInputTaker(new InputTaker(pix["in"], this, 1)), xoff, yoff);
}

GBitCrush::~GBitCrush()
{
    delete sig;
    delete v_bits;
}



Value** GBitCrush::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GBitCrush::addInput(Value** in, uint id)
{
    if(id==0)sig->setInput(in);
    else if(id==1)sig->setNBitsIN(in);
}

void GBitCrush::remInput(Value** out, uint id)
{
    if(id==0)sig->setInput(NULL);
    else if(id==1)sig->setNBitsIN((Value**)&v_bits);
}


/*state*/

void GBitCrush::getState(SoundCompEvent* e)
{
    State s(nbit);
    e->copyData((char*)&s, sizeof(State));
}

void GBitCrush::setState(SoundCompEvent* e)
{
    DASSERT(e!=NULL);
    if(e->empty())return;
   
    const State* s = (const State*)e->getData();
   
    ns_nbit->setValue(s->n);
    ns_nbit->action();
}

GBitCrush::C_Update::C_Update(GBitCrush* src)
{
    this->src=src;
}

void GBitCrush::C_Update::action(Component* c)
{
    src->sig->setNBits(src->nbit);
}
