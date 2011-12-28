#include "g_midipgm.h"


GMIDIpgm::GMIDIpgm(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    dorec=false;  
   
    Pixmap* back_pix = new Pixmap(pix["128x64"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    og = new OutputGiver(pix["out"], this, 0);
    addOutputGiver(og);
    add(og, 19, 3);
   

    one_shot = new OneShot();


    channel=0;
    ns_chan = new NumberSelect<int>(f, 3,0, &channel);
    ns_chan->setValue(channel);
    add(ns_chan,15,30);
   
    id=0;
    ns_id = new NumberSelect<int>(f, 3,0, &id);
    ns_id->setValue(id);
    add(ns_id,55,30);
   
    Label* l = new Label(fnt["label"]);
    l->printF("M pgm");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);

   
    l = new Label(f);
    l->printF("chan");
    add(l,10,20);
   
   
    l = new Label(f);
    l->printF("id");
    add(l,50,20);
   
    l = new Label(f);
    l->printF("rec");
    add(l,90,20);
      
    rec = new Button(pix["up"],pix["down"]);
    rec->stayPressed(true);
    rec->setAction(new REC(this));
    add(rec, 95,32);
         
}

GMIDIpgm::~GMIDIpgm()
{
    delete one_shot;
}


Value** GMIDIpgm::getOutput(unsigned int id)
{
    return (Value**)&one_shot;
}

int GMIDIpgm::getChan()
{
    return channel;
}

int GMIDIpgm::getId()
{
    return id;
}

void  GMIDIpgm::setChan(int c)
{
    channel=c;
    ns_chan->setValue(channel);
}

void  GMIDIpgm::setId(int c)
{
    id=c;
    ns_id->setValue(id);
}

void GMIDIpgm::setOn()
{
    one_shot->set();
}

bool GMIDIpgm::recIdChan()
{
    bool t=dorec;
    dorec=false;
    rec->setPressed(false);
    return t;
}

GMIDIpgm::REC::REC(GMIDIpgm* src)
{
    this->src=src;
}

void GMIDIpgm::REC::action(Component* c)
{
    src->dorec=src->rec->isPressed();
}


/*******/
/*STATE*/
/*******/

void GMIDIpgm::getState(SoundCompEvent* e)
{
    State s(channel, id, dorec);
   
    e->copyData((char*)&s, sizeof(State));
}

void GMIDIpgm::setState(SoundCompEvent* e)
{  
    if(e->empty())return;
    if(e->getSize()!=sizeof(State))
    {
        DERROR("size missmatch");
        if(e->getSize() < sizeof(State))return;
	
        DERROR("trying to read...");
    }
    const State* s = (const State*)e->getData();

   
    setChan(s->channel);
    setId(s->id);
   
    dorec=s->dorec;
    rec->setPressed(dorec);
}

