#include "g_punchpad.h" 

/**********/
/*PUNCHCOL*/
/**********/

PunchCol::PunchCol(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    font=f;
    n=NKEYS;
    buttons = new Button*[n];
   
    key=0;
    oct_off=0;
    octave=0;
   
    for(int i=0;i<n;i++)
    {
        buttons[i] = new Button(pix["but8x8off"],pix["but8x8on"]);
        buttons[i]->stayPressed(true);
        buttons[i]->setPressed(false);
        buttons[i]->setAction(new R_A(this, i+1));
        add(buttons[i],0,(n-1-i)*buttons[i]->getH());
    }
    ns = new NumberSelect<short>(font, 1,0, &octave, true, NumberSelect<short>::UNDER); 
    ns->setAction(new Update(this));
    add(ns,0,n*buttons[0]->getH()+2);
   
    master=NULL;
}

PunchCol::~PunchCol()
{
    //buttons deleted by ~Container()
    delete[] buttons;
}


void PunchCol::setMaster(GPunchPad* m)
{
    master=m;
}


void PunchCol::setHz()
{
    if(key<=0)return;
   
    hz = noteMap(key, octave+oct_off); 
}

smp_t PunchCol::getHz()
{
    return hz;
}

void PunchCol::setOctOff(short o)
{
    oct_off=o;
   
    setHz();
}

short PunchCol::getOctOff()
{
    return oct_off;
}

void PunchCol::transpose(int n)
{
    if(n==0)return;
   
    //only transpose when there is a key
    if(key==0)
    {
        return;
    }
   
    buttons[key-1]->setPressed(false);
   
    key+=n;
   
    //FIXME: check theses
    if(key <= 0)
    {
        uint octs = (key-12)/(this->n);
        ns->setValue(octave+octs);
        key-=12*octs;
        DASSERT(key>0);
    }
   
    if(key > this->n)
    {
        uint octs = (key-1)/this->n;
        ns->setValue(octave+octs);
        key-=12*octs;
    }
   
   
   
    buttons[key-1]->setPressed(true);
    setHz();
   
}


PunchCol::R_A::R_A(PunchCol* s, int i)
{
    src=s;
    index=i; //what key num
}

void PunchCol::R_A::action(Component* co)
{
    //Button* b = (Button*)co;
    if(src->key==index)src->key = 0; //=off
    else if(src->key<=0) //=on, first
    {
        src->key=index;
    }
    else //on, after another
    {
        src->buttons[src->key-1]->setPressed(false);
        src->key=index;
    }
    src->setHz();
   
    //update p.pad(unnessecary if this col is not the one used)
    DASSERT(src->master!=NULL);
    /*if(src->master->col==mycol)*/src->master->updateOuts();
}


bool PunchCol::isOn()
{
    return key>0;
}

void PunchCol::getState(ColState& s)
{
    s.oct=octave;
    s.key = key - 1; //WARNING: -1 : old key indices, also in setState
}

void PunchCol::setState(const ColState& s)
{
    //off old
    if(key>0)buttons[key-1]->setPressed(false);
   
    //on new
    key = s.key + 1;  //WARNING: +1 : old key indices, also in setState
    if(key>0)buttons[key-1]->setPressed(true);
   
    ns->setValue(s.oct);
   
}

void PunchCol::setOctave(short o)
{
    DASSERT(o>=0);
    ns->setValue(o);
    setHz();
}


void PunchCol::setKey(int k)
{
    if(key!=0)buttons[key-1]->setPressed(false);
   
    key=k;
    if(key!=0)buttons[key-1]->setPressed(true);
   
    setHz();
}


PunchCol::Update::Update(PunchCol* s)
{
    src=s;
}

void PunchCol::Update::action(Component* comp)
{
    src->setHz();
    DASSERT(src->master!=NULL);
    src->master->updateOuts();
}

/**********/
/*PUNCHPAD*/
/**********/

char GPunchPad::key_str[12][3] = {"A ","A#","B ","C ","C#","D ","D#","E ","F ","F#","G ","G#"};

GPunchPad::GPunchPad(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    ncols=64;
    stopcol=ncols; //start at 1, i.e stopcol=1 is cols[0]
   
    col=0;
    font=f;
   
    cols = new PunchCol*[ncols];
    keyout = new Const(0);
    gate = new Const(0);
    patch = new OneShot();
   
    updater = new PadUpdater(this);
    ground.addInput((Value**)&updater);
   
    //background
    //back_pix = new Pixmap(pix["368x280"]);
    back_pix = new Pixmap(pix["700x300"]);
    add(back_pix,0,0);
    add(new MoveBorder(this, back_pix->pos.w,20),0,0);
   
   
    for(int i=0;i<3;i++)
    {
        og[i] = new OutputGiver(pix["out"],this, i); //WARNING: id need to increment like 0,1..n
        addOutputGiver(og[i]);
    }
   
   
    text = new Label(font);
    text->printF("sync in/out");
    add(text,270,19);
   
   
    add(og[2], 340,28);
   
    sync_in = new InputTaker(pix["in"], this, 1);
    addInputTaker(sync_in);
   
    add(sync_in, 315, 28);
   
   
   
    int yoff=100;
    int xsp = 10;
    int xoff=45;
    for(uint i=0;i<ncols;i++)
    {
        cols[i] = new PunchCol(font, ground); //0,0 unused anyways
        cols[i]->setMaster(this);
        add(cols[i],xoff+i*xsp, yoff);
    }
   
    leds = new LedsIndex<int>(pix["led_on"], pix["led_off"], ncols, false, 1 ,
    pix["ledg_on"], pix["ledg_off"], 4, &col);
    add(leds, xoff,yoff-10);
   
   
    l = new Label*[ncols];
   
    for(uint i=0;i<12;i++)
    {
        l[i] = new Label(font);
	
        l[i]->printF("%s",key_str[11-i]);
        add(l[i],xoff-30,yoff+i*9);
    }
   
    octave=0;
    ns = new NumberSelect<short>(font, 1,0, &octave);
    ns->setAction(new C_OCTAVE(this));
   
    for(uint i=0;i<ncols;i++)
    {
        cols[i]->setOctOff(octave);
    }
    add(ns,xoff-28, yoff+9*12+2);
   
   
   
    //stop col
    xoff-=20;
    yoff-=20;
   
    b_stopcol = new Button*[ncols];
   
    for(uint i=0;i<ncols;i++)
    {
        b_stopcol[i] = new Button(pix["but8x8off"],pix["but8x8on"]);
        b_stopcol[i]->stayPressed(true);
        b_stopcol[i]->setAction(new C_STOPCOL(this, i+1));
        b_stopcol[i]->setPressed(stopcol==(i+1));
        add(b_stopcol[i], xoff+20+i*xsp, yoff);
    }
   
    text = new Label(font);
    text->printF("stp");
    add(text,xoff-15,yoff);
   
   
    text = new Label(fnt["label"]);
    text->printF("punchpad");
    add(text,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //step
    text = new Label(font);
    text->printF("step");
    add(text,30,19);
   
    in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, 40, 30);
   
    xoff=70;
    yoff=19;
   
    text = new Label(font);
    text->printF("trig");
    add(text,xoff,yoff);
   
    in = new InputTaker(pix["in"], this, 2);
    addInputTaker(in);
    add(in, xoff+10, yoff+11);
   
    /*
      text = new Label(font);
      text->printF("trig on");
      add(text,20,45);*/
   
    endless=true;
    run=true;
    //b_endless = new Button(pix["bs_right"],pix["bs_left"]);
    b_endless = new Button(pix["up"],pix["down"]);
    b_endless->stayPressed(true);
    b_endless->setAction(new C_ENDLESS(this));
    b_endless->setPressed(!endless);
    add(b_endless, xoff+30, yoff+11);
   
    /*   
         text = new Label(font);
         text->printF("on");
         add(text,xoff-20, yoff+39);
         * 
         text = new Label(font);
         text->printF("off");
         add(text,xoff+20, yoff+39);*/
   
    xoff=150;
    yoff=19;
   
    //note/gate
    text = new Label(font);
    text->printF("hz");
    add(text,xoff,yoff);
   
    add(og[0], xoff+10,yoff+11);
   
    text = new Label(font);
    text->printF("gate");
    add(text,xoff+55,yoff);
   
    add(og[1], xoff+70,yoff+11);
   
    xoff=70;
    yoff=60;
   
    text = new Label(font);
    text->printF("hold gate");
    add(text, xoff, yoff);
   
    hold_gate=true;
    b_holdgate = new Button(pix["up"],pix["down"]);
    b_holdgate->stayPressed(true);
    b_holdgate->setAction(new C_HOLDGATE(this));
    b_holdgate->setPressed(hold_gate);
    add(b_holdgate, xoff+75, yoff-4);
   
   
    xoff=10;
    yoff=275;
   
    //transpose
    text = new Label(font);
    text->printF("transpose");
    add(text,xoff,yoff);
   
    Button* b_tran_up = new Button(pix["uparrow_up"],pix["uparrow_down"]);
    b_tran_up->stayPressed(false);
    b_tran_up->setAction(new C_Transpose(this, 1));
    add(b_tran_up, xoff+80, yoff);
   
    Button* b_tran_down = new Button(pix["downarrow_up"],pix["downarrow_down"]);
    b_tran_down->stayPressed(false);
    b_tran_down->setAction(new C_Transpose(this, -1));
    add(b_tran_down, xoff+90, yoff);
   
    xoff+=120;
   
    text = new Label(font);
    text->printF("clear");
    add(text,xoff,yoff);
   
    Button* b_clear = new Button(pix["up_ns"],pix["down_ns"]);
    b_clear->stayPressed(false);
    b_clear->setAction(new C_Clear(this));
    add(b_clear, xoff+45, yoff-3);
   
    col_wait=false;
    col_wait_led = new LedsLevel<bool>(pix["ledr_on"], pix["ledr_off"], 1,false,&col_wait);
    add(col_wait_led, 45-10, 90);
   
    /*REC*/
   
    xoff=450;
    yoff=19;
   
    text = new Label(font);
    text->printF("rec gate/hz");
    add(text,xoff,yoff);
   
    in = new InputTaker(pix["in"], this, 3);
    addInputTaker(in);
    add(in, xoff+30, yoff+11);
   
    xoff+=25;
   
    in = new InputTaker(pix["in"], this, 4);
    addInputTaker(in);
    add(in, xoff+30, yoff+11);
}

GPunchPad::~GPunchPad()
{
    ground.remInput((Value**)&updater); //IMPORTANT
   
    delete patch;
    delete keyout;
    delete gate;
   
    delete[] cols;
   
    //can delete array pointers stil to individual buttons added to cont will be deleted by that
    delete[] b_stopcol;
   
}

void GPunchPad::updateOuts()
{
    if(col==-1)
    {
        //keep hz as it was
        if(!hold_gate)gate->set(0);
        return;
    }
   
    if(cols[col]->isOn())
    {
        smp_t hz=cols[col]->getHz();
        keyout->set(hz);
	
        gate->set(1);
    }
    else 
    {
        gate->set(0);
    }
   
}


int GPunchPad::syncToIndex(unsigned int index)
{
    //FIXME: -1
   
    col=index;
    col%=stopcol;
   
    run=true;
   
    updateOuts();
   
    return col;
}

Value** GPunchPad::getOutput(unsigned int id)
{ 
    if(id==0)return (Value**)&keyout;
    if(id==1)return (Value**)&gate;
   
    return (Value**)&patch;
}

bool GPunchPad::lastCol()
{
    if(col==stopcol-1)return true;
    return false;
}


void GPunchPad::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)updater->setStep(out);
    else if(fromid==1)updater->setSyncIn(out);
    else if(fromid==2)updater->setTrig(out);
    else if(fromid==3)updater->setRecGate(out);
    else if(fromid==4)updater->setRecHz(out);
}

void GPunchPad::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)updater->setStep(NULL);
    else if(fromid==1)updater->setSyncIn(NULL);
    else if(fromid==2)updater->setTrig(NULL);
    else if(fromid==3)updater->setRecGate(NULL);
    else if(fromid==4)updater->setRecHz(NULL);
}

/*STATE*/

void GPunchPad::getState(SoundCompEvent* e)
{
    State s(cols, ncols, ns->getValue(), endless, hold_gate, stopcol);
   
    e->copyData((char*)&s, sizeof(State));
}

void GPunchPad::setState(SoundCompEvent* e)
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
   
    uint min = (ncols > s->n) ? s->n : ncols;
   
    for(uint i=0;i<min;i++)
    {
        cols[i]->setState(s->data[i]);
    }
   
    ns->setValue(s->oct); //updates 'octave'
   
    stopcol = s->stopcol;
    DASSERTP(stopcol>=1 && stopcol<=ncols, "stopcol: " << stopcol);
   
    if(stopcol<1 || stopcol>ncols)stopcol=ncols;
   
    if(col>=stopcol)col=stopcol-1;
   
    for(uint i=0;i<ncols;i++)
    {
        cols[i]->setOctOff(octave);
	
        if( (i+1)==stopcol)b_stopcol[i]->setPressed(true);
        else b_stopcol[i]->setPressed(false);
	
    }
   
    endless=s->endless;
    b_endless->setPressed(!endless);
    if(endless)
    {
        col_wait=false;
        if(col==-1)col=0;
        run=true;
    }
    else 
    {
        /*col_wait=true;
          run=false;
          col=-1;*/
    }
   
   
    hold_gate=s->hold_gate;
    b_holdgate->setPressed(hold_gate);
   
    updateOuts();
}



/*CALLBACKS*/

GPunchPad::PadUpdater::PadUpdater(GPunchPad* in)
{
    step=NULL;
    trig=NULL;
    sync=NULL;
    src=in;
    restep=true;
    resync=true;
    retrig=true;
   
    sync_wait=0;
   
    rec_hz=NULL;
    rec_gate=NULL;
   
    last_rec_hz=0;
    last_rec_col=-2;
}

void GPunchPad::PadUpdater::setRecGate(Value** in)
{
    rec_gate=in;
}

void GPunchPad::PadUpdater::setRecHz(Value** in)
{
    rec_hz=in;
}



void GPunchPad::PadUpdater::setStep(Value** in)
{
    step=in;
}

void GPunchPad::PadUpdater::setTrig(Value** in)
{
    trig=in;
}

//syncs on rising
void GPunchPad::PadUpdater::setSyncIn(Value** in)
{
    sync=in;
}

smp_t GPunchPad::PadUpdater::nextValue()
{
    smp_t sy=0;
    if(sync != NULL)
    {
        sy = (*sync)->nextValue();
    }
   
    smp_t st=0;
    if(step!=NULL)
    {
        st = (*step)->nextValue();
    }
   
    smp_t tr=0;
    if(trig!=NULL)
    {
        tr = (*trig)->nextValue();
    }
   
   
    /******/
    /*PLAY*/
    /******/
   
    //trig before step!
    if(!src->endless)
    {
        if(tr > GATE_ZERO_LVL)
        {
            if(retrig)
            {
                src->run=true;
		  
                /*if(src->col==-1)
                  {*/
		  
                src->col_wait=false;
                src->col=0;
                src->updateOuts();
		  
                //}
		  
                retrig=false;
            }
        }
        else retrig=true;
    }
    else retrig=false;
   
    if(sync_wait>0) //this is the wait after sync
    {
        if(sync_wait==1)
        {
            sync_wait=0;
	     
            //src->col=0;
            //if(!src->endless)src->run=false;
	     
            src->updateOuts();
        }
        else 
        {
            sync_wait--;
            return 0;
        }
    }
   
   
    //this is here to reset step even if no output
    src->patch->unset();
   
    if(!src->run)return 0;
   
    //step matrix?
    if(st > GATE_ZERO_LVL)
    {
        if(restep)
        {
            //src->step();
	     
            //must do this before col++ otherwise would flag statechange while still on last col
            if(src->lastCol())
            {
                src->patch->set(); //flag end of cols
                sync_wait=GCOMP_WAIT_SMPL_AFTER_SYNCOUT;
                restep=false;
		  
                //still set col to what it will be after wait; so that any
                //state changes sets the right col
                if(!src->endless)
                {
                    src->col=-1;
                    src->run=false;
                    src->col_wait=true;
                }
                else
                {
                    src->col=0;
                }
		  
                return 0; //count 1 wait here
            }
	     
            src->col++;
	     
            /*if(!src->endless)
              {
              if(src->col==src->stopcol)src->run=false;
              }
              src->col%=src->stopcol;*/
	     
            restep=false;
	     
            src->updateOuts();
        }
    }
    else restep=true;
   
    //SYNC IN: must do this after step; If using same metro in two pads and one sync the other
    //if first sync: sync ok, but then synced also steps!
   
    //sync on rising, so that a square wave can be used to sync without resyncing it every sample
    if(sy > GATE_ZERO_LVL)
    {
        if(resync)
        {
            src->syncToIndex(0);
            resync=false;
        }
	
        sync_wait=0;
    }
    else resync=true;
   
   
   
    /*RECORD last so that it gets played correctly i.e rec takes president*/
    smp_t r_gate=0;
    if(rec_gate!=NULL)
    {
        r_gate = (*rec_gate)->nextValue();
    }
    smp_t r_hz=0;
    if(rec_hz!=NULL)
    {
        r_hz = (*rec_hz)->nextValue();
    }
   
    if(!feq(last_rec_hz, r_hz) || last_rec_col != src->col)
    {
        if(r_gate>GATE_ZERO_LVL)
        {
            last_rec_hz = r_hz;
            last_rec_col=src->col;
	     
            int key = hzNote(r_hz);
	     
            //REMOVEME: heavy debug(only for exact hz)
            //DASSERT( feq(r_hz, noteHz(key)) );
	     
            if(key<=0)key=1;
	     
            int oct = (key-1)/12;
            key -= 12*oct;
	     
            //DASSERT(key>0 && key<=12);
	     
            src->cols[src->col]->setKey(key);
            src->cols[src->col]->setOctave(oct);
	     
            src->updateOuts();
        }
    }
   
   
    //this is just a dummy
    return 0;
}

void GPunchPad::C_OCTAVE::action(Component* comp)
{
    for(uint i=0;i<src->ncols;i++)
    {
        src->cols[i]->setOctOff(src->octave);
    }
   
    src->updateOuts(); 
}


void GPunchPad::C_ENDLESS::action(Component* comp)
{
    Button* b = (Button*)comp;
   
    src->endless=!b->isPressed();
   
    if(src->endless)
    {
        src->col_wait=false;
        if(src->col==-1)src->col=0;
        src->run=true;
    }
    else
    {
        src->col_wait=true;
        //src->col_wait_led->setValue(src->col_wait);
        src->run=false;
        src->col=-1; 
    }
   
    src->updateOuts();
}


void GPunchPad::C_HOLDGATE::action(Component* comp)
{
    Button* b = (Button*)comp;
   
    src->hold_gate=b->isPressed();
   
    src->updateOuts();
}

void GPunchPad::C_STOPCOL::action(Component* comp)
{
    DASSERT(i<=src->ncols);
   
    if(i==src->stopcol)
    {
        //stay pressed, do nothing
        src->b_stopcol[src->stopcol-1]->setPressed(true);
        return;
    }
   
   
    //depress last pressed
    src->b_stopcol[src->stopcol-1]->setPressed(false);
    src->stopcol=i;
   
    if(src->col >= src->stopcol-1)
    {
        src->col=src->stopcol-1;
        src->updateOuts();
    }
   
}


void GPunchPad::C_Transpose::action(Component* comp)
{
    for(uint i=0;i<src->ncols;i++)
    {
        src->cols[i]->transpose(mode);
    }
   
    src->updateOuts();
}

void GPunchPad::C_Clear::action(Component* comp)
{
    src->ns->setValue(0);
   
    for(uint i=0;i<src->ncols;i++)
    {
        src->cols[i]->setKey(0);
        src->cols[i]->setOctOff(0);
        src->cols[i]->setOctave(0);
	
    }
   
    src->updateOuts();
}
