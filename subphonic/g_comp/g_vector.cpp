#include "g_vector.h"

//FIXME: path to follow

GVector::GVector(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    plane = new SelectPlane(96,96);
    plane->maxWaypoints(1);
    plane->setAction(new C_UpdatePAD(this));
    plane->setMouseMoveAction(new C_UpdatePAD(this));
    //plane->setClickMoveDelay(50);
    plane->showLine(true);
   
    Pixmap* back_pix = new Pixmap(pix["220x148"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Pixmap* vec_pix = new Pixmap(pix["vback96x96"]);
   
    int xoff=94;
    int yoff=32;
    add(vec_pix, xoff, yoff);
    add(plane, xoff, yoff);
   
    mix = new Mixer();
   
    for(int i=0;i<4;i++)
    {
        inputs[i]=&empty;
        in[i] = new InputTaker(pix["in"], this, i);
        addInputTaker(in[i]);
	
        gains[i] = new Gain(0.0);
        gains[i]->setInput(inputs[i]);
        mix->addInput((Value**)&gains[i]);
    }
   
    //CCW start at 9
    int h=96;
    int w=96;
    int hh=h>>1;
    int hw=w>>1;
   
    add(in[0], xoff-16, yoff+hh-6);
    add(in[1], xoff+hw-6, yoff+h+3);
    add(in[2], xoff+3+w, yoff+hh-6);
    add(in[3], xoff-6+hw, yoff-14);
   
    out = new OutputGiver(pix["out"],this,0);
    addOutputGiver(out);
    add(out,6,3);
   
    Coord crd;
    crd.x=0;
    crd.y=0;
    plane->addWaypoint(crd);
   
    /*
      rem_sel = new Button(pix["up"],pix["down"]);
      rem_sel->stayPressed(false);
      rem_sel->setAction(new C_RemSel(this));
      add(rem_sel, 30,30);*/
   
    Label* l = new Label(fnt["label"]);
    l->printF("vector mix");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
   
    //POS IN
   
    xoff=10;
    yoff=20;
   
    l = new Label(f);
    l->printF("pos in");
    add(l, xoff, yoff);
   
    yoff+=15;
    l = new Label(f);
    l->printF("x");
    add(l, xoff, yoff);
   
    InputTaker* it = new InputTaker(pix["in"], this, 4);
    addInputTaker(it);
    add(it, xoff, yoff+16);
   
    xoff+=30;
   
    l = new Label(f);
    l->printF("y");
    add(l, xoff, yoff);
   
    it = new InputTaker(pix["in"], this, 5);
    addInputTaker(it);
    add(it, xoff, yoff+16);
   
    //POS OUT
   
    xoff=10;
    yoff=70;
   
    l = new Label(f);
    l->printF("pos out");
    add(l, xoff, yoff);
   
    yoff+=10;
    l = new Label(f);
    l->printF("x");
    add(l, xoff, yoff);
   
    OutputGiver* og = new OutputGiver(pix["out"], this, 1);
    addOutputGiver(og);
    add(og, xoff, yoff+16);
   
    xoff+=30;
   
    l = new Label(f);
    l->printF("y");
    add(l, xoff, yoff);
   
    og = new OutputGiver(pix["out"], this, 2);
    addOutputGiver(og);
    add(og, xoff, yoff+16);
   
   
    pos_in_v=NULL;
    pos_in_h=NULL;
   
    pos_out_v = new Const(crd.x);
    pos_out_h = new Const(crd.y);
   
    drain = new Drain(this);
   
   
    ground.addInput((Value**)&drain);
   
    //better do this last
    updateGains(crd);
}

GVector::~GVector()
{
    ground.remInput((Value**)&drain);
   
    delete drain;
   
    delete mix;
}

void GVector::updateGains(const Coord& c)
{
   
    //cout << c.x << " " << c.y << endl;
   
    if(c.x<0)
    {
        gains[0]->setGain(-c.x);
        gains[2]->setGain(0);
    }
    else
    {
        gains[2]->setGain(c.x);
        gains[0]->setGain(0);
    }
   
    if(c.y<0)
    {
        gains[1]->setGain(-c.y);
        gains[3]->setGain(0);
    }
    else
    {
        gains[3]->setGain(c.y);
        gains[1]->setGain(0);
    }
   
    pos_out_v->set(c.x);
    pos_out_h->set(c.y);
   
}

void GVector::addInput(Value** out, unsigned int fromid)
{
    if(fromid<4)
    {
        inputs[fromid]=out;
        gains[fromid]->setInput(inputs[fromid]);
    }
    else if(fromid==4)pos_in_v=out;
    else if(fromid==5)pos_in_h=out;
   
}

void GVector::remInput(Value** out, unsigned int fromid)
{
    if(fromid<4)
    {
        inputs[fromid]=&empty;
        gains[fromid]->setInput(inputs[fromid]);
    }
    else if(fromid==4)pos_in_v=NULL;
    else if(fromid==5)pos_in_h=NULL;
   
}

Value** GVector::getOutput(unsigned int id)
{
    if(id==0)return (Value**)&mix;
    else if(id==1)return (Value**)&pos_out_v;
    else if(id==2)return (Value**)&pos_out_h;
   
    DERROR("unreachable");
    return NULL;
}


void GVector::C_UpdatePAD::action(Component* co)
{
    SelectPlane* s = (SelectPlane*)co;
   
    Coord c = s->getValue(0);
   
    src->updateGains(c);
}

void GVector::C_RemSel::action(Component* co)
{
    if(src->plane->hasLastSelected() && src->plane->numWaypoints()>1)
    {
        src->plane->remWaypoint(src->plane->getLastSelected());
	
        Coord c = src->plane->getValue(0);
        src->updateGains(c);
    }
}

/*******/
/*DRAIN*/
/*******/

GVector::Drain::Drain(GVector* s)
{
    src=s;
}


smp_t GVector::Drain::nextValue()
{
    Coord c;
    bool update=false;
   
    if(src->pos_in_v!=NULL)
    {
        c.x = (*src->pos_in_v)->nextValue();
        update=true;
    }
    if(src->pos_in_h!=NULL)
    {
        c.y = (*src->pos_in_h)->nextValue();
        update=true;
    }
   
    if(update)
    {
        src->plane->setValue(0, c);
        src->updateGains(c);
    }
   
   
    return 0;
}
