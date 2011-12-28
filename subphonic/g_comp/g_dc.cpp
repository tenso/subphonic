#include "g_dc.h"

GDc::GDc(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    sig_out = new Const();
   
    Pixmap* back_pix = new Pixmap(pix["200x110"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w, back_pix->pos.h), 0,0);
   
    Label* l = new Label(fnt["label"], "dc");
    add(l,GCOMP_LABEL_XOFF,GCOMP_LABEL_YOFF);
   
    add(addOutputGiver(new OutputGiver(pix["out"], this, 0)), 19, 3);
   
    int xoff=10;
    int yoff=30;
   
    knob = new KnobSlider(pix["knob16x16"]);
    knob->setMouseMoveAction(new C_UpdateOut(this));
    add(knob, xoff+7,yoff+15);
    knob->setValue(1.0);
   
    val_left=0;
    ns_l = new NumberSelect<smp_t>(f, NUM_WHOLE, NUM_FRAC, &val_left, true);
    ns_l->setAction(new C_UpdateLimits(this));
   
    val_right=1.0;
    ns_r = new NumberSelect<smp_t>(f, NUM_WHOLE, NUM_FRAC, &val_right, true);
    ns_r->setAction(new C_UpdateLimits(this));
      
   
    xoff+=35;
   
    l = new Label(f, "left:");
    add(l,xoff, yoff+5);
   
    add(ns_l, xoff+50, yoff-5);
   
    yoff+=30;
   
    l = new Label(f, "right:");
    add(l, xoff, yoff+5);
   
    add(ns_r, xoff+50, yoff-5);

    //show out
    xoff=10;
    yoff+=35;
    l_valshow = new Label(f);
    add(l_valshow, xoff, yoff);
   
    updateLimits();
    updateOut();
}

void GDc::updateOut()
{
    sig_out->set(knob->getValue());
   
    stringstream out;
    out << "out: %." << NUM_FRAC << "f";
    l_valshow->printF(out.str().c_str(), sig_out->nextValue());
   

}

void GDc::updateLimits()
{
    //FIXME: make selectable:
    //keep knob position
   
    double p = knob->getNormValue();
   
    knob->setLimits(val_left, val_right);
   
    knob->setNormValue(p);
}

GDc::~GDc()
{
    delete sig_out;
}

Value** GDc::getOutput(unsigned int id)
{
    return (Value**)&sig_out;
}


GDc::C_UpdateLimits::C_UpdateLimits(GDc* src)
{
    this->src=src;
} 

void GDc::C_UpdateLimits::action(Component* c)
{
    src->updateLimits();
   
    src->updateOut();
}

GDc::C_UpdateOut::C_UpdateOut(GDc* src)
{
    this->src=src;
} 

void GDc::C_UpdateOut::action(Component* c)
{
    src->updateOut();
}


/*state*/

void GDc::getState(SoundCompEvent* e)
{
    State s(knob->getValue(), val_left, val_right);
   
    e->copyData((char*)&s, sizeof(State));
}

void GDc::setState(SoundCompEvent* e)
{
    DASSERT(e!=NULL);
    if(e->empty())return;
    if(e->getSize()!=sizeof(State))
    {
        DERROR("size missmatch");
        cout << "have: " << e->getSize() << " want:" << sizeof(State) << endl;
        if(e->getSize() < sizeof(State))return;
	
        DERROR("trying to read...");
    }
    const State* s = (const State*)e->getData();
   
    if(progstate.getCurrentLoadVersion()<4)
    {
        val_left=0;
        val_right=s->val_left;
	
        if(progstate.getCurrentLoadVersion()<3)
        {
            //old v uses abs(val) and s->neg
            //correct this:
            if(s->neg)
                val_right=-val_right;
        }
	
        ns_l->setValue(val_left);
        ns_r->setValue(val_right);
	
        updateLimits();
	
        knob->setValue(s->knob*val_right);
	
        updateOut();
    }
    else
    {
        ns_l->setValue(s->val_left);
        ns_r->setValue(s->val_right);
	
        updateLimits();
	
        knob->setValue(s->knob);
        updateOut();
    }
}
