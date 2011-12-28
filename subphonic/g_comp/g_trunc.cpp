#include "g_trunc.h"

GTrunc::GTrunc(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["96x48"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("trunc");
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
   
    /*l = new Label(f);
      l->printF("-1 , 1");
      add(l, xoff, yoff+2);*/
   
    sig = new Trunc();
   
}

GTrunc::~GTrunc()
{
    delete sig;
}


Value** GTrunc::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GTrunc::addInput(Value** out, unsigned int fromid)
{
    sig->setInput(out);

}

void GTrunc::remInput(Value** out, unsigned int fromid)
{
    sig->setInput(NULL);
} 


/*
  void GTrunc::getState(SoundCompEvent* e)
  {
  }

  void GTrunc::setState(SoundCompEvent* e)
  {
  DASSERT(e!=NULL);
  }
*/
