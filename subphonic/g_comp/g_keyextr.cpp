#include "g_keyextr.h"

GKeyExtr::GKeyExtr(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["138x48"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("key extract");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //out
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
      
    InputTaker* in;
  
    uint xoff=10;
    uint yoff=25;
   
    in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
   
    l = new Label(f);
    l->printF("440hz -> 49");
    add(l, xoff, yoff);
   
    sig = new KeyExtract();
   
}

GKeyExtr::~GKeyExtr()
{
    delete sig;
}


Value** GKeyExtr::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GKeyExtr::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(out);
}

void GKeyExtr::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(NULL);

} 


/*
  void GKeyExtr::getState(SoundCompEvent* e)
  {
  }

  void GKeyExtr::setState(SoundCompEvent* e)
  {
  DASSERT(e!=NULL);
  }
*/
