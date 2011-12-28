#include "g_stepmatrix.h"

/**********/
/*GSTEPROW*/
/**********/

StepRow::StepRow(uint ncol)
{
    this->ncol = ncol;
   
    out = new OneShot();
    //out->setLevel();
   
    vec = new Data(ncol);
   
    buttons = new Button*[ncol];
   
    for(uint i=0;i<vec->numCol();i++)
    {
	
        buttons[i] = new Button(pix["but8x8off"],pix["but8x8on"]);
        buttons[i]->stayPressed(true);
        buttons[i]->setPressed(false);
        buttons[i]->setAction(new C_COL(this, i));
        add(buttons[i],i*(buttons[i]->getW()+1),0);
    }
}

void StepRow::update()
{
    for(uint i=0;i<vec->numCol();i++)
    {
        buttons[i]->setPressed(vec->get(i));
    }
   
    if(vec->isOn())out->set();
    else out->unset();
}


StepRow::~StepRow()
{
    delete out;
    delete[] buttons;
    delete vec;
}


bool StepRow::isOn()
{
    return vec->isOn();
}

void StepRow::clear()
{
    vec->clear();
    update();
}

void StepRow::step(uint stop)
{
    vec->step(stop);
   
    if(vec->isOn())out->set();
    else out->unset();
}

uint StepRow::numCol() const
{
    return vec->numCol();
}

uint StepRow::getIndex()
{
    return vec->getIndex();
}

void StepRow::setIndex(uint i)
{
    vec->setIndex(i);
   
    if(vec->isOn())out->set();
    else out->unset();
}

void StepRow::setAll(const RowData& s)
{
    vec->setAll(s);
   
    if(vec->isOn())out->set();
    else out->unset();
}

void StepRow::getAll(RowData& s)
{
    return vec->getAll(s);
}

Value** StepRow::getOut()
{
    return  (Value**)&out;
}

int StepRow::buttonW() const
{
    return buttons[0]->getW();
}

int StepRow::buttonH() const
{
    return buttons[0]->getH();
}

void StepRow::setUseShot(bool m)
{
    out->setAlwaysOn(m);
}


//DATA

StepRow::Data::Data(uint ncol)
{
    this->ncol=ncol;
   
    vec = new bool[ncol];
   
    for(uint i=0;i<ncol;i++)vec[i]=false;
   
    index=0;
}

StepRow::Data::Data(const Data& rh)
{
    ncol=rh.ncol;
   
    vec = new bool[ncol];
   
    index=rh.index;
    for(uint i=0;i<ncol;i++)vec[i]=rh.vec[i];
}

void StepRow::Data::setAll(const RowData& s)
{
    uint min = minOf(s.ncols, ncol);
    for(uint i=0;i<min;i++)vec[i]=s.data[i];
}

void StepRow::Data::getAll(RowData& s)
{
    s.ncols=ncol;
    for(uint i=0;i<ncol;i++)s.data[i]=vec[i];
}


StepRow::Data::~Data()
{
    delete[] vec;
}

void StepRow::Data::clear()
{
    for(uint i=0;i<ncol;i++)set(i, false);
    //index=0; dont reset index!
}


bool StepRow::Data::get(uint i)
{
    return vec[i];
}


void StepRow::Data::set(uint i, bool v)
{
    vec[i]=v;
}

void StepRow::Data::step(uint stop)
{
    DASSERT(stop<=ncol);
    index++;
    index%=stop;
}

bool StepRow::Data::isOn()
{
    return vec[index];
}

uint StepRow::Data::numCol() const
{
    return ncol;
}

uint StepRow::Data::getIndex()
{
    return index;
}

void StepRow::Data::setIndex(uint i)
{
    index=i;
}



/**********/
/*GSTEPMAT*/
/**********/

GStepMat::GStepMat(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    ncols=NUM_COLUMNS;
    stopcol=ncols;
   
    col=0;
    nrows=NUM_ROWS;
    rows = new StepRow*[nrows];
   
    drain = new MatUpdater(this);
    //use ground
    ground.addInput((Value**)&drain);
   
    patch = new OneShot();
   
   
   
   
    //background
    Pixmap* back_pix = new Pixmap(pix["710x240"]);
    add(back_pix,0,0);
    add(new MoveBorder(this, back_pix->pos.w,20),0,0);
    Label* l = new Label(fnt["label"]);
    l->printF("stepmatrix");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
   
    in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
   
    int xadd=20;
    int yadd=20;
    int xoff=10;
    int yoff=20;
   
    //STEP
    l = new Label(f, string("stepin"));
    add(l, xoff, yoff);
   
    add(in, xoff+20, yoff+10);
   
   
    //TRIGGER
   
    xoff=90;
    yoff=20;
   
    l = new Label(f);
    l->printF("trig");
    add(l,xoff, yoff);
   
    xoff+=5;
    yoff+=10;
   
    in = new InputTaker(pix["in"], this, 2);
    addInputTaker(in);
    add(in, xoff, yoff);
   
    endless=true;
    run=true;
    b_endless = new Button(pix["up"],pix["down"]);
    b_endless->stayPressed(true);
    b_endless->setAction(new C_ENDLESS(this));
    b_endless->setPressed(!endless);
    add(b_endless, xoff+20, yoff);
   
    xoff=150;
    yoff=20;
   
    l = new Label(f);
    l->printF("hold gate");
    add(l, xoff, yoff);
   
    hold_gate=false;
    b_holdgate = new Button(pix["up"],pix["down"]);
    b_holdgate->stayPressed(true);
    b_holdgate->setAction(new C_HOLDGATE(this));
    b_holdgate->setPressed(hold_gate);
    add(b_holdgate, xoff+20, yoff+10);
   
    //SYNC
    xoff=390;
    yoff=20;
    l = new Label(f, string("sync"));
    add(l, xoff, yoff);
   
    og = new OutputGiver(pix["out"], this, 0);
    addOutputGiver(og);
    add(og, xoff+10, yoff+10);
   
    sync_in = new InputTaker(pix["in"], this, 1);
    addInputTaker(sync_in);
    add(sync_in, xoff+30, yoff+10);
   
   
    xoff=10+xadd;
    yoff=80+yadd;
    int ysp = 5;
    int xsp=0;
   
    //ROWS
    for(uint i=0;i<nrows;i++)
    {
        rows[i] = new StepRow(ncols);
        xsp=rows[0]->buttonW()+1;
	
        //x+xoff,y+yoff+i*8+ysp*i
        add(rows[i],xoff,yoff+i*8+ysp*i);
	
        row_givers[i] = new OutputGiver(pix["out"],this, 1+i);
        addOutputGiver(row_givers[i]);
        add(row_givers[i], xadd+18+ncols*xsp, -3+yoff+i*rows[i]->buttonH()+(ysp-1)*i+2);
    }
   
    leds = new LedsIndex<int>(pix["led_on"], pix["led_off"], ncols, false, 1 , 
    pix["ledg_on"], pix["ledg_off"], 4, &col);
   
    add(leds, xoff,yoff-8-ysp);
   
   
    col_wait=false;
    col_wait_led = new LedsLevel<bool>(pix["ledr_on"], pix["ledr_off"], 1,false, &col_wait);
    add(col_wait_led, xoff-10, yoff-8-ysp);
   
   
    //stop col
    xoff-=20; //-10
    yoff-=23; //57
   
    b_stopcol = new Button*[ncols];
   
    for(uint i=0;i<ncols;i++)
    {
        b_stopcol[i] = new Button(pix["but8x8off"],pix["but8x8on"]);
        b_stopcol[i]->stayPressed(true);
        b_stopcol[i]->setAction(new C_STOPCOL(this, i+1));
        b_stopcol[i]->setPressed(stopcol==(i+1));
	
        add(b_stopcol[i], xoff+20+i*xsp, yoff);
    }
   
    l = new Label(f);
    l->printF("stp");
    add(l,xoff+15,yoff-12);
   
    l = new Label(f);
    l->printF("gates");
    add(l,645+xadd, yoff-12);
   
    xoff=240+xadd;
    yoff=20;
   
    l = new Label(f);
    l->printF("gate mode");
    add(l,xoff,yoff);
   
    yoff+=10;
   
    l = new Label(f);
    l->printF("shot");
    add(l,xoff-20,yoff);
   
    l = new Label(f);
    l->printF("hold");
    add(l,xoff+45,yoff);
   
    gate_mode = new Button(pix["bs_left"],pix["bs_right"]);
    gate_mode->setPressed(false);
    gate_mode->stayPressed(true);
    gate_mode->setAction(new C_GMode(this));
    add(gate_mode, xoff+20, yoff-5);
   
    xoff=20;
    yoff=215;
   
    l = new Label(f);
    l->printF("clear");
    add(l,xoff,yoff);
   
    Button* b_clear = new Button(pix["up_ns"],pix["down_ns"]);
    b_clear->stayPressed(false);
    b_clear->setAction(new C_Clear(this));
    add(b_clear, xoff+45, yoff-3);
}

GStepMat::~GStepMat()
{
    ground.remInput((Value**)&drain);
   
    delete[] rows;
    delete drain;
   
    delete[] b_stopcol;
   
}

void GStepMat::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)drain->setStep(out);
    else if(fromid==1)drain->setSyncIn(out);
    else drain->setTrig(out);
}

void GStepMat::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)drain->setStep(NULL);
    else if(fromid==1)drain->setSyncIn(NULL);
    else drain->setTrig(NULL);
}


Value** GStepMat::getOutput(unsigned int id)
{
    if(id==0)return (Value**)&patch;
    else 
    {
        DASSERT(id>=1 && id-1<nrows);
	
        return rows[id-1]->getOut();
    }
   
}


int GStepMat::syncToIndex(unsigned int index)
{
    for(uint i=0;i<nrows;i++)
    {
        rows[i]->setIndex(index);
    }
   
    col=rows[0]->getIndex();
    return col;
}

void GStepMat::getState(SoundCompEvent* e)
{
    State s(rows, nrows, stopcol, gate_mode->getValue(), endless, hold_gate);
    e->copyData((char*)&s, sizeof(State));
}

void GStepMat::setState(SoundCompEvent* e)
{
    DASSERT(e!=NULL);
    if(e->empty())return;
    /*if(e->getSize()!=sizeof(State))
      {
      DERROR("size missmatch");
      if(e->getSize() < sizeof(State))return;
      * 
      DERROR("trying to read...");
      }*/
    const State* s = (const State*)e->getData();
   
    stopcol=s->stopcol;
   
    DASSERTP(stopcol>=1 && stopcol<=ncols, "stopcol: " << stopcol);
   
    if(stopcol<1 || stopcol>ncols)stopcol=ncols;
   
    //cout << "doing" << endl;
   
    uint min = minOf(nrows, s->nrows);
    for(uint i=0;i<min;i++)
    {
        rows[i]->setAll(s->vec[i]);
        rows[i]->update();
    }
   
   
    for(uint i=0;i<ncols;i++)
    {
        if( (i+1)==stopcol)b_stopcol[i]->setPressed(true);
        else b_stopcol[i]->setPressed(false);
    }
   
    gate_mode->setValue(s->gmode);
    for(uint i=0;i<nrows;i++)
    {
        rows[i]->setUseShot(gate_mode->getValue());
    }
   
   
    if(progstate.getCurrentLoadVersion()>=10)
    {
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
        }
	
        hold_gate=s->hold_gate;
        b_holdgate->setPressed(hold_gate);
    }
   
}


GStepMat::MatUpdater::MatUpdater(GStepMat* in)
{
    step=NULL;
    sync=NULL;
    trig=NULL;
    src=in;
   
    resync=true;
    restep=true;
    retrig=true;
   
    sync_wait=0;
}

void GStepMat::MatUpdater::setStep(Value** in)
{
    step=in;
}

void GStepMat::MatUpdater::setTrig(Value** in)
{
    trig=in;
}

void GStepMat::MatUpdater::setSyncIn(Value** in)
{
    sync=in;
}


smp_t GStepMat::MatUpdater::nextValue()
{
   
    smp_t sy=0;
    if(sync!=NULL)
    {
        sy = (*sync)->nextValue();
    }
   
    smp_t st = 0;
    if(step!=NULL)
    {
        st = (*step)->nextValue();
    }
   
    smp_t tr=0;
    if(trig!=NULL)
    {
        tr = (*trig)->nextValue();
    }
   
   
    //trig before step!
    if(!src->endless)
    {
        if(tr > GATE_ZERO_LVL)
        {
            if(retrig)
            {
                src->run=true;
		  
                src->col_wait=false;
                src->col=0;
                src->updateOuts();
		  
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
	     
            //OK to do the step, but only if run!
            if(src->run)
            {
                for(uint i=0;i<src->nrows;i++)
                {
                    src->rows[i]->step(src->stopcol);
                }
                src->col=src->rows[0]->getIndex();
            }
        }
        else 
        {
            sync_wait--;
            return 0;
        }
    }
   
    src->patch->unset();
   
    if(!src->run)return 0;
   
    if(st > GATE_ZERO_LVL)
    {
        if(restep)
        {
	     
            //flag patch when standing on last col going to first
            if(src->rows[0]->getIndex()==src->stopcol-1)
            {
                src->patch->set(); //flag end of cols
		  
                sync_wait=GCOMP_WAIT_SMPL_AFTER_SYNCOUT;
		  
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
		  
                return 0; //+1 for sync wait here
            }
	     
            for(uint i=0;i<src->nrows;i++)
            {
                src->rows[i]->step(src->stopcol);
            }
	     
            //use rows[0], any will do, all are synced
            src->col=src->rows[0]->getIndex();
	     
            restep=false;
	     
	     
        }
    }
    else restep=true;
   
   
   
    //reason for after step:se punchpad
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
   
   
   
    return 0;
}


void GStepMat::C_STOPCOL::action(Component* comp)
{
    DASSERT(i<=src->ncols);
   
    if(i==src->stopcol)
    {
        //stay pressed, do nothing
        src->b_stopcol[src->stopcol-1]->setPressed(true);
        return;
    }
   
    src->b_stopcol[src->stopcol-1]->setPressed(false);
    src->stopcol=i;
}

void GStepMat::C_GMode::action(Component* comp)
{
    Button* b = (Button*)comp;
   
    for(uint i=0;i<src->nrows;i++)
    {
        src->rows[i]->setUseShot(b->isPressed());
    }
}


void GStepMat::updateOuts()
{
    if(col==-1)
    {
        if(!hold_gate)
        {
            for(uint i=0;i<nrows;i++)
            {
                ((OneShot*)(*rows[i]->getOut()))->unset();
            }
        }
    }
    else
    {
        for(uint i=0;i<nrows;i++)
        {
            rows[i]->setIndex(col); //updates out
        }
    }
   
   
}


void GStepMat::C_HOLDGATE::action(Component* comp)
{
    Button* b = (Button*)comp;
   
    src->hold_gate=b->isPressed();
   
    src->updateOuts(); //might need to kill some gates
}


void GStepMat::C_ENDLESS::action(Component* comp)
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
        src->run=false;
        src->col=-1; 
    }
   
    src->updateOuts();
}

void GStepMat::C_Clear::action(Component* comp)
{
    for(uint i=0;i<src->nrows;i++)
    {
        src->rows[i]->clear(); //updates out
    }
   
}
