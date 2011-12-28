#include "g_select_in.h" 

GSelectIn::GSelectIn(BitmapFont* f, GroundMixer& g) : SoundComp(g) 
{
    out = new Out(this);
   
    drain = new Drain(this);
    ground.addInput((Value**)&drain);
   
    //add eveything
    Pixmap* back_pix = new Pixmap(pix["330x196"]);
    add(back_pix,0,0);
    add(new MoveBorder(this, back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("select in");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    int xoff=30;
    int yoff=15;
   
    l = new Label(f);
    l->printF("in");
    add(l, 10, yoff+13);
   
    l = new Label(f);
    l->printF("on");
    add(l, 10, yoff+28);
   
    l = new Label(f);
    l->printF("sel");
    add(l, 10, yoff+58);
   
    //inputs
    for(uint i=0;i<NUM_IN;i++)
    {
        inputs[i]=NULL;
	
        in[i] = new InputTaker(pix["in"], this, i);
        addInputTaker(in[i]);
	
        add(in[i], xoff+18+17*i, yoff+10);
	
        b_on[i] = new Button(pix["up"],pix["down"]);
        b_on[i]->stayPressed(true);
        b_on[i]->setPressed(false);
        b_on[i]->setAction(new C_ON(this,i));
	
        add(b_on[i],16+17*i+xoff, yoff+25);
	
        in_sel[i] = new InputTaker(pix["in"], this, NUM_IN+i);
        addInputTaker(in_sel[i]);
        add(in_sel[i], xoff+18+17*i, yoff+55);
        sel_v[i]=NULL;
    }
   
    sel_v_f=NULL;
   
    num_on=0;//NUM_IN;
    selected=0;

   
    leds = new LedsIndex<int>(pix["ledg_on"], pix["ledg_off"], NUM_IN, false, 8);
    leds->setData(&selected);
    add(leds, 20+xoff, 45+yoff);
   
    //output
    og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, 19, 3);
   
    xoff=0;
    yoff=100;
   
    l = new Label(f);
    l->printF("sel cycle");
    add(l, 20+xoff, 3+yoff);
   
    in_sel_f = new InputTaker(pix["in"], this, NUM_IN*2);
    addInputTaker(in_sel_f);
    add(in_sel_f, 100+xoff, yoff);
   
    b_sel = new Button(pix["up"],pix["down"]);
    b_sel->stayPressed(false);
    b_sel->setAction(new C_SEL(this));
   
    add(b_sel,118+xoff, yoff-1);
   
   
   
    sel_v_index=NULL;
   
    l = new Label(f);
    l->printF("sel idx");
    add(l, 20+xoff, 18+yoff);
   
    in_sel_index = new InputTaker(pix["in"], this, NUM_IN*2+4);
    addInputTaker(in_sel_index);
    add(in_sel_index, 100+xoff, yoff+15);
   
   
    //level select, 0-1 CV maps to in(0-NUM_IN) uniform
    sel_v_lvl=NULL;
   
    l = new Label(f);
    l->printF("sel lvl");
    add(l, 20+xoff, 33+yoff);
   
    in_sel_lvl = new InputTaker(pix["in"], this, NUM_IN*2+1);
    addInputTaker(in_sel_lvl);
    add(in_sel_lvl, 100+xoff, yoff+30);
   
    //sync out
    yoff+=55;
    l = new Label(f);
    l->printF("sync");
    add(l, 20+xoff, yoff);
   
    sync = new OneShot();
    sync->setShotLen(progstate.getSampleRate());
   
    l = new Label(f);
    l->printF("len");
    add(l, 65+xoff, yoff);
   
    sync_len = new KnobSlider(pix["knob16x16"]);
    sync_len->setMouseMoveAction(new C_SLEN(this));
    sync_len->setContinous(1, progstate.getSampleRate()*2, progstate.getSampleRate());
    add(sync_len, xoff+70,15+yoff);
    sync_len->setValue(11025);
   
    og_sync = new OutputGiver(pix["out"],this, 1);
    addOutputGiver(og_sync);
    add(og_sync, 20+xoff, 15+yoff);
   
    InputTaker* it_sync = new InputTaker(pix["in"],this, NUM_IN*2+2);
    addInputTaker(it_sync);
    add(it_sync, 35+xoff, 15+yoff);
   
    //num on
   
    xoff=170;
    yoff=103;
   
    l = new Label(f);
    l->printF("num on");
    add(l, xoff, yoff);
   
    InputTaker* it_numon = new InputTaker(pix["in"],this, NUM_IN*2+3);
    addInputTaker(it_numon);
    add(it_numon, 55+xoff, yoff-3);
   
    xoff=170;
    yoff=133;
    //on_out output
    l = new Label(f);
    l->printF("c idx");
    add(l, xoff, yoff);
    on_out = new Const(selected);
    OutputGiver* og = new OutputGiver(pix["out"],this, 2);
    addOutputGiver(og);
    add(og, xoff+55, yoff);
   
    xoff=230;
    yoff=180;
    //on_out output
    l = new Label(f);
    l->printF("lvl: [0 1]");
    add(l,xoff,yoff);
    update();
}

GSelectIn::~GSelectIn()
{
    ground.remInput((Value**)&drain);
    delete drain;
   
    delete out;
    delete sync;
    delete on_out;
   
}

void GSelectIn::update()
{
    if(num_on==0)
    {
        selected=-1;
        return;
    }
   
    if(selected==-1)selected=0; //all were off, restart from 0
   
    uint dbg_done=0;
    while(!b_on[selected]->isPressed() && dbg_done<NUM_IN)
    {
        selected++;
        selected%=NUM_IN;
	
        dbg_done++;
    }
   
    //num_on>0 means some must exist:
    DASSERT(b_on[selected]->isPressed());
   
    on_out->setValue(selected);
}



Value** GSelectIn::getOutput(unsigned int id)
{
    if(id==0)return (Value**)&out;
    else if(id==1)return (Value**)&sync;
    return (Value**)&on_out;
}

void GSelectIn::addInput(Value** out, unsigned int fromid)
{
    if(fromid==NUM_IN*2+4)sel_v_index=out;
    else if(fromid==NUM_IN*2+3)drain->setNumOnIn(out);
    else if(fromid==NUM_IN*2+2)drain->setSyncIn(out);
    else if(fromid==NUM_IN*2+1)sel_v_lvl=out;
    else if(fromid==NUM_IN*2)sel_v_f=out;
    else if(fromid>=NUM_IN)sel_v[fromid-NUM_IN]=out;
    else inputs[fromid]=out;
}

void GSelectIn::remInput(Value** out, unsigned int fromid)
{
    if(fromid==NUM_IN*2+4)sel_v_index=NULL;
    else if(fromid==NUM_IN*2+3)drain->setNumOnIn(NULL);
    else if(fromid==NUM_IN*2+2)drain->setSyncIn(NULL);
    else if(fromid==NUM_IN*2+1)sel_v_lvl=NULL;
    else if(fromid==NUM_IN*2)sel_v_f=NULL;
    else if(fromid>=NUM_IN)sel_v[fromid-NUM_IN]=NULL;
    else inputs[fromid]=NULL;
}


GSelectIn::C_ON::C_ON(GSelectIn* s, int i)
{
    src = s;
    index=i;
}

void GSelectIn::C_ON::action(Component* co)
{
    if(!src->b_on[index]->isPressed())
    {
        src->num_on--;
    }
    else src->num_on++;
   
    src->update();
}

GSelectIn::C_SEL::C_SEL(GSelectIn* s)
{
    src = s;
}

void GSelectIn::C_SEL::action(Component* co)
{
    src->selected++;
    src->selected%=NUM_IN;
   
    src->update();
   
    //signal update
    src->sync->set();
}


GSelectIn::C_SLEN::C_SLEN(GSelectIn* s)
{
    src = s;
}

void GSelectIn::C_SLEN::action(Component* co)
{
    src->sync->setShotLen((uint)src->sync_len->getValue());
}


void GSelectIn::getState(SoundCompEvent* e)
{
    State s(b_on, selected, sync_len->getValue());
    e->copyData((char*)&s, sizeof(State));
}

void GSelectIn::setState(SoundCompEvent* e)
{
    DASSERT(e!=NULL);
    if(e->empty())return;
   
    /*if(e->getSize()!=sizeof(State))
      {
      DERROR("size missmatch");
      //if(e->getSize() < sizeof(State))return;
      * 
      DERROR("trying to read...");
      }*/
   
    const State* s = (const State*)e->getData();
   
    num_on=0;
   
    int min = minOf<int>(NUM_IN, s->num);
   
    if(progstate.getCurrentLoadVersion()<7)min=6; //THIS WAS OLD
   
    for(uint i=0;i<NUM_IN;i++)
    {
        if(i<min)
        {
            b_on[i]->setPressed(s->buttons[i]);
            if(b_on[i]->isPressed())num_on++;
        }
        else b_on[i]->setPressed(false);
    }
    int csel=selected;
    selected=s->selected;
   
    sync_len->setValue(s->slen);
    sync->setShotLen((uint)sync_len->getValue());
   
    update();
   
    //signal update
    if(csel!=selected)sync->set();
}

GSelectIn::Out::Out(GSelectIn* src)
{
    this->src=src;
}

smp_t GSelectIn::Out::nextValue()
{
    if(src->num_on==0)return 0;
    if(src->inputs[src->selected]==NULL)return 0;
    //if(src->selected==-1)return 0;
   
    return (*src->inputs[src->selected])->nextValue();
}


GSelectIn::Drain::Drain(GSelectIn* src)
{
    re_tick_f=true;
   
    for(uint i=0;i<NUM_IN;i++)
    {
        re_tick[i]=true;
    }
   
    this->src=src;
   
    sync_in=NULL;
    resync=true;
   
    in_numon=NULL;
    last_num_on=0;
}

void GSelectIn::Drain::setSyncIn(Value** v)
{
    sync_in=v;
}

void GSelectIn::Drain::setNumOnIn(Value** v)
{
    in_numon = v;
}


smp_t GSelectIn::Drain::nextValue()
{
    bool sel_change=false;
   
   
    //num in overrides all button presses
   
    if(in_numon!=NULL)
    {
        uint num_on = fabs((*in_numon)->nextValue());
        if(num_on>NUM_IN)num_on=NUM_IN;
	
        if(num_on != last_num_on)
        {
            last_num_on=num_on;
	     
            //try to keep selected
	     
            src->num_on=0;
            for(uint i=0;i<NUM_IN;i++)
            {
                if(i<num_on)
                {
                    src->num_on++;
                    src->b_on[i]->setPressed(true);
                }
                else src->b_on[i]->setPressed(false);
            }
	     
            if(!src->b_on[src->selected]->isPressed())
            {
                src->update(); 
            }
        }
    }
   
   
   
    smp_t sy_in=0;
    if(sync_in != NULL)sy_in = (*sync_in)->nextValue();
   
    if(sy_in>GATE_ZERO_LVL)
    {
        if(resync)
        {
            int new_s=0;
            src->selected=new_s;
            src->selected%=NUM_IN;
	     
            src->update();
	     
            sel_change=true;
	     
            resync=false;
        }
    }
    else resync=true;
   
    //check sel cycle
    if(src->sel_v_f!=NULL)
    {
        smp_t tick=0;
	
        tick = (*src->sel_v_f)->nextValue();
        
        if(tick<=0)re_tick_f=true;
	
        if(tick>0 && re_tick_f)
        {
            re_tick_f=false;
	     
            src->selected++;
            src->selected%=NUM_IN;
	     
            src->update();
	     
            sel_change=true;
        }
    }
   
    //check sel in, highest number selected if multiple on
    for(uint i=0;i<NUM_IN;i++)
    {
        if(src->sel_v[i]!=NULL)
        {
            smp_t tick = (*src->sel_v[i])->nextValue();
	     
            if(tick<=0)re_tick[i]=true;
	     
            if(tick>0 && re_tick[i])
            {
                re_tick[i]=false;
		  
                src->selected=i;
		  
                src->update();
		  
                sel_change=true;
            }
        }
    }
   
    //check sel index(zero ouput if index button is off)
    if(src->sel_v_index!=NULL)
    {
        if(src->num_on>0)
        {
            uint t_index=0;
            t_index = (uint)((*src->sel_v_index)->nextValue());
	     
            if(t_index != src->selected)
            {
                if(t_index >= src->num_on)
                {
                    t_index=src->num_on-1;
                }
		  
		  		  
                if(t_index != src->selected)
                {
                    src->selected=t_index;
		       
                    src->update(); 
		       
                    sel_change=true;
                }
            }
        }
    }
      
    //check sel level
    if(src->sel_v_lvl!=NULL)
    {
        if(src->num_on>0)
        {
            smp_t t_lvl=0;
            t_lvl = (*src->sel_v_lvl)->nextValue();
	     
            /*map tick to input,
             * because last must have same weight, i.e not only on 1.0
             * must offset, now last gets a little higher prob
             */
            uint i_on=0;
            if(t_lvl >= 1.0)
            {
                i_on = src->num_on-1;
            }
            else i_on = (uint)((src->num_on)*t_lvl);
	     
            i_on++;
	     
            uint actual=0;
            int found=0;
            for(uint i=0; i<NUM_IN; i++, actual++)
            {
                //cout << src->b_on[i]->isPressed() << " ";
		  
                if(src->b_on[i]->isPressed())
                {
                    found++;
                    if(found==i_on)break;
                }
            }

	     
            DASSERT(src->b_on[actual]->isPressed());
	     
            if(actual!=src->selected)
            {
                src->selected=actual;
                src->selected%=NUM_IN;
		  
                src->update(); //update fixes if selected is off
		  
                sel_change=true;
            }
        }
	
    }
   
    //FIXMENOW: wait after this??
    //signal change
    if(sel_change)src->sync->set();
   
    return 0;
}
