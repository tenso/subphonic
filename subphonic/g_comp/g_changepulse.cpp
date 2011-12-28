#include "g_changepulse.h"

GChangePulse::GChangePulse(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["138x48"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("changepulse");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
   
    InputTaker* in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
   
    uint xoff=10;
    uint yoff=25;
   
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, xoff, yoff);
   
    l = new Label(f);
    l->printF("change");
    add(l, xoff+30, yoff+2);
   
    sig = new ChangePulse();
   
}

GChangePulse::~GChangePulse()
{
    delete sig;
}


Value** GChangePulse::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GChangePulse::addInput(Value** out, unsigned int fromid)
{
    sig->setInput(out);
}

void GChangePulse::remInput(Value** out, unsigned int fromid)
{
    sig->setInput(NULL);
} 


/*
  void GChangePulse::getState(SoundCompEvent* e)
  {
  }

  void GChangePulse::setState(SoundCompEvent* e)
  {
  DASSERT(e!=NULL);
  }
*/
