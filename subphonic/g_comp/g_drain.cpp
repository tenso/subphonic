#include "g_drain.h"

GDrain::GDrain(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    dop = new ValueDoppler(1);
   
    Pixmap* back_pix = new Pixmap(pix["128x64"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("drain");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    int xoff=10;
    int yoff=30;
   
    l = new Label(f);
    l->printF("in");
    add(l, xoff, yoff);
   
    InputTaker* it = new InputTaker(pix["in"], this, 0);
    addInputTaker(it);
    add(it, xoff+4, yoff+15);
   
    xoff+=50;
   
    l = new Label(f);
    l->printF("through");
    add(l, xoff, yoff);
   
    OutputGiver* og = new OutputGiver(pix["out"], this, 0);
    addOutputGiver(og);
    add(og, xoff+26, yoff+15);
   
    ground.addInput((Value**)&dop);
}

GDrain::~GDrain()
{
    ground.remInput((Value**)&dop);
    delete dop;
}


Value** GDrain::getOutput(unsigned int id)
{
    return /*(Value**)&*/dop->getSlave();
}

void GDrain::addInput(Value** out, unsigned int fromid)
{
    dop->setInput(out);
}

void GDrain::remInput(Value** out, unsigned int fromid)
{
    dop->setInput(NULL);
}

