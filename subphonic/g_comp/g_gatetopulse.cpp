#include "g_gatetopulse.h"

GGateToPulse::GGateToPulse(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["168x48"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("gate to pulse");
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
            
    in = new InputTaker(pix["in"], this, 1);
    addInputTaker(in);
    add(in, GCOMP_STDIN_XOFF, yoff);
   
    l = new Label(f);
    l->printF("pulse len, def:1");
    add(l, GCOMP_STDIN_XOFF+20, yoff+2);
   
    sig = new GateToPulse();
   
}

GGateToPulse::~GGateToPulse()
{
    delete sig;
}


Value** GGateToPulse::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GGateToPulse::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(out);
    if(fromid==1)sig->setPulseLenSmp(out);
}

void GGateToPulse::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(NULL);
    if(fromid==1)sig->setPulseLenSmp(NULL);
} 


/*
  void GGateToPulse::getState(SoundCompEvent* e)
  {
  }

  void GGateToPulse::setState(SoundCompEvent* e)
  {
  DASSERT(e!=NULL);
  }
*/
