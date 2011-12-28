#include "g_samplerate.h"

GSampleRate::GSampleRate(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["96x48"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("srate");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //out
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
    sig = new Const(progstate.getSampleRate());
   
    l = new Label(f);
    l->printF("%d", (int)sig->nextValue());
    add(l, 20, 30);
}

GSampleRate::~GSampleRate()
{
    delete sig;
}


Value** GSampleRate::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

/*
  void GSampleRate::getState(SoundCompEvent* e)
  {
  }

  void GSampleRate::setState(SoundCompEvent* e)
  {
  DASSERT(e!=NULL);
  }
*/
