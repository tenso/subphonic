#include "g_linear.h"

GLinear::GLinear(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["96x64"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("linear");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //out
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
      
    InputTaker* in;
  
    uint xoff=10;
    uint yoff=25;
   
   
    l = new Label(f);
    l->printF("run");
    add(l, xoff, yoff+2);
   
    in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, xoff+45, yoff);
   
    yoff+=15;
   
    l = new Label(f);
    l->printF("reset");
    add(l, xoff, yoff+2);
   
    in = new InputTaker(pix["in"], this, 1);
    addInputTaker(in);
    add(in, xoff+45, yoff);
   
    sig = new Linear();
    sig->setRetrigMode(Linear::DURING_GATE);
}

GLinear::~GLinear()
{
    delete sig;
}


Value** GLinear::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GLinear::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setRunInput(out);
    if(fromid==1)sig->setResetInput(out);

}

void GLinear::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setRunInput(NULL);
    if(fromid==1)sig->setResetInput(NULL);

} 


/*
  void GLinear::getState(SoundCompEvent* e)
  {
  }

  void GLinear::setState(SoundCompEvent* e)
  {
  DASSERT(e!=NULL);
  }
*/
