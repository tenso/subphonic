#include "g_print.h"

GPrint::GPrint(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    sig = new PrintValue(cout);
   
    Pixmap* back_pix = new Pixmap(pix["128x74"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("print");
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
   
    knob = new KnobSlider(pix["knob16x16"]);
    knob->setMouseMoveAction(new C_Update(this));
    knob->setContinous(1,44100, 11025);
    knob->setValue(4410);
    knob->mouseMoveAction();
   
    add(knob, xoff-15,yoff+15);


}

GPrint::~GPrint()
{
    delete sig;
}


Value** GPrint::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GPrint::addInput(Value** out, unsigned int fromid)
{
    sig->setInput(out);
}

void GPrint::remInput(Value** out, unsigned int fromid)
{
    sig->setInput(NULL);
}


GPrint::C_Update::C_Update(GPrint* src)
{
    this->src=src;
} 

void GPrint::C_Update::action(Component* c)
{
    src->sig->setPeriod((int)src->knob->getValue());
}
