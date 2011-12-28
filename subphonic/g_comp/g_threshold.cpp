#include "g_threshold.h"

GThreshold::GThreshold(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["220x64"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w, back_pix->pos.h), 0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("   threshold");
    add(l,GCOMP_LABEL_XOFF,GCOMP_LABEL_YOFF);
   
    int xoff=10;
    int yoff=30;
   
    OutputGiver* og = new OutputGiver(pix["out"], this,0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
    //on in
   
    l = new Label(f, "on (1)");
    add(l, xoff, yoff);
   
    InputTaker* it =  new InputTaker(pix["in"], this, 0);
    addInputTaker(it);
    add(it, xoff+10, yoff+15);
   
    //off in
   
    xoff=70;
   
    l = new Label(f, "off (0)");
    add(l, xoff, yoff);
   
    it =  new InputTaker(pix["in"], this, 1);
    addInputTaker(it);
    add(it, xoff+35, yoff+15);
   
    xoff=140;
   
    //off in
   
   
    l = new Label(f, "th (0.5)");
    add(l, xoff, yoff);
   
    it =  new InputTaker(pix["in"], this, 2);
    addInputTaker(it);
    add(it, xoff+35, yoff+15);
   
    //in
   
    it =  new InputTaker(pix["in"], this, 3);
    addInputTaker(it);
    add(it, GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);

    th = new Threshold();
   
    LedsLevel<bool>* led = new LedsLevel<bool>(pix["led_on"], pix["led_off"]);
    led->setData(th->isOnPtr());
    add(led, GCOMP_STDOUT_XOFF+20, -6);
   
    //RESET
    /*l = new Label(f, "reset");
      add(l, xoff, yoff);
   
      b_reset = new Button(pix["up"],pix["down"]);
      b_reset->stayPressed(false);
      b_reset->setAction(new C_Reset(this));
      add(b_reset, xoff+45, yoff);*/
   
}


GThreshold::~GThreshold()
{

}

Value** GThreshold::getOutput(unsigned int id)
{
    return (Value**)&th;
}

void GThreshold::addInput(Value** out, unsigned int id)
{
    if(id==0)th->setOnVal(out);
    if(id==1)th->setOffVal(out);
    if(id==2)th->setThreshold(out);
    if(id==3)th->setInput(out);
}

void GThreshold::remInput(Value** out, unsigned int id)
{
    if(id==0)th->setOnVal(NULL);
    if(id==1)th->setOffVal(NULL);
    if(id==2)th->setThreshold(NULL);
    if(id==3)th->setInput(NULL);
}
/*
  GThreshold::C_Reset::C_Reset(GThreshold* src)
  {
  this->src=src;
  } 

  void GThreshold::C_Reset::action(Component* c)
  {
  src->th->reset();
  }
*/
/*state*/
/*
  void GThreshold::getState(SoundCompEvent* e)
  {
  State s(val, knob->getValue(), b_neg->getValue());
   
  e->copyData((char*)&s, sizeof(State));
  }

  void GThreshold::setState(SoundCompEvent* e)
  {
  DASSERT(e!=NULL);
  if(e->empty())return;
  if(e->getSize()!=sizeof(State))
  {
  DERROR("size missmatch");
  if(e->getSize() < sizeof(State))return;
	
  DERROR("trying to read...");
  }
  const State* s = (const State*)e->getData();

  val=s->val;
  ns->setData(val);
   
  knob->setValue(s->knob);
   
  b_neg->setValue(s->neg);
   
  updateGain();
  }
*/
