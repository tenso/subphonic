#include "g_notegain.h"

GNoteGain::GNoteGain(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    sig = new NoteTran();
   
    cent=0;
    key=0;
    oct=0;

    v_cent = new SmpPoint(&cent);
    v_key = new SmpPoint(&key);
    v_oct = new SmpPoint(&oct);
   
    sig->setCentOff((Value**)&v_cent);
    sig->setKeyOff((Value**)&v_key);
    sig->setOctOff((Value**)&v_oct);
   
    //add eveything
    Pixmap* back_pix = new Pixmap(pix["170x170"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("notegain");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //in
    InputTaker* it = new InputTaker(pix["in"], this, 0);
    addInputTaker(it);
    add(it, GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);

    //output
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
      
    int xoff=10;
    int yoff=30;
   
    //CENT
      
    l = new Label(f);
    l->printF("cent");
    add(l,xoff,yoff);
   
    it = new InputTaker(pix["in"], this, 1);
    addInputTaker(it);
    add(it,xoff+40, yoff);
   
    ns_cent = new NumberSelect<smp_t>(f, 4, 0, &cent, true);
    add(ns_cent, xoff+40+30, yoff);
   
    yoff+=40;
   
    //KEY
      
    l = new Label(f);
    l->printF("key");
    add(l,xoff,yoff);
   
    it = new InputTaker(pix["in"], this, 2);
    addInputTaker(it);
    add(it,xoff+40, yoff);
   
    ns_key = new NumberSelect<smp_t>(f, 2, 0, &key, true);
    add(ns_key, xoff+40+30, yoff);
   
    yoff+=40;
   
    //OCTAVE
   
    l = new Label(f);
    l->printF("oct");
    add(l,xoff,yoff);
   
    it = new InputTaker(pix["in"], this, 3);
    addInputTaker(it);
    add(it,xoff+40, yoff);
   
    ns_oct = new NumberSelect<smp_t>(f, 2, 0, &oct, true);
    add(ns_oct, xoff+40+30, yoff);
    //ns_oct->setAction(new C_Oct(this));
   
   
    //QUANTIZE
   
    xoff=10;
    yoff=145;
   
    l = new Label(f);
    l->printF("quantize offsets\nto integers");
   
    add(l,xoff, yoff);
   
    b_quantize = new Button(pix["up"], pix["down"]);
    b_quantize->setStayPressed(true);
    b_quantize->setPressed(true); //NoteTran::quantize defaults to true
    b_quantize->setAction(new C_Quantize(this));
    add(b_quantize, xoff+135, yoff);

}

GNoteGain::~GNoteGain()
{
    delete v_cent;
    delete v_key;
    delete v_oct;
    delete sig;
}


Value** GNoteGain::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GNoteGain::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(out);
    else if(fromid==1)sig->setCentOff(out);
    else if(fromid==2)sig->setKeyOff(out);
    else if(fromid==3)sig->setOctOff(out);
   
}

void GNoteGain::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(NULL);
    else if(fromid==1)sig->setCentOff((Value**)&v_cent);
    else if(fromid==2)sig->setKeyOff((Value**)&v_key);
    else if(fromid==3)sig->setOctOff((Value**)&v_oct);
} 


void GNoteGain::getState(SoundCompEvent* e)
{
    State s((int)ns_cent->getValue(), (int)ns_key->getValue(), (int)ns_oct->getValue(), b_quantize->getValue());
    e->copyData((char*)&s, sizeof(State));
}

void GNoteGain::setState(SoundCompEvent* e)
{
    DASSERT(e!=NULL);
    if(e->empty())return;
   
    /*if(e->getSize()!=sizeof(State))
      {
      DERROR("size missmatch");
      if(e->getSize() < sizeof(State))return;
	
      DERROR("trying to read...");
      }*/
   
    const State* s = (const State*)e->getData();
   
    ns_cent->setValue(s->cent);
    ns_key->setValue(s->key);
    ns_oct->setValue(s->oct);
   
    if(progstate.getCurrentLoadVersion()>=14)
    {
        b_quantize->setValue(s->quantize);
        b_quantize->action();
    }
   
}

GNoteGain::C_Quantize::C_Quantize(GNoteGain* gen)
{
    src=gen;
}

void GNoteGain::C_Quantize::action(Component* co)
{
    src->sig->setQuantize(src->b_quantize->getValue());
}
