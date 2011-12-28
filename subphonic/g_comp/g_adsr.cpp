#include "g_adsr.h"

GAdsr::GAdsr(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    att_val=0;
    dec_val=0;
    rel_val=0;
    sus_val=0;
   
    att = new SmpPoint(&att_val);
    dec = new SmpPoint(&dec_val);
    sus = new SmpPoint(&sus_val);
    rel = new SmpPoint(&rel_val);
   
    sig = new AHDSRin();
    sig->setTimeMult(progstate.getSampleRate());
    sig->setAttack((Value**)&att);
    sig->setDecay((Value**)&dec);
    sig->setRelease((Value**)&rel);
    sig->setSustain((Value**)&sus);
   
    epointer = new SmpPoint(sig->envValPtr());
   
    //background
    Pixmap* back_pix = new Pixmap(pix["96x190"]); //144
   
    //output connector
    og[0] = new OutputGiver(pix["out"], this, 0);
    og[1] = new OutputGiver(pix["out"], this, 1);
    addOutputGiver(og,2);
   
    //input connector
    for(int i=0;i<7;i++)
    {
        it[i] = new InputTaker(pix["in"], this, i);
    }
    addInputTaker(it,7);
   
   
    //add everything
    add(back_pix,0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    int y_off=30;
    int y_inc=20;
   
    for(int i=0;i<4;i++)
    {
        if(i==2)
        {
            sustain = new KnobSlider(pix["knob16x16"]);
            sustain->setMouseMoveAction(new C_SUST(this));
            add(sustain, 22+17*i,y_off);
            sustain->setValue(0);
        }
        else
        {
            coarse[i] = new KnobSlider(pix["knob16x16"]);
            coarse[i]->setMouseMoveAction(new C_COARSE(this, i));
            add(coarse[i], 22+17*i,y_off);
            coarse[i]->setValue(0);
	     
            fine[i] = new KnobSlider(pix["knob16x16"]);
            fine[i]->setMouseMoveAction(new C_FINE(this, i));
            add(fine[i], 22+17*i,y_off+y_inc);
            fine[i]->setValue(0);
        }
	
        add(it[i+2], 22+17*i+2,y_off+2*y_inc);
    }
   
    int cf_yoff=36;
    Label* l = new Label(f);
    l->printF("C");
    add(l,10,cf_yoff);
   
    l = new Label(f);
    l->printF("F");
    add(l,10,cf_yoff+20);
   
    add(it[0], GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
    add(og[0], GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF); 
   
    l = new Label(f);
    l->printF("ADSR");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    int lx_off=26;
    int ly_off=22+63;
   
    l = new Label(f);
    l->printF("A");
    add(l,lx_off,ly_off);
   
    l = new Label(f);
    l->printF("D");
    add(l,lx_off+17,ly_off);
   
    l = new Label(f);
    l->printF("S");
    add(l,lx_off+17*2,ly_off);
   
    l = new Label(f);
    l->printF("R");
    add(l,lx_off+17*3,ly_off);
   
    //trigger
    l = new Label(f);
    l->printF("gate");
    add(l,7,98);
   
    add(it[1], 7+36,95);
   
    led=new LedsLevel<smp_t>(pix["led_on"],pix["led_off"]);
    led->setData(sig->gateOn());
    led->setThreshold(0);
    add(led, 60,88);
   
   
    gate_knob = new KnobSlider(pix["knob16x16"]);
    gate_knob->setMouseMoveAction(new C_GATELVL(this));
    gate_knob->setValue(0);
    add(gate_knob,74,98);

   
    smp_t as = gate_knob->getValue();
    sig->setGateLevel(as);
    led->setThreshold(as);
   
    //raw env
    l = new Label(f);
    l->printF("env");
    add(l,7,96+16);
    add(og[1], 43, 95+14);
   
   
    l = new Label(f);
    l->printF("hrdcut");
    add(l,7,96+30);
   
    rb = new Button(pix["up"],pix["down"]);
    rb->stayPressed(true);
    rb->setAction(new C_CUTMODE(this));
    add(rb, 74,120);
   
   
    //gen mode
    int xoff=0;
    int yoff=145;
   
    l = new Label(f);
    l->printF("fix cycle");
    add(l,xoff+10,yoff);
   
    yoff-=3;
    xoff=22+17*2;
   
    sus_len_val = new Const(0);

    sig_gen = new AHDSRgen();
    //this is ok(to share), only one operational at any one time
    sig_gen->setTimeMult(progstate.getSampleRate());
    sig_gen->setAttack((Value**)&att);
    sig_gen->setDecay((Value**)&dec);
    sig_gen->setRelease((Value**)&rel);
    sig_gen->setSustain((Value**)&sus);
    sig_gen->setSustainTime((Value**)&sus_len_val);
   
    usein = new Button(pix["up"],pix["down"]);
    usein->stayPressed(true);
    usein->setAction(new C_GEN(this));
    add(usein, xoff-30, yoff+15);
   
    sus_len_k = new KnobSlider(pix["knob16x16"]);
    sus_len_k->setMouseMoveAction(new C_SUSLEN(this));
    add(sus_len_k,xoff,yoff+15);
    sus_len_k->setValue(0);
   
    xoff=22+17*3+2;
    add(it[6], xoff, yoff+17);
   
    out = sig;
   
    ac = coarse[0]->getValue();
    af = fine[0]->getValue();
    dc = coarse[1]->getValue();
    df = fine[1]->getValue();
    rc = coarse[3]->getValue();
    rf = fine[3]->getValue();
    s= sustain->getValue();
   
}

GAdsr::~GAdsr()
{
    delete sig;
    delete sig_gen;
   
    delete sus_len_val;
    delete att;
    delete dec;
    delete sus;
    delete rel;
   
}


Value** GAdsr::getOutput(unsigned int id)
{
    if(id==0)return (Value**)&out;
    return (Value**)&epointer;
}

void GAdsr::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(out);
    if(fromid==1)sig->setGate(out);
   
    if(fromid==2)sig->setAttack(out);
    if(fromid==3)sig->setDecay(out);
    if(fromid==4)sig->setSustain(out);
    if(fromid==5)sig->setRelease(out);
   
    //set for gen also
    if(fromid==0)sig_gen->setInput(out);
    if(fromid==1)sig_gen->setGate(out);
   
    if(fromid==2)sig_gen->setAttack(out);
    if(fromid==3)sig_gen->setDecay(out);
    if(fromid==4)sig_gen->setSustain(out);
    if(fromid==5)sig_gen->setRelease(out);
   
    //only for gen
    if(fromid==6)sig_gen->setSustainTime(out);
   
}

void GAdsr::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(NULL);
    if(fromid==1)sig->setGate(NULL); //&empty
   
    //set to point to knob values
    if(fromid==2)sig->setAttack((Value**)&att);
    if(fromid==3)sig->setDecay((Value**)&dec);
    if(fromid==4)sig->setSustain((Value**)&sus);
    if(fromid==5)sig->setRelease((Value**)&rel);
   
    //FOR GEN
    if(fromid==0)sig_gen->setInput(NULL);
    if(fromid==1)sig_gen->setGate(NULL); //&empty
   
    //set to point to knob values
    if(fromid==2)sig_gen->setAttack((Value**)&att);
    if(fromid==3)sig_gen->setDecay((Value**)&dec);
    if(fromid==4)sig_gen->setSustain((Value**)&sus);
    if(fromid==5)sig_gen->setRelease((Value**)&rel);
   
    if(fromid==6)sig_gen->setSustainTime((Value**)&sus_len_val);
}

void GAdsr::getState(SoundCompEvent* e)
{
    State state(af,ac,df,dc,rf,rc, s, gate_knob->getValue(), usein->getValue(), rb->getValue());
   
    e->copyData((char*)&state, sizeof(State));
}

void GAdsr::setState(SoundCompEvent* e)
{
    DASSERT(e!=NULL);
    if(e->empty())return;
    if(e->getSize()!=sizeof(State))
    {
        DERROR("size missmatch");
        if(e->getSize() < sizeof(State))return;
	
        DERROR("trying to read...");
    }
    const State* state = (const State*)e->getData();
         
    //set
    af=state->af;
    ac=state->ac;
    df=state->df;
    dc=state->dc;
    rf=state->rf;
    rc=state->rc;
    s=state->s; 
   
    //update knobs
    coarse[0]->setValue(ac);
    fine[0]->setValue(af);
    coarse[1]->setValue(dc);
    fine[1]->setValue(df);
    coarse[3]->setValue(rc);
    fine[3]->setValue(rf);
   
    sustain->setValue(s);
   
    gate_knob->setValue(state->g);
    smp_t as = gate_knob->getValue();
    sig->setGateLevel(as);
    led->setThreshold(as);
   
    rb->setValue(state->hrd);
    sig->setHardCut(rb->getValue());
   
    //update
    for(int i=0;i<4;i++)
    {
        updateVal(i);
    }
   
    usein->setValue(state->usein);
    if(!usein->getValue())
    {
        out = sig;
        led->setData(sig->gateOn());
    }
    else
    {
        out = sig_gen;
        led->setData(sig_gen->gateOn());
    }
   
   
   
}

void GAdsr::updateVal(int i)
{
    switch(i)
    {
        case 0:
            att_val=ac*COARSE_MAX+af*FINE_MAX;
            break;
	
        case 1:
            dec_val=dc*COARSE_MAX+df*FINE_MAX;
            break;
	
        case 3:
            rel_val=rc*COARSE_MAX+rf*FINE_MAX;
            break;
	
        case 2:
            sus_val=s;
            break;
    }
}


/***********/
/*CALLBACKS*/
/***********/

GAdsr::C_COARSE::C_COARSE(GAdsr* s, int i)
{
    src = s;
    this->i=i;
}

void GAdsr::C_COARSE::action(Component* co)
{
    KnobSlider* s = (KnobSlider*)co;
   
    double sval= s->getValue();
   
    switch(i)
    {
        case 0:
            src->ac=sval;
            break;
	
        case 1:
            src->dc=sval;
            break;
	
        case 3:
            src->rc=sval;
            break;
    }
   
    src->updateVal(i);
}



GAdsr::C_FINE::C_FINE(GAdsr* s, int i)
{
    src = s;
    this->i=i;
}

void GAdsr::C_FINE::action(Component* co)
{
    KnobSlider* s = (KnobSlider*)co;
   
    double sval= s->getValue();
   
    switch(i)
    {
        case 0:
            src->af=sval;
            break;
	
        case 1:
            src->df=sval;
            break;
	
        case 3:
            src->rf=sval;
            break;
    }
   
    src->updateVal(i);
}



GAdsr::C_SUST::C_SUST(GAdsr* s)
{
    src = s;
}

void GAdsr::C_SUST::action(Component* co)
{
    KnobSlider* s = (KnobSlider*)co;
   
    src->s = s->getValue();
   
    src->updateVal(2);
}



GAdsr::C_GATELVL::C_GATELVL(GAdsr* s)
{
    src = s;
}

void GAdsr::C_GATELVL::action(Component* co)
{
    KnobSlider* ks = (KnobSlider*)co;
   
    smp_t as = ks->getValue();
    src->sig->setGateLevel(as);
    src->led->setThreshold(as);
   
    /*cout << "slider:" << src->gate_knob->getValue() << endl;
      cout << "led: " << src->led->getThreshold() << endl;
      cout << "gate: " << sig->gateOn() << endl;*/
}


GAdsr::C_CUTMODE::C_CUTMODE(GAdsr* s)
{
    src=s;
}

void GAdsr::C_CUTMODE::action(Component* co)
{
    src->sig->setHardCut(!src->sig->getHardCut());
}


GAdsr::C_GEN::C_GEN(GAdsr* s)
{
    src=s;
}

void GAdsr::C_GEN::action(Component* co)
{
    if(!src->usein->getValue())
    {
        src->out = src->sig;
        src->led->setData(src->sig->gateOn());
    }
    else
    {
        src->out = src->sig_gen;
        src->led->setData(src->sig_gen->gateOn());
    }
   
}
   

GAdsr::C_SUSLEN::C_SUSLEN(GAdsr* s)
{
    src=s;
}

void GAdsr::C_SUSLEN::action(Component* co)
{
    src->sus_len_val->set(src->sus_len_k->getValue());
}
