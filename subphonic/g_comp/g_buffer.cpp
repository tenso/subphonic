#include "g_buffer.h" 

GBuffer::GBuffer(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    sig = new ValueBuffer();
         
    Pixmap* back_pix = new Pixmap(pix["128x200"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w, back_pix->pos.h), 0,0);
    Label* l = new Label(fnt["label"]);
    l->printF("buffer");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
      
    //signal in/out
    add(addInputTaker(new InputTaker(pix["in"], this, 0)), GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
    add(addOutputGiver(new OutputGiver(pix["out"], this, 0)), GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
    int xoff=10;
    int yoff=40;
   
    //MAX LEN
    l = new Label(f);
    l->printF("max sec");
    add(l, xoff, yoff);
   
    yoff+=15;
   
    //in seconds
    len=1;
    ns_len = new NumberSelect<smp_t>(f, 3, 0, &len);
    ns_len->setAction(new C_Update(this));
    ns_len->action(); //sets def value
    add(ns_len,xoff,yoff);
      
  
    //READ
    xoff=10; 
    yoff+=40;
    l = new Label(f);
    l->printF("read smp");
    add(l, xoff, yoff);
    yoff+=15;
    add(addInputTaker(new InputTaker(pix["in"], this, 1)), xoff, yoff);
   
    //GATE
    xoff=10;
    yoff+=40;
    l = new Label(f);
    l->printF("sample gate");
    add(l, xoff, yoff);
    yoff+=15;
    add(addInputTaker(new InputTaker(pix["in"], this, 2)), xoff, yoff);
}

GBuffer::~GBuffer()
{
    delete sig;
}



Value** GBuffer::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GBuffer::addInput(Value** in, uint id)
{
    if(id==0)sig->setInput(in);
    else if(id==1)sig->setRead(in);
    else sig->setGate(in);
}

void GBuffer::remInput(Value** out, uint id)
{
    if(id==0)sig->setInput(NULL);
    else if(id==1)sig->setRead(NULL);
    else sig->setGate(NULL);
}


/*state*/

void GBuffer::getState(SoundCompEvent* e)
{
    State s(len/*, suppressed*/);
    e->copyData((char*)&s, sizeof(State));
}

void GBuffer::setState(SoundCompEvent* e)
{
    DASSERT(e!=NULL);
    if(e->empty())return;
   
    const State* s = (const State*)e->getData();
   
    ns_len->setValue(s->len);
    ns_len->action();
}

GBuffer::C_Update::C_Update(GBuffer* src)
{
    this->src=src;
}

void GBuffer::C_Update::action(Component* c)
{
    src->sig->setLen((uint)(src->len*44100));
}
