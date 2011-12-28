#include "g_slew.h" 

GSlew::GSlew(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
   
    rt_v = new Const(0);
    ft_v = new Const(0);
   
    slew_t = new SlewLimitTime();
    slew_t->setTimeMult(progstate.getSampleRate());
    slew_t->setMode(SlewLimitTime::SINE);
    slew_t->setRiseTime((Value**)&rt_v);
    slew_t->setFallTime((Value**)&ft_v);
   
   
    Pixmap* back_pix = new Pixmap(pix["128x112"]);
    add(back_pix);
    add(new MoveBorder(this, back_pix->pos.w,20),0,0);
   
    //IN
    InputTaker* in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in,GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
   
    //OUT
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
    Label* l = new Label(fnt["label"]);
    l->printF("slew lim.");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //RISE
    int xoff=10;
    int yoff=24;
   
    /*l = new Label(f);
      l->printF("rise time");
      add(l, xoff, yoff);*/
   
    Pixmap* pixmap = new Pixmap(pix["slew_mark_r"]);
    add(pixmap, xoff, yoff+3);
   
    in = new InputTaker(pix["in"], this, 1);
    addInputTaker(in);
    add(in, xoff+26, yoff+14);
   
    rt_knob = new KnobSlider(pix["knob16x16"]);
    rt_knob->setMouseMoveAction(new C_UPDATE(this));
    rt_knob->setContinous(0, KNOB_MAX, KNOB_MAX/KNOB_NREVS);
    rt_knob->setTurnSpeed(KNOB_TURN_FAST);
    rt_knob->setValue(0.1); 
    add(rt_knob,xoff+4,yoff+12);
   
    //FALL
    yoff+=40;
   
    /*l = new Label(f);
      l->printF("fall time");
      add(l, xoff, yoff);*/
   
    pixmap = new Pixmap(pix["slew_mark_f"]);
    add(pixmap, xoff, yoff+3);
   
   
    in = new InputTaker(pix["in"], this, 2);
    addInputTaker(in);
    add(in, xoff+26, yoff+14);
   
    ft_knob = new KnobSlider(pix["knob16x16"]);
    ft_knob->setMouseMoveAction(new C_UPDATE(this));
    ft_knob->setContinous(0, KNOB_MAX, KNOB_MAX/KNOB_NREVS);
    ft_knob->setTurnSpeed(KNOB_TURN_FAST);
    ft_knob->setValue(0.1); 
    add(ft_knob, xoff+4, yoff+12);
   
    //mode
    xoff=65;
    yoff=28;
   
/*   l = new Label(f);
     l->printF("mode");
     add(l, xoff, yoff-20);*/
   
    /*l = new Label(f);
      l->printF("lin");
      add(l, xoff, yoff);*/
      
    pixmap = new Pixmap(pix["slew_lin"]);
    add(pixmap, xoff, yoff);
   
   
    /*l = new Label(f);
      l->printF("sin");
      add(l, xoff, yoff+34);*/
   
    slew_mode = new Button(pix["bs_up"],pix["bs_down"]);
    slew_mode->stayPressed(true);
    slew_mode->setPressed(false);
    slew_mode->setAction(new C_UPDATE(this));
    add(slew_mode, xoff+5, yoff+25);
   
    pixmap = new Pixmap(pix["slew_sin"]);
    add(pixmap, xoff, yoff+46);
   
    /**/
    update();
}

GSlew::~GSlew()
{
    delete rt_v;
    delete ft_v;
    delete slew_t;
}


Value** GSlew::getOutput(unsigned int id)
{
    if(id==0)return (Value**)&slew_t;
    //else return (Value**)&env;
    return NULL;
}

void GSlew::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)slew_t->setInput(out);
    if(fromid==1)slew_t->setRiseTime(out);
    if(fromid==2)slew_t->setFallTime(out);
}

void GSlew::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)slew_t->setInput(NULL);
    if(fromid==1)slew_t->setRiseTime((Value**)&rt_v);
    if(fromid==2)slew_t->setFallTime((Value**)&ft_v);
}

void GSlew::update()
{
    rt_v->set(rt_knob->getValue());
    ft_v->set(ft_knob->getValue());
   
    if(slew_mode->isPressed())
    {
        slew_t->setMode(SlewLimitTime::SINE);
    }
    else
    {
        slew_t->setMode(SlewLimitTime::LINEAR);
    }
   
}


/*state*/

void GSlew::getState(SoundCompEvent* e)
{
    State s(rt_knob->getValue(), ft_knob->getValue(), slew_mode->getValue());
    e->copyData((char*)&s, sizeof(State));
}

void GSlew::setState(SoundCompEvent* e)
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
      
    rt_knob->setValue(s->rt);
    ft_knob->setValue(s->ft);
   
    slew_mode->setValue(s->mode);
   
    update();
}



/*callbacks*/

GSlew::C_UPDATE::C_UPDATE(GSlew* src)
{
    this->src=src;
}

void GSlew::C_UPDATE::action(Component* c)
{
    src->update();
}
  
/*
  GSlew::PMODE::PMODE(GSlew* src)
  {
  this->src=src;
  }

  void GSlew::PMODE::action(Component* c)
  {
  Button* s = (Button*)c;
   
  if(s->isPressed())
  {
  src->sig->setMode(Envelope::Envelope_RMS);
  }
  else
  {
  src->sig->setMode(Envelope::Envelope_Peak);
  }
   
  }
*/
