#include "g_minus.h"

GMinus::GMinus(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["110x48"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("minus");
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
    add(in, xoff, yoff);
            
    l = new Label(f);
    l->printF("-");
    add(l, xoff+15, yoff+2);
   
    in = new InputTaker(pix["in"], this, 1);
    addInputTaker(in);
    add(in, xoff+15+8+3, yoff);
   
    l = new Label(f);
    l->printF("defs:0");
    add(l, xoff+45, yoff+2);
   
    sig = new Minus();
   
}

GMinus::~GMinus()
{
    delete sig;
}


Value** GMinus::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GMinus::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput0(out);
    if(fromid==1)sig->setInput1(out);
}

void GMinus::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput0(NULL);
    if(fromid==1)sig->setInput1(NULL);
} 
