#include "g_delay.h"

GDelay::GDelay(BitmapFont* f, GroundMixer& g) : SoundComp(g) 
{
    //dubble the input:
    dop = new ValueDoppler();
   
    del = new Delay_MK2(progstate.getSampleRate()*10/*max delay(smp)*/); 
    del->setTimeMult(progstate.getSampleRate());
   
    //del->setFracInterpolateMode(Delay_MK2::FRAC_INTERPOLATE_NONE);
   
    del->setInput(dop->getSlave());
   
    cross = new Crossfade();
    cross->setMix(true);
    cross->setInput((Value**)&dop, 0);
    cross->setInput((Value**)&del, 1);
   
    fade_v = new Const(0.5);
   
    cross->setFade((Value**)&fade_v);
   
    ground.addInput((Value**)&cross);
   
    //add eveything
    Pixmap* back_pix = new Pixmap(pix["214x124"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("delay");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //input
    InputTaker* in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
      
    //output
    OutputGiver* og = new OutputGiver(pix["out"], this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
    int xoff=15;
    int yoff=30;
   
    l = new Label(f);
    l->printF("delay");
    add(l,xoff-5, yoff);
      
    in = new InputTaker(pix["in"], this, 1); //delay
    addInputTaker(in);
    add(in, xoff+45, yoff-2);
   
    xoff=10;
    yoff+=30;
       
    //delay depth slider
   
    l = new Label(f);
    l->printF("mix    0:dry 1:wet");
    add(l,xoff+20,yoff);
   
    //mix
    in = new InputTaker(pix["in"], this, 2);
    addInputTaker(in);
    add(in, xoff+50, yoff-2);
   
    yoff+=15;
   
    mix_slide = new Slider(pix["slide_knob"],pix["slide_under"]);
    mix_slide->setMouseMoveAction(new C_Mix(this));
    mix_slide->setValue(0.5);
    fade_v->setValue(mix_slide->getValue());
   
    add(mix_slide, xoff+30, yoff);
     
    l = new Label(f);
    l->printF("dry");
    add(l,xoff, yoff+4);
   
    l = new Label(f);
    l->printF("wet");
    add(l,xoff+100, yoff+4);
   
    xoff=25+45+10;
    yoff=30;
   
    l = new Label(f);
    l->printF("sec");
    add(l,xoff, yoff);
   
    sec_button = new Button(pix["bs_left"],pix["bs_right"]);
    sec_button->stayPressed(true);
    sec_button->setAction(new C_Sec(this));
    add(sec_button,  xoff+25, yoff-4);
   
    l = new Label(f);
    l->printF("smp,min:1");
    add(l,xoff+30+18, yoff);
   
    xoff=10;
    yoff=105;
   
    l = new Label(f);
    l->printF("peek ahead, smp");
    add(l,xoff, yoff);
   
    in = new InputTaker(pix["in"], this, 3);
    addInputTaker(in);
    add(in, xoff+130, yoff-2);

}

GDelay::~GDelay()
{
    ground.remInput((Value**)&cross);
   
    delete cross;
    delete dop;
    delete del;
    delete fade_v;
   
}


Value** GDelay::getOutput(unsigned int id)
{
    return cross->getOutput();
}

void GDelay::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0) //input
    {
        dop->setInput(out);
    }
    if(fromid==1)//delay
    {
        del->setDelay(out);
    }
    if(fromid==2)//fade
    {
        cross->setFade(out);
    }
    if(fromid==3)
    {
        del->setPeekInput(out);
    }
}

void GDelay::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)
    {
        dop->setInput(NULL);
    }
    if(fromid==1)
    {
        del->setDelay(NULL);
    }
    if(fromid==2)
    {
        cross->setFade((Value**)&fade_v);
    }
    if(fromid==3)
    {
        del->setPeekInput(NULL);
    }
}


/*cb*/

GDelay::C_Mix::C_Mix(GDelay* d)
{
    this->src=d;
}

void GDelay::C_Mix::action(Component* co)
{
    src->fade_v->setValue(src->mix_slide->getValue());
}


GDelay::C_Sec::C_Sec(GDelay* d)
{
    this->src=d;
}

void GDelay::C_Sec::action(Component* co)
{
    if(src->sec_button->getValue())
    {
        src->del->setTimeMult(1);
    }
    else src->del->setTimeMult(progstate.getSampleRate());
}

/*state*/

void GDelay::getState(SoundCompEvent* e)
{
    State s(mix_slide->getValue(), sec_button->getValue());
   
    e->copyData((char*)&s, sizeof(State));
}

void GDelay::setState(SoundCompEvent* e)
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
   
    mix_slide->setValue(s->num);
   
    fade_v->setValue(mix_slide->getValue());
   
    sec_button->setValue(s->b_sec);
   
    if(sec_button->getValue())
    {
        del->setTimeMult(1);
    }
    else del->setTimeMult(progstate.getSampleRate());
   
}

