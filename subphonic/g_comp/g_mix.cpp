#include "g_mix.h" 

GMix::GMix(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    //background
    Pixmap* back_pix = new Pixmap(pix["128x64"]);
    add(back_pix,0,0);
   
   
    mix = new MixerArray(NUM_IN);
   
    //output
    og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
   
    //add eveything
   
    add(new MoveBorder(this, back_pix->pos.w,back_pix->pos.h/*20*/),0,0);
   
    int xoff=-10;
    int yoff=6;
   
    //inputs
    for(uint i=0;i<NUM_IN;i++)
    {
        inputs[i]=NULL;
	
        in[i] = new InputTaker(pix["in"], this, i);
        addInputTaker(in[i]);
	
        add(in[i], xoff+28+17*i, yoff+16);
	
	
        rb[i] = new Button(pix["up"],pix["down"]);
        rb[i]->stayPressed(true);
        rb[i]->setPressed(true);
        rb[i]->setAction(new R_A(this,i));
	
        add(rb[i],xoff+26+17*i, yoff+28);
    }
   
    add(og, 19, 3);
   
    Label* l = new Label(fnt["label"]);
    l->printF("mixer");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
}

GMix::~GMix()
{
    delete mix;
}


Value** GMix::getOutput(unsigned int id)
{
    return (Value**)&mix;
}

void GMix::addInput(Value** out, unsigned int fromid)
{
    inputs[fromid]=out;
    if(rb[fromid]->isPressed())mix->addInput(out,fromid);
}

void GMix::remInput(Value** out, unsigned int fromid)
{
    inputs[fromid]=NULL;
    mix->remInput(fromid);
}



GMix::R_A::R_A(GMix* s, int i)
{
    src = s;
    index=i;
}

void GMix::R_A::action(Component* co)
{
    Button* b = (Button*)co;
    if(!b->isPressed())src->mix->remInput(index);
    else src->mix->addInput(src->inputs[index], index);
}

void GMix::getState(SoundCompEvent* e)
{
    State s(rb);
    e->copyData((char*)&s, sizeof(State));
}

void GMix::setState(SoundCompEvent* e)
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
   
    for(uint i=0;i<NUM_IN;i++)
    {
        rb[i]->setPressed(s->buttons[i]);
        if(!rb[i]->isPressed())mix->remInput(i);
        else mix->addInput(inputs[i], i);
    }
}
