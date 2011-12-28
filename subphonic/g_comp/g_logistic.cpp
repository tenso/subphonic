#include "g_logistic.h"

GLogisticMap::GLogisticMap(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["220x64"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w, back_pix->pos.h), 0,0);
   
    Label* l = new Label(fnt["label"], "logistic map");
    add(l,GCOMP_LABEL_XOFF,GCOMP_LABEL_YOFF);
   
    l = new Label(f, "x(n+1) = r*x(n)*(1-x(n))");
    add(l, 10, 20);
   
    int xoff=10;
    int yoff=40;
   
    OutputGiver* og = new OutputGiver(pix["out"], this,0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
    //R in
   
    l = new Label(f, "r");
    add(l, xoff, yoff);
   
    InputTaker* it =  new InputTaker(pix["in"], this, 0);
    addInputTaker(it);
    add(it, xoff+10, yoff);
   
    //seed in
   
    xoff=50;
   
    l = new Label(f, "x(0)");
    add(l, xoff, yoff);
   
    it =  new InputTaker(pix["in"], this, 1);
    addInputTaker(it);
    add(it, xoff+35, yoff);
   
    xoff=110;
   
    //RESET
    l = new Label(f, "reset");
    add(l, xoff, yoff);
   
    b_reset = new Button(pix["up"],pix["down"]);
    b_reset->stayPressed(false);
    b_reset->setAction(new C_Reset(this));
    add(b_reset, xoff+45, yoff);
   
   
    lmap = new LogisticMap();
    lmap->setContSeed(true);
}


GLogisticMap::~GLogisticMap()
{

}

Value** GLogisticMap::getOutput(unsigned int id)
{
    return (Value**)&lmap;
}

void GLogisticMap::addInput(Value** out, unsigned int id)
{
    if(id==0)lmap->setR(out);
    if(id==1)lmap->setSeed(out);
}

void GLogisticMap::remInput(Value** out, unsigned int id)
{
    if(id==0)lmap->setR(NULL);
    if(id==1)lmap->setSeed(NULL);
}

GLogisticMap::C_Reset::C_Reset(GLogisticMap* src)
{
    this->src=src;
} 

void GLogisticMap::C_Reset::action(Component* c)
{
    src->lmap->reset();
}

/*state*/
/*
  void GLogisticMap::getState(SoundCompEvent* e)
  {
  State s(val, knob->getValue(), b_neg->getValue());
   
  e->copyData((char*)&s, sizeof(State));
  }

  void GLogisticMap::setState(SoundCompEvent* e)
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
