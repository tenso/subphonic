#include "g_crossfade.h"

GCrossfade::GCrossfade(BitmapFont* f, GroundMixer& g) : SoundComp(g) 
{
    cross = new Crossfade();
      
    //add eveything
    Pixmap* back_pix = new Pixmap(pix["128x124"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
      
    Label* l = new Label(fnt["label"]);
    l->printF("crossfade");
    add(l,10,GCOMP_LABEL_YOFF);
   
    //input
    InputTaker* intak[3];
    intak[0] = new InputTaker(pix["in"], this, 0);
    intak[1] = new InputTaker(pix["in"], this, 1); 
    intak[2] = new InputTaker(pix["in"], this, 2); 
    addInputTaker(intak,3);
   
      
    int xoff=10;
    int yoff=20;
   
    l = new Label(f);
    l->printF("1:");
    add(l,xoff,yoff+2);
   
    l = new Label(f);
    l->printF("2:");
    add(l,xoff,yoff+15+2);

   
    add(intak[0],xoff+20, yoff);
    add(intak[1],xoff+20, yoff+15);
   
    l = new Label(f);
    l->printF("<-mono");
    add(l,xoff+2+20+40,yoff);
   
    //output
    OutputGiver* outg[2];
    outg[0] = new OutputGiver(pix["out"], this, 0);
    outg[1] = new OutputGiver(pix["out"], this, 1);
    addOutputGiver(outg,2);
   
    xoff+=40;
   
    add(outg[0],xoff, yoff);
    add(outg[1],xoff, yoff+15);
   
    xoff=25;
    yoff+=45;
   
    c_slide = new Slider(pix["slide_knob"],pix["slide_under"]);
    c_slide->setMouseMoveAction(new C_CROSS(this));
    c_slide->setValue(0.5);
    add(c_slide, xoff-10,yoff-5);
   
    add(intak[2],xoff+17, yoff+15);
   
    l = new Label(f);
    l->printF("1");
    add(l,xoff-20,yoff);
   
    l = new Label(f);
    l->printF("2");
    add(l,xoff+58,yoff);
   
    xoff=5;
    yoff+=40;
    l = new Label(f);
    l->printF("stereo");
    add(l,xoff+10,yoff);
   
    stereo = new Button(pix["up"], pix["down"]);
    stereo->stayPressed(true);
    stereo->setAction(new C_STEREO(this));
    add(stereo, xoff+65, yoff-5);
   
   
    f_val = new Const(c_slide->getValue());
    cross->setFade((Value**)&f_val);
   
    ground.addInput((Value**)&cross);
}

GCrossfade::~GCrossfade()
{
    ground.remInput((Value**)&cross);
   
    delete cross;
    delete f_val;
}


Value** GCrossfade::getOutput(unsigned int id)
{
    return cross->getOutput(id);
}

void GCrossfade::addInput(Value** out, unsigned int fromid)
{
    if(fromid<=1)
    {
        cross->setInput(out, fromid);
    }
    if(fromid==2)
    {
        cross->setFade(out);
    }
}

void GCrossfade::remInput(Value** out, unsigned int fromid)
{
    if(fromid<=1)
    {
        cross->setInput(NULL, fromid);
    }
    if(fromid==2)cross->setFade((Value**)&f_val);
}


GCrossfade::C_CROSS::C_CROSS(GCrossfade* d)
{
    src=d;
}

void GCrossfade::C_CROSS::action(Component* co)
{
    src->f_val->set(src->c_slide->getValue());
}


GCrossfade::C_STEREO::C_STEREO(GCrossfade* d)
{
    src=d;
}

void GCrossfade::C_STEREO::action(Component* co)
{
    src->cross->setMix(!src->stereo->getValue());
}

/*state*/

void GCrossfade::getState(SoundCompEvent* e)
{
    State s(c_slide->getValue(), stereo->getValue());
   
    e->copyData((char*)&s, sizeof(State));
}

void GCrossfade::setState(SoundCompEvent* e)
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
   
    c_slide->setValue(s->c_slide);
    f_val->set(c_slide->getValue());
   
    stereo->setValue(s->stereo);
    cross->setMix(!stereo->getValue());
   
}

