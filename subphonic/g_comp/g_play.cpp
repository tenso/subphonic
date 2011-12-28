#include "g_play.h"

GPlay::GPlay(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    mix=true;
    aplay=true;
    on = true;
    sig = new Stereo();
    sig->distFirstCh(mix);
      

    Pixmap* back_pix = new Pixmap(pix["100x190"]);
    add(back_pix,0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("speaker");
    add(l,10, GCOMP_LABEL_YOFF);
   
    int dx=50;
    int xoff=10;
    int yoff=20;
    l = new Label(f);
    l->printF("L &\nmono");
    add(l,xoff,yoff);
    l = new Label(f);
    l->printF("R");
    add(l,xoff+dx,yoff); 
   
   
    //uv meters
    xoff=10;
    yoff+=15;
   
    for(uint i=0;i<2;i++)
    {
        meters[i] = new PeakMeter(); 
        meters[i]->setFalloffRate(0.25/11025.0);
        meters[i]->setPeakHold(11025*2);
	
        meter_leds[i] = new LedsLevel<smp_t>(pix["led_on"], pix["led_off"], 10, true);
	
        for(uint j=5;j<10;j++)
        {
            if(j<8)meter_leds[i]->setPixmap(j, pix["ledg_on"], pix["ledg_off"]);
            else meter_leds[i]->setPixmap(j, pix["ledr_on"], pix["ledr_off"]);
        }
	
        meter_leds[i]->setData(meters[i]->peakPtr());
        add(meter_leds[i], xoff, yoff);
	
        sig->addInput((Value**)&meters[i], i);
	
        xoff+=dx;
    }
   
    xoff=10;
    yoff=150;
    //inputs
    in[0] = new InputTaker(pix["in"], this,0);
    in[1] = new InputTaker(pix["in"], this,1);
    addInputTaker(in, 2);
    add(in[0],xoff, yoff);
    add(in[1],xoff+dx, yoff);
   
    xoff=10;
    yoff+=20;
   
    l = new Label(f);
    l->printF("stereo");
    add(l,xoff, yoff);
   
    rb = new Button(pix["up"],pix["down"]);
    rb->stayPressed(true);
    rb->setAction(new C_STEREO(this));
    add(rb,xoff+50,yoff-4);
   
}

GPlay::~GPlay()
{
    for(uint i=0;i<2;i++)
    {
        delete meters[i];
    }
   
    delete sig;
}


void GPlay::setNChan(int chans)
{
    sig->setNChan(chans);
}

bool GPlay::isOn()
{
    return on;
}

void GPlay::addInput(Value** out, unsigned int fromid)
{
    meters[fromid]->setInput(out);
   
}

void GPlay::remInput(Value** out, unsigned int fromid)
{
    meters[fromid]->setInput(NULL);
}

Value** GPlay::getOutput()
{
    return (Value**)&sig;
}

GPlay::C_STEREO::C_STEREO(GPlay* s)
{
    src=s;
}

void GPlay::C_STEREO::action(Component* co)
{
    src->mix=!src->mix;
    src->sig->distFirstCh(src->mix);
    if(src->mix)
    {
        //clear leds so that they dont stay on
        src->meters[1]->reset();
    }
}


void GPlay::getState(SoundCompEvent* e)
{
    State s(rb->getValue());
   
    e->copyData((char*)&s, sizeof(State));
}

void GPlay::setState(SoundCompEvent* e)
{
    if(e->empty())return;
   
    if(e->getSize()!=sizeof(State))
    {
        DERROR("size missmatch");
        if(e->getSize() < sizeof(State))return;
	
        DERROR("trying to read...");
    }
   
    const State* s = (const State*)e->getData();
      
    rb->setValue(s->stereo);
   
    mix=!rb->getValue();
    sig->distFirstCh(mix);
}
