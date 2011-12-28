#include "g_integrate.h"

GIntegrate::GIntegrate(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["128x48"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("integrate");
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
    l->printF("reset");
    add(l, xoff, yoff+2);
   
    in = new InputTaker(pix["in"], this, 1);
    addInputTaker(in);
    add(in, xoff+45, yoff);
   
    sig = new Integrate();
}

GIntegrate::~GIntegrate()
{
    delete sig;
}


Value** GIntegrate::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GIntegrate::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(out);
    if(fromid==1)sig->setResetInput(out);

}

void GIntegrate::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(NULL);
    if(fromid==1)sig->setResetInput(NULL);

} 


/*
  void GIntegrate::getState(SoundCompEvent* e)
  {
  }

  void GIntegrate::setState(SoundCompEvent* e)
  {
  DASSERT(e!=NULL);
  }
*/
