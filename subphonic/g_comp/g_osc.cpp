#include "g_osc.h" 

GOsc::GOsc(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    gen = new ComboGen();
    ground.addInput((Value**)&gen);
   
    c_shape = new Const(0.5);
    c_amp = new Const(0);
    c_hz = new Const(0);
    c_sync_in = new Const(0);
    c_per = new ConvertHz2Per(Convert(progstate.getSampleRate()));
    c_per->setInput((Value**)&c_hz);
   
    gen->setSyncIn((Value**)&c_sync_in);
    gen->setAmp((Value**)&c_amp);
    gen->setPeriod((Value**)&c_per);
    gen->setShape((Value**)&c_shape);
   
   
    //set default for b-selectable
    sig = new ValueForward();
    sig->setInput(gen->getGen(ComboGen::SQUARE));
   
    //wform selector
    curr_wf=0;
    wforms[0] = pix["wf_squ"];
    wforms[1] = pix["wf_saw"];
    wforms[2] = pix["wf_tri"];
    wforms[3] = pix["wf_sin"];
    wforms[4] = pix["wf_pulse"];
    wforms[5] = pix["wf_noise"];
   
   
    //output connector
    for(uint i=0;i<NUM_OSC;i++)
    {
        og_osc[i] = new OutputGiver(pix["out"], this, i); //out
        addOutputGiver(og_osc[i]);
    }
   
   
   
   
    og_sync = new OutputGiver(pix["out"], this, NUM_OSC+1); //sync out
    addOutputGiver(og_sync);
   
    //inputs
    it_amp = new InputTaker(pix["in"], this, 0);
    it_per = new InputTaker(pix["in"], this, 1);
    it_shape = new InputTaker(pix["in"], this, 2);
    it_sync = new InputTaker(pix["in"], this, 3);
    it_phase = new InputTaker(pix["in"], this, 4);
    addInputTaker(it_amp); 
    addInputTaker(it_per);
    addInputTaker(it_shape);
    addInputTaker(it_sync);
    addInputTaker(it_phase);
   
    //add everything
    int yoff=0;
   
    //background
    Pixmap* back_pix = new Pixmap(pix["128x184"]);
    add(back_pix,0,0);
   
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("osc");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
   
    SDL_Surface* wpix[NUM_OSC];
    wpix[0] = pix["wf_squ_s"];
    wpix[1] = pix["wf_saw_s"];
    wpix[2] = pix["wf_tri_s"];
    wpix[3] = pix["wf_sin_s"];
    wpix[4] = pix["wf_pulse_s"];
    wpix[5] = pix["wf_noise_s"];
   
   
    //outs
    for(uint i=0;i<NUM_OSC;i++)
    {
        int y = 19+i*16;
	
        Pixmap* o_pix = new Pixmap(wpix[i]);
        add(o_pix, 92, y);
	
        add(og_osc[i], 110,y+2);
    }
   
   
    //FORM select
   
    sig_pix = new Pixmap();
    sig_pix->change(wforms[0]);
    add(sig_pix, 10,22);
   
    //cw_button = new Button(pix["up_ns"],pix["down_ns"]);
    cw_button = new Button(pix["but8x8off"],pix["but8x8on"]);
    cw_button->setAction(new C_WavForm(this));
    add(cw_button, 10+23, 24-4);
   
    og_out = new OutputGiver(pix["out"], this, NUM_OSC);
    addOutputGiver(og_out);
    add(og_out, 10+22,36-4);
   
   
   
    yoff=24+6;
   
    /*
      l = new Label(f);
      l->printF("C  F");
      add(l,56,yoff+10);*/
   
    //amplitude
    l = new Label(f);
    l->printF("amp");
    add(l,6,26+yoff);
   
    add(it_amp, 34,23+yoff);
   
    aslide_c = new KnobSlider(pix["knob16x16"]);
    aslide_c->setMouseMoveAction(new C_AMP(this));
    aslide_c->setContinous(0,AMPMAX_C, AMPMAX_C/(float)KNOB_NUMREVS);
    aslide_c->setTurnSpeed(KNOB_TURN_FAST);
    aslide_c->setValue(0.25); //initial
    add(aslide_c,52,21+yoff);
   
    /*
      aslide_f = new KnobSlider(pix["knob16x16"]);
      aslide_f->setMouseMoveAction(new C_AMP(this));
      aslide_f->setContinous(0,AMPMAX_F, AMPMAX_F/KNOB_NUMREVS);
      aslide_f->setTurnSpeed(KNOB_TURN_FAST);
      aslide_f->setValue(0); //initial
      add(aslide_f, 74,21+yoff);*/
   
   
    //FREQUENCY
    yoff+=0;
    l = new Label(f);
    l->printF("hz");
    add(l,6,48+yoff);
   
    add(it_per, 34,45+yoff);
   
    hslide_c = new KnobSlider(pix["knob16x16"]);
    hslide_c->setMouseMoveAction(new C_FREQ(this));
    hslide_c->setContinous(0, FQMAX_C, FQMAX_C / (float)KNOB_NUMREVS);
    hslide_c->setTurnSpeed(KNOB_TURN_FAST);
    hslide_c->setValue(440); //initial
    add(hslide_c,52,43+yoff);
   
   
    /*hslide_f = new KnobSlider(pix["knob16x16"]);
      hslide_f->setMouseMoveAction(new C_FREQ(this));
      hslide_f->setContinous(0, FQMAX_F, FQMAX_F / KNOB_NUMREVS);
      hslide_f->setTurnSpeed(KNOB_TURN_FAST);
      hslide_f->setValue(0); //initial
      add(hslide_f, 74,43+yoff);*/
   
   
    //SHAPE
    yoff=26;
   
    l = new Label(f);
    l->printF("shp");
    add(l,6,74+yoff);
   
    add(it_shape, 34,72+yoff);
   
    slide = new KnobSlider(pix["knob16x16"]);
    slide->setMouseMoveAction(new C_SHAPE(this));
    slide->setValue(0.5);
    add(slide,52,70+yoff);
   
    //reset shape
    res = new Button(pix["but8x8off"],pix["but8x8on"]);//new Button(pix["up_ns"],pix["down_ns"]);
    res->stayPressed(false);
    res->setAction(new R_RESET(this));
    add(res, 74,70+yoff);
   
   
    l = new Label(f);
    l->printF("hard sync");
    add(l,6,115);
   
    add(og_sync, 20, 125);
   
    add(it_sync, 35, 125);
   
    //PHASE
    l = new Label(f);
    l->printF("phase(not pul)\n1.0=one per");
    add(l,6,140);
    add(it_phase, 35, 160);
   
    //dont forget
    updateAmp();
    updateHz();
}

GOsc::~GOsc()
{
    delete c_amp;
    delete c_hz;
    delete c_per;
    delete c_shape;
    delete c_sync_in;
   
    ground.remInput((Value**)&gen);
    delete gen;
}

string GOsc::name() const
{
    return string("GOsc");
}

int GOsc::syncToIndex(unsigned int index)
{
    gen->reset(index); 
    return 0;
}

void GOsc::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)gen->setAmp(out);
    if(fromid==1)c_per->setInput(out);
    if(fromid==2)gen->setShape(out);
    if(fromid==3)gen->setSyncIn(out);
    if(fromid==4)gen->setPhase(out);
   
}

void GOsc::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)gen->setAmp((Value**)&c_amp);
    if(fromid==1)c_per->setInput((Value**)&c_hz);
    if(fromid==2)gen->setShape((Value**)&c_shape);
    if(fromid==3)gen->setSyncIn((Value**)&c_sync_in);
    if(fromid==4)gen->setPhase(NULL);
}

Value** GOsc::getOutput(unsigned int id)
{
    if(id==NUM_OSC)
    {
        return (Value**)&sig;
    }
   
    if(id==NUM_OSC+1)return gen->getSyncOut();
   
    DASSERT(id<NUM_OSC);
   
    return gen->getGen((ComboGen::TYPE)id); //FIXME: this 'happens' to coincide
   
}

void GOsc::changeWF()
{
    sig_pix->change(wforms[curr_wf]);
   
    //set
    sig->setInput(gen->getGen((ComboGen::TYPE)curr_wf));
   
}



/*******/
/*STATE*/
/*******/

void GOsc::getState(SoundCompEvent* e)
{
    State s(curr_wf, slide->getValue(), 
    aslide_c->getValue(), /*aslide_f->getValue()*/0,
    hslide_c->getValue(), /*hslide_f->getValue()*/0);
   
    e->copyData((char*)&s, sizeof(State));
}

void GOsc::setState(SoundCompEvent* e)
{
    if(e->empty())return;
      
    //THIS: IS UGLY
    if(progstate.getCurrentLoadVersion()>5)
    {
        if(e->getSize()!=sizeof(State))
        {
            DERROR("size missmatch");
            //if(e->getSize() < sizeof(State))return;
            DERROR("trying to read...");
        }
    }
    else
    {
        /*if(e->getSize() != (sizeof(State)-sizeof(double)*2))
          {
          DERROR("size missmatch");
          //if(e->getSize() < sizeof(State))return;
          DERROR("trying to read...");
          }
        **/
    }
   
    const State* s = (const State*)e->getData();
      
    curr_wf=s->num;
    changeWF();
   
    slide->setValue(s->shape);
    c_shape->set(slide->getValue());
   
    aslide_c->setValue(s->amp_c);
    hslide_c->setValue(s->hz_c);
   
    /*
      if(progstate.getCurrentLoadVersion()>5)
      {
      //new saves 0-MAX
      aslide_c->setValue(s->amp_c);
      aslide_f->setValue(s->amp_f);
	
      hslide_c->setValue(s->hz_c);
      hslide_f->setValue(s->hz_f);
      }
      else
      {
      //old saves 0-1
      if(s->ascl>0)
	  {
      aslide_c->setValue(0);
      aslide_f->setValue(s->amp_c*0.01);
	  }
      else
	  { 
      aslide_c->setValue(s->amp_c);
      aslide_f->setValue(0);
	  }
	
      if(s->hscl)
	  {
      hslide_c->setValue(0);
      hslide_f->setValue(s->hz_c*FQMAX_F);
	  }
      else
	  {
      hslide_c->setValue(s->hz_c*FQMAX_C);
      hslide_f->setValue(0);
	  }
      }*/

    updateAmp();
    updateHz();
   
}


void GOsc::updateAmp()
{
    smp_t val=0;

    val = aslide_c->getValue();// + aslide_f->getValue();
   
    c_amp->setValue(val);

}

void GOsc::updateHz()
{
    smp_t val;
   
    val = hslide_c->getValue();// + hslide_f->getValue();
   
    c_hz->set(val);
}



/***********/
/*CALLBACKS*/
/***********/


GOsc::C_WavForm::C_WavForm(GOsc* src)
{
    this->src=src;
}

void GOsc::C_WavForm::action(Component* c)
{
    src->curr_wf++;
    src->curr_wf%=NUM_OSC;
    src->changeWF();
}

void GOsc::R_RESET::action(Component* co)
{
    src->c_shape->set(0.5);
    src->slide->setValue(0.5);
}


GOsc::C_SHAPE::C_SHAPE(GOsc* gen)
{
    src = gen;
}

void GOsc::C_SHAPE::action(Component* co)
{
    KnobSlider* s = (KnobSlider*)co;
    src->c_shape->set(s->getValue() );
}


GOsc::C_AMP::C_AMP(GOsc* gen)
{
    src = gen;
}

void GOsc::C_AMP::action(Component* co)
{
    src->updateAmp();
}

GOsc::C_FREQ::C_FREQ(GOsc* gen)
{
    src = gen;
}

void GOsc::C_FREQ::action(Component* co)
{
    src->updateHz();
}


