#include "g_edelay.h"

GEDelay::GEDelay(BitmapFont* f, GroundMixer& g) : SoundComp(g) 
{
    del = new Delay_MK2(progstate.getSampleRate()*10/*max delay(smp)*/); 
    del->setTimeMult(progstate.getSampleRate());
         
    //add eveything
    Pixmap* back_pix = new Pixmap(pix["214x64"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("edelay");
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
    yoff+=15;
   
    l = new Label(f);
    l->printF("peek ahead, smp");
    add(l,xoff, yoff);
   
    in = new InputTaker(pix["in"], this, 2);
    addInputTaker(in);
    add(in, xoff+130, yoff-2);

}

GEDelay::~GEDelay()
{
    delete del;
}


Value** GEDelay::getOutput(unsigned int id)
{
    return (Value**)&del;
}

void GEDelay::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0) //input
    {
        del->setInput(out);
    }
    if(fromid==1)//delay
    {
        del->setDelay(out);
    }
    if(fromid==2)
    {
        del->setPeekInput(out);
    }
}

void GEDelay::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)
    {
        del->setInput(NULL);
    }
    if(fromid==1)
    {
        del->setDelay(NULL);
    }
    if(fromid==2)
    {
        del->setPeekInput(NULL);
    }
}


/*cb*/


GEDelay::C_Sec::C_Sec(GEDelay* d)
{
    this->src=d;
}

void GEDelay::C_Sec::action(Component* co)
{
    if(src->sec_button->getValue())
    {
        src->del->setTimeMult(1);
    }
    else src->del->setTimeMult(progstate.getSampleRate());
}

/*state*/

void GEDelay::getState(SoundCompEvent* e)
{
    State s(sec_button->getValue());
   
    e->copyData((char*)&s, sizeof(State));
}

void GEDelay::setState(SoundCompEvent* e)
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
   
    sec_button->setValue(s->b_sec);
    sec_button->action();
   
}

