#include "g_filter.h"

GFilter::GFilter(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    cut_val = new Const(0);
    q_val = new Const(0);
   
    //LP

    filters[DB_6][T_LP] = new Filter_BW_LP_6();
    filters[DB_12][T_LP] = new Filter_BW_LP_12();
    filters[DB_18][T_LP] = new Filter_BW_LP_18();
   
    //HP

    filters[DB_6][T_HP] = new Filter_BW_HP_6();
    filters[DB_12][T_HP] = new Filter_BW_HP_12();
    filters[DB_18][T_HP] = new Filter_BW_HP_18();
   
    //BP
    filters[DB_6][T_BP] = new Filter_BW_BP_6();
    filters[DB_12][T_BP] = new Filter_BW_BP_12();
   
    filters[DB_18][T_BP] = new Filter_Identity();
   
    for(uint i=0;i<N_DECIBEL;i++)
    {
        for(uint j=0;j<N_TYPE;j++)
        {
            filters[i][j]->setSampleRate(progstate.getSampleRate());
	     
            //good pratice: however what are good values?
            filters[i][j]->cutSafe(7.5*i,progstate.getSampleRate()/2-7.5*i);
            filters[i][j]->qSafe(0.2,10);
            filters[i][j]->setUseSafe(true);
        }
    }
   
	
   
    //input
    it[0] = new InputTaker(pix["in"], this,0);
    it[1] = new InputTaker(pix["in"], this,1);
    it[2] = new InputTaker(pix["in"], this,2);
    addInputTaker(it,3);
   
   
    //output
    OutputGiver* out0 = new OutputGiver(pix["out"], this);
    addOutputGiver(out0);
   
    //background
    Pixmap* back_pix = new Pixmap(pix["196x96"]);
    add(back_pix,0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    //title
    Label* l = new Label(fnt["label"]);
    l->printF("butterworth");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //in/out
    add(it[0],5, 3);
    add(out0,19, 3); 
   
    l = new Label(f);
    l->printF("bp+18db\n => off");
    add(l,120,70);

   
      
    //cut in
    add(it[1],32, 23);
      
    //cut slider
    int yoff=20;
    int xoff=0;
    cutoff = new KnobSlider(pix["knob16x16"]);
    cutoff->setMouseMoveAction(new C_PARAM(this));
    add(cutoff,12+xoff,21+yoff);
    cutoff->setContinous(0,1.0, 1.0/(float)KNOB_NUMREVS);
    cutoff->setTurnSpeed(KNOB_TURN_FAST);
    cutoff->setValue(0.25); //initial

    /*
      hz_scl = new Button(pix["bs_up"],pix["bs_down"]);
      hz_scl->stayPressed(true);
      hz_scl->setAction(new C_PARAM(this));
      add(hz_scl, 28+xoff,17+yoff);*/
         
    l = new Label(f);
    l->printF("Cut");
    add(l,5,26);
   
    //Q in
   
    yoff=0;
    xoff=50;
   
    add(it[2],19+xoff, 23+yoff);
        
    //Q slider
    q = new KnobSlider(pix["knob16x16"]);
    q->setMouseMoveAction(new C_PARAM(this));
    add(q,12+xoff,41+yoff);
    q->setContinous(0,1.0, 1.0/(float)KNOB_NUMREVS);
    q->setTurnSpeed(KNOB_TURN_FAST);
    q->setValue(0.25); //initial

    l = new Label(f);
    l->printF("Q");
    add(l,5+xoff,26+yoff);
      
    //mode
    yoff=10;
    xoff=40;
    SDL_Surface* surf[3];
    surf[0]=pix["bs_up"];
    surf[1]=pix["bs_middle_v"];
    surf[2]=pix["bs_down"];
   
    //c_db
    c_db = new NButton(3, surf);
    c_db->setAction(new C_FILTER(this));
    add(c_db,50+xoff,19+yoff);
   
    xoff+=5;
    l = new Label(f);
    l->printF("6db");
    add(l,64+xoff,16+yoff);
   
    l = new Label(f);
    l->printF("12db");
    add(l,64+xoff,26+yoff);
   
    l = new Label(f);
    l->printF("18db");
    add(l,64+xoff,36+yoff);
   
    //c_type
    xoff+=50;
    c_type = new NButton(3, surf);
    c_type->setAction(new C_FILTER(this));
    add(c_type,50+xoff,19+yoff);
   
    xoff+=5;
    l = new Label(f);
    l->printF("lp");
    add(l,64+xoff,16+yoff);
   
    l = new Label(f);
    l->printF("bp");
    add(l,64+xoff,26+yoff);
   
    l = new Label(f);
    l->printF("hp");
    add(l,64+xoff,36+yoff);
  
    //reset
    l = new Label(f);
    l->printF("reset");
    add(l,42,73);

   
    reset = new Button(pix["up"], pix["down"]);
    reset->setAction(new C_RESET(this));
    add(reset,20,70);
   
    //inital filter
    sig=filters[c_db->getValue()][c_type->getValue()];
    sig->setCutoff((Value**)&cut_val);
    sig->setQ((Value**)&q_val);
    //sig->setInput(&empty);
  
    in=NULL;
   
    updateParam();
}

GFilter::~GFilter()
{
    delete sig;
}

Value** GFilter::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GFilter::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)
    {
        in=out;
        sig->setInput(out);
    }
    else if(fromid==1)
    {
        sig->setCutoff(out);
    }
    else
    {
        sig->setQ(out);
    }
   
   
}

void GFilter::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)
    {
        in=NULL;
        sig->setInput(NULL);
    }
    else if(fromid==1)
    {
        sig->setCutoff((Value**)&cut_val);
    }
    else
    {
        sig->setQ((Value**)&q_val);
    }
}

void GFilter::updateFilter()
{
    Value** cut = sig->getCutoff();
    Value** q = sig->getQ();
   
    sig = filters[c_db->getValue()][c_type->getValue()];
   
    sig->setCutoff(cut);
    sig->setQ(q);
   
    sig->setInput(in);
   
    //cout << typeid(*sig).name() << endl;
}


void GFilter::updateParam()
{
    //update cutoff
    smp_t val;
    /*if(hz_scl->isPressed())*/
    {
        val = cutoff->getValue()*FQMAX/**FQSCALE_ON*/;
    }
/*   else 
     {
     val = cutoff->getValue()*FQMAX*FQSCALE_OFF;
     }*/
   
    cut_val->set(val);
   
    //update Q
    val = Q_MIN+q->getValue()*(Q_MAX-Q_MIN);
    q_val->set(val);
}


/*state*/

void GFilter::getState(SoundCompEvent* e)
{
    State s(c_db->getValue(), c_type->getValue(), cutoff->getValue(), 
    q->getValue(), /*hz_scl->getValue()*/false);
   
    e->copyData((char*)&s, sizeof(State));
}

void GFilter::setState(SoundCompEvent* e)
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
      
    //set values
    cutoff->setValue(s->cut);
    q->setValue(s->q);
    //hz_scl->setValue(s->hzscl);
    updateParam();
     
    c_db->setValue(s->c_db);
    c_type->setValue(s->c_type);
    updateFilter();
}

int GFilter::syncToIndex(unsigned int index)
{
    for(uint i=0;i<N_DECIBEL;i++)
    {
        for(uint j=0;j<N_TYPE;j++)
        {
            //good pratice: however what are good values?
            filters[i][j]->reset(index);
        }
    }
    return 0;
}



/*callback*/

GFilter::C_FILTER::C_FILTER(GFilter* in)
{
    src = in;
}

void GFilter::C_FILTER::action(Component* comp)
{
    src->updateFilter();
}


GFilter::C_PARAM::C_PARAM(GFilter* gen)
{
    src = gen;
}

void GFilter::C_PARAM::action(Component* co)
{
    src->updateParam();
}


GFilter::C_RESET::C_RESET(GFilter* gen)
{
    src = gen;
}

void GFilter::C_RESET::action(Component* co)
{
    for(uint i=0;i<N_DECIBEL;i++)
    {
        for(uint j=0;j<N_TYPE;j++)
        {
            src->filters[i][j]->resetState();
        }
    }
    //src->updateParam();
}

