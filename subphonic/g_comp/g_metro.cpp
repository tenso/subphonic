#include "g_metro.h" 

GMetro::GMetro(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    num=0;
   
    sig = new OscPulse();
   
    c = new SmpPoint(&num);
    ck = new Const(1/60.0); //divide by 60 to get bpm
   
    m = new Gain();
    m->setInput((Value**)&c);
    m->setGainIn((Value**)&ck);
   
    v = new ConvertHz2Per(Convert(progstate.getSampleRate()), (Value**)&m);
    sig->setPeriod((Value**)&v );
      
    a = new Const(1.0);
    sig->setAmp((Value**)&a);
   
    shp=new Const(0);
    sig->setShape((Value**)&shp);
   
    sig->reset(1); //WARNING: this suppresses first pulse but alsa changes period by -1 for first
      
    Pixmap* back_pix = new Pixmap(pix["112x110"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w, back_pix->pos.h), 0,0);
    Label* l = new Label(fnt["label"]);
    l->printF("bpm");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    add(addOutputGiver(new OutputGiver(pix["out"], this, 0)), 19, 3);
   
    ns = new NumberSelect<smp_t>(f, 4, 3, &num);
    add(ns,16,22);
   
    add(addInputTaker(new InputTaker(pix["in"], this, 1)), 85, 30);
   
    l = new Label(f);
    l->printF("sync");
    add(l, 10, 54);
    add(addInputTaker(new InputTaker(pix["in"], this, 0)), 50, 53);
   
    int xoff=10;
    int yoff=80;
   
   
   
    l = new Label(f);
    l->printF("skip per");
    add(l, xoff, yoff);
   
    suppressed=1;
   
    n_sup = new NumberSelect<uint>(f, 2, 0, &suppressed);
    n_sup->setAction(new C_UpdateSup(this));
    n_sup->action();
    add(n_sup, xoff+70, yoff-9);
   
   

}

GMetro::~GMetro()
{
    delete m;
    delete a;
    delete c;
    delete ck;
    delete v;
    delete shp;
    delete sig;
}


int GMetro::syncToIndex(unsigned int index)
{
    sig->reset(index);
    return 0;
}


Value** GMetro::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GMetro::addInput(Value** in, uint id)
{
    if(id==0)sig->setSyncIn(in);
    else m->setInput(in);
}

void GMetro::remInput(Value** out, uint id)
{
    if(id==0)sig->setSyncIn(NULL);
    else m->setInput((Value**)&c);
}


/*state*/

void GMetro::getState(SoundCompEvent* e)
{
    State s(num, suppressed); 
    e->copyData((char*)&s, sizeof(State));
}

void GMetro::setState(SoundCompEvent* e)
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
   
    ns->setValue(s->num);
   
    if(progstate.getCurrentLoadVersion()>=11)
    {
        n_sup->setValue(s->suppressed);
        n_sup->action();
    }
}

GMetro::C_UpdateSup::C_UpdateSup(GMetro* src)
{
    this->src=src;
}

void GMetro::C_UpdateSup::action(Component* c)
{
    src->sig->setSuppressedPeriods(src->suppressed);
}
