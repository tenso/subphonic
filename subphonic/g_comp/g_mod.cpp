#include "g_mod.h"

GMod::GMod(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["128x48"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("mod");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //out
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
      
    InputTaker* in;
  
    uint xoff=10;
    uint yoff=25;
   
    //in
    in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, xoff, yoff);
   
    l = new Label(f);
    l->printF("%%");
    add(l, xoff+17, yoff+2);
   
    //mod
    in = new InputTaker(pix["in"], this, 1);
    addInputTaker(in);
    add(in, xoff+30, yoff);
   
    l = new Label(f);
    l->printF("def:1.0");
    add(l, xoff+50, yoff+2);
   
    sig = new Mod();
   
}

GMod::~GMod()
{
    delete sig;
}


Value** GMod::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GMod::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(out);
    if(fromid==1)sig->setMod(out);
}

void GMod::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(NULL);
    if(fromid==1)sig->setMod(NULL);
} 


/*
  void GMod::getState(SoundCompEvent* e)
  {
  }

  void GMod::setState(SoundCompEvent* e)
  {
  DASSERT(e!=NULL);
  }
*/
