#include "g_keyboard.h"


GKeyboard::GKeyboard(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    octave = 2;
    stay=false;
   
    hz=noteMap(1, (int)octave);
    sig_hz = new SmpPoint(&hz);
   
    ison=0;
    sig_gate = new SmpPoint(&ison);
   
    Pixmap* back_pix = new Pixmap(pix["128x86"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
      
    l_note = new Label(f);
    l_note->printF("%s", noteStr(1, octave).c_str());
    add(l_note,10,GCOMP_LABEL_YOFF);
   
    Label* l = new Label(fnt["label"]);
    l->printF("keyboard");
    add(l, GCOMP_LABEL_XOFF+10, GCOMP_LABEL_YOFF);
   
    int xoff=10;
    int yoff=20;
   
    //outs
    og[0] = new OutputGiver(pix["out"], this, 0);
    og[1] = new OutputGiver(pix["out"], this, 1);
    addOutputGiver(og, 2);
   
    //hz
    l = new Label(f);
    l->printF("hz");
    add(l, xoff, yoff);
   
    xoff+=38;
    add(og[0], xoff, yoff-2); 
   
    xoff=10;
    yoff+=15;
      
    //gate
    l = new Label(f);
    l->printF("gate");
    add(l, xoff, yoff);
   
    xoff+=38;
    add(og[1], xoff, yoff-2); //gate
   
    xoff+=16;
    add(new LedsLevel<smp_t>(pix["led_on"], pix["led_off"], 1, true, &ison), xoff,yoff-9);
   
   
    //octave
    xoff=10;
    yoff+=25;
   
    l = new Label(f);
    l->printF("oct");
    add(l, xoff, yoff);
   
    s_oct = new NumberSelect<int>(f, 2, 0, &octave);
    s_oct->setValue(octave);
    xoff+=36;
    add(s_oct,xoff,yoff-8);
   
    yoff=20;
    xoff+=35;
     
    l = new Label(f);
    l->printF("a.on");
    add(l, xoff, yoff);
   
    //hold b
    b_hold = new Button(pix["up"],pix["down"]);
    b_hold->stayPressed(true);
    b_hold->setAction(new C_HOLD(&stay));
    add(b_hold,xoff+10,yoff+15);
   
}


Value** GKeyboard::getOutput(unsigned int id)
{
    if(id==0)return (Value**)&sig_hz;
    if(id==1)return (Value**)&sig_gate;
    return NULL;
}

void GKeyboard::setHz(int key)
{
    hz = noteMap(key, (int)octave);
    l_note->printF("%s", noteStr(key, octave).c_str());

}

void GKeyboard::gateOn(bool on)
{
    if(on)ison=1.0;
    else ison=0;
}

/*callbacks*/

GKeyboard::C_HOLD::C_HOLD(bool* in)
{
    this->in = in;
}

void GKeyboard::C_HOLD::action(Component* comp)
{
    (*in)=!(*in);
}


void GKeyboard::getState(SoundCompEvent* e)
{
    State s(b_hold->getValue(), s_oct->getValue());
   
    e->copyData((char*)&s, sizeof(State));
}

void GKeyboard::setState(SoundCompEvent* e)
{
    if(e->empty())return;
         
    const State* s = (const State*)e->getData();
   
    stay=s->aon;
    b_hold->setValue(s->aon);
    //b_hold->action();
   
    s_oct->setValue(s->oct);
    s_oct->action();
}
