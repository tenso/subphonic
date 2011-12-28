#include "g_joystick.h"


GJoystick::GJoystick(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["196x196"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,back_pix->pos.h),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("joystick");
    add(l, GCOMP_LABEL_XOFF+10, GCOMP_LABEL_YOFF);
   
    int xoff=10;
    int yoff=30;
   
    //outs
    for(uint i=0;i<N_AXIS+N_BUTTONS;i++)
    {
        og[i] = new OutputGiver(pix["out"], this, i);
	
        if(i<N_AXIS)axis[i] = new Const(0);
        else buttons[i-N_AXIS] = new Const(0);
    }
    addOutputGiver(og, N_AXIS+N_BUTTONS);
   
    //axis
    for(uint i=0;i<N_AXIS;i++)
    {
        l = new Label(fnt["label"]);
        l->printF("axis %i", i);
        add(l, xoff, yoff);
        add(og[i], xoff+56, yoff-3);
      
        yoff+=12;
    }

    xoff=100;
    yoff=30;
   
    //buttons
    for(uint i=0;i<N_BUTTONS;i++)
    {

        l = new Label(fnt["label"]);
        l->printF("b %i", i);
        add(l, xoff, yoff);
   
        add(og[i+N_AXIS], xoff+32, yoff-3);
      
        yoff+=12;
    }
   
      
    xoff=10;
    yoff=120;
   
    l = new Label(fnt["label"]);
    l->printF("id");
    add(l, xoff, yoff);
   
    xoff+=24;
    id=0;
    ns_id = new NumberSelect<int>(f, 1, 0, &id);
    ns_id->setMax(1);
    ns_id->setValue(id);
    add(ns_id,xoff,yoff);
   
    open=false;
    xoff+=16;
    add(new LedsLevel<bool>(pix["led_on"], pix["led_off"], 1, true, &open), xoff,yoff);

  
    //hold b
    /*b_hold = new Button(pix["up"],pix["down"]);
      b_hold->stayPressed(true);
      b_hold->setAction(new C_HOLD(&stay));
      add(b_hold,xoff+10,yoff+15);*/
   
}

GJoystick::~GJoystick()
{
    for(uint i=0;i<N_AXIS+N_BUTTONS;i++)
    {
        og[i] = new OutputGiver(pix["out"], this, i);
	
        if(i<N_AXIS)delete axis[i];
        else delete buttons[i-N_AXIS];
    }
}


Value** GJoystick::getOutput(unsigned int id)
{
    if(id<N_AXIS)return (Value**)&axis[id];
    return (Value**)&buttons[id-N_AXIS];

}

int GJoystick::getId()
{
    return ns_id->getValue();
}


void GJoystick::setOpen(bool val)
{
    open=val;
}


void GJoystick::giveAxisPos(uint axis, double pos)
{
    if(axis>=N_AXIS)
    {
        cout << "axis unimplemented " << axis << ": " << pos << endl;
        return;
    }
    if(axis%2==1 && axis!=0)pos=-pos;
    this->axis[axis]->setValue((smp_t)pos);
}

void GJoystick::giveButtonState(uint button, bool state)
{
    if(button>=N_BUTTONS)
    {
        cout << "button unimplemented" << button << ": " << state << endl;
        return;
    }
   
    this->buttons[button]->setValue((smp_t)state);
}



/*callbacks*/
/*
  GJoystick::C_HOLD::C_HOLD(bool* in)
  {
  this->in = in;
  }

  void GJoystick::C_HOLD::action(Component* comp)
  {
  (*in)=!(*in);
  }
*/

void GJoystick::getState(SoundCompEvent* e)
{
    State s(ns_id->getValue());
   
    e->copyData((char*)&s, sizeof(State));
}

void GJoystick::setState(SoundCompEvent* e)
{
    if(e->empty())return;
         
    const State* s = (const State*)e->getData();
   
    ns_id->setValue(s->id);
    ns_id->action();
}
