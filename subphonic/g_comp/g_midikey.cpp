#include "g_midikey.h"

char GMIDIkey::key_str[12][3]= {"A ","A#","B ","C ","C#","D ","D#","E ","F ","F#","G ","G#"};

Note::Note()
{
    rel_index=-1;
    on_index=-1;
   
    hz=(short)noteMap(1, 0/*oct*/);
    vel=0;
    n=-1;
    gate_on=false;
   
}


PolyNote::PolyNote(int max)
{
    this->max=max;
    num=max;
   
    notes = new Note[max];
    gate = new Gate*[max];
    hz = new Const*[max];
    vel = new Const*[max];
    key_num = new Const*[max];
   
    for(int i=0;i<max;i++)
    {
        gate[i] = new Gate();
        hz[i] = new Const(0);
        vel[i] = new Const(0);
        key_num[i] = new Const(0);
    }
   
    rel_count=0;
    on_count=0;
   
    char* ptr = new char;
    delete ptr;
}


PolyNote::~PolyNote()
{
    delete[] notes;
   
    for(unsigned int i=0;i<max;i++)
    {
        delete gate[i];
        delete hz[i];
        delete vel[i];
        delete key_num[i];
    }
   
    delete[] gate;
    delete[] hz;
    delete[] vel;
    delete[] key_num;
}


bool PolyNote::noteOn(int n, smp_t hz, smp_t vel, OVERRIDE override, RECYCLE recycle)
{
    //n is actually a uchar in midibase: so max is 127
    //cout << hz << endl; ok
   
    //find a empty slot(if it exist) and open gate
    //if many empty exist take key with smallest rel_index;
    //i.e the key released first(most probable to have finished ADSR etc)
    //a rel_index of -1 means it defenitly is unused take any with -1
   
    int min_rel=-1;
    int found=-1;
   
    //also search for min/max on if they would be needed if found==-1
    int min_on=-1;
    int max_on=-1;
    int min_found=-1;
    int max_found=-1;
   
    for(unsigned int i=0;i<num;i++)
    {
        //search for unused keys i.e gate off
        if(!notes[i].gate_on)
        {
            //on_index==-1 ==> note never used
            if(notes[i].on_index==-1) 
            {
                //if this happens min/max wont be used anyways so ok to break
                found=i;
                break;
            }
	     
            if(recycle==MIN_FREE_INDEX)
            {
                found=i;
                break;
            }
	     
	     
            //take note which has been release the longest
            if(min_rel==-1 || notes[i].rel_index<min_rel)
            {
                min_rel=notes[i].rel_index;
                found=i;
            }
        }
	
        //sarch for override
        if(i==0 || notes[i].on_index<min_on) //i==0 : alywas do first
        {
            min_on=notes[i].on_index;
            min_found=i;
        }
        if(i==0 || notes[i].on_index>max_on)
        {
            max_on=notes[i].on_index;
            max_found=i;
        }
    }
   
    if(found!=-1) //found a free: key on
    {
        readyNote(found, n, hz, vel);
	
        return true;
    }
    else //no free, override an old?
    {
        if(override==OFF)
        {
        }
        //beacause these overrides an already on note, there is no need to off it
        else if(override==OLDEST)
        {
            DASSERT(min_found!=-1);
            readyNote(min_found, n, hz, vel);
	     
            return true;
        }
        else if(override==NEWEST)
        {
            DASSERT(max_found!=-1);
            readyNote(max_found, n, hz, vel);
	     
            return true;
        }
    }
   
   
    return false;
}

//overrides notes
void PolyNote::readyNote(uint index, int n, smp_t hz, smp_t vel)
{
    on_count++;
   
    notes[index].on_index=on_count;
   
    notes[index].gate_on=true;
    notes[index].hz=hz;
    notes[index].n=n;
    notes[index].vel=vel;
      
    this->gate[index]->setOn(true);
    this->hz[index]->set(hz);
    this->vel[index]->set(vel);
    this->key_num[index]->set(n);
   
}


bool PolyNote::retrigNote(int n)
{
    bool found=false;
   
    for(unsigned int i=0;i< num;i++)
    {
        if(notes[i].n==n)
        {
            //count this as a new on event
            on_count++;
            notes[i].on_index=on_count;
	     
            /*rel_count++;
              notes[i].rel_index=rel_count;*/
	     
            gate[i]->offRetrigIn(1);
            found=true;
        }
    }
   
    DASSERT(found);
   
    return found;
}

bool PolyNote::noteOff(int n)
{
    bool found=false;
   
    //find key and set gate off, hack, check noteoff for max not num so
    //that changes in num atleast off's notes
   
    //FIXME: using max here(needed now: might change num notes an leave some on)
    for(unsigned int i=0;i< /*num*/max;i++)
    {
        //cout << notes[i].n << endl;
        //only OFF thouse that are on
        if(notes[i].gate_on && notes[i].n==n)
        {
            rel_count++;
            notes[i].gate_on=false;
            notes[i].rel_index=rel_count;
            //cout << "off:" << i << " " << notes[i].rel_index << endl;
            gate[i]->setOn(false);
            key_num[i]->setValue(0);
	     
            found=true;
        }
    }
   

    /*if(!found)
      this might happen: if poly is full and keys are pressed no note on but note off will get in 
    */
   
   
    return found;
}

void PolyNote::allNotesOff()
{
    //FIXME: using max here
    for(unsigned int i=0;i< /*num*/max;i++)
    {
        if(notes[i].gate_on)
        {
            rel_count++;
            notes[i].gate_on=false;
            notes[i].rel_index=rel_count;
            gate[i]->setOn(false);
            key_num[i]->setValue(0);
        }
    }
}


unsigned int PolyNote::getNumNotes()
{
    return num;
}

void PolyNote::setNumNotes(unsigned int n)
{
    DASSERT(n<=max);
    if(n>max)n=max;
   
    num=n;
}


unsigned int PolyNote::maxNotes()
{
    return max;
}


Gate** PolyNote::getGate(unsigned int n)
{
    DASSERT(n<max);
   
    return &gate[n];
}

Value** PolyNote::getHz(unsigned int n)
{
    DASSERT(n<max);
   
    return (Value**)&hz[n];
}

Value** PolyNote::getVel(unsigned int n)
{
    DASSERT(n<max);
   
    return (Value**)&vel[n];
}

Value** PolyNote::getKeyNum(unsigned int n)
{
    DASSERT(n<max);
   
    return (Value**)&key_num[n];
}

/**********/
/*GMIDIkey*/
/**********/

GMIDIkey::GMIDIkey(BitmapFont* f, GroundMixer& g) : SoundComp(g), notes(MAXPOLY)
{
    octave = 0;
    pwheel=0.5;
   
    pitchwheel = new SmpPoint(&pwheel);
   
    Pixmap* back_pix = new Pixmap(pix["264x260"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
    Label* ll = new Label(fnt["label"]);
    ll->printF("MIDI keyb");
    add(ll,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    og_pw = new OutputGiver(pix["out"], this, 0);
    addOutputGiver(og_pw);
    add(og_pw, 19, 120);
   
    int xstride=15;
   
    for(int i=0;i<NUMPOLY;i++)
    {
        og_hz[i] = new OutputGiver(pix["out"], this, i+ID_HZ_OFF);
        addOutputGiver(og_hz[i]);
        add(og_hz[i], 19+xstride*i, 30);
	
        og_gate[i] = new OutputGiver(pix["out"], this, i+ID_GATE_OFF);
        addOutputGiver(og_gate[i]);
        add(og_gate[i], 19+xstride*i, 53);
	
        LedsLevel<smp_t>* led = new LedsLevel<smp_t>(pix["led_on"], pix["led_off"], 1, true, (*notes.getGate(i))->gatePtr());
        add(led, 20+xstride*i,57);
	
        og_vel[i] = new OutputGiver(pix["out"], this, i+ID_VEL_OFF);
        addOutputGiver(og_vel[i]);
        add(og_vel[i], 19+xstride*i, 88);
	
        og_keynum[i] = new OutputGiver(pix["out"], this, i+ID_KEYNUM_OFF);
        addOutputGiver(og_keynum[i]);
        add(og_keynum[i], 19+xstride*i, 180);
    }
   
   
   
    //numpoly used
    int xoff=200;
    int yoff=110;
   
    ll = new Label(f);
    ll->printF("poly");
    add(ll,xoff, yoff);
   
    poly_used=NUMPOLY;
    ns_poly = new NumberSelect<int>(f, 2,0, &poly_used);
    ns_poly->setMin(1);
    ns_poly->setMax(NUMPOLY);
    ns_poly->setAction(new C_POLYUSED(this));
    ns_poly->setValue(poly_used);
    ns_poly->action();
    add(ns_poly, xoff+10, yoff+10);
   
    /*   Button* pmode1 = new Button(pix["bs_up"],pix["bs_down"]);
         pmode1->stayPressed(true);
         pmode1->setAction(new PMODE(&stay));*/
    //add(pmode1,74,15);
   
   
    ll = new Label(f);
    ll->printF("hz");
    add(ll,3,20);
   
    ll = new Label(f);
    ll->printF("gate");
    add(ll,3,44);
   
    ll = new Label(f);
    ll->printF("vel");
    add(ll,3,78);
   
    ll = new Label(f);
    ll->printF("pwheel");
    add(ll,3,110);
   
    l_lpress = new Label(f);
    l_lpress->printF("key:----\nmidi:---");
    add(l_lpress,100, 110);
   
    //sustain
    xoff=5;
    yoff=145;
   
    ll = new Label(f);
    ll->printF("sustain");
    add(ll,xoff,yoff);
   
    it_sust = new InputTaker(pix["in"], this, 0);
    addInputTaker(it_sust);
    add(it_sust, xoff+60, yoff);
   
    ll = new Label(f);
    ll->printF("note num");
    add(ll, 3, 170);
   
   
    drain = new Drain(this);
    ground.addInput((Value**)&drain);
   
    sustain_on=false; //Drain updates this
   
    /*OVERRIDE MODE*/
    xoff=10;
    yoff=200;
    int dy=10;
   
    ll = new Label(f);
    ll->printF("note override");
    add(ll, xoff, yoff+dy);
   
    ll = new Label(f);
    ll->printF("off");
    add(ll, xoff, yoff+dy*2);
   
    ll = new Label(f);
    ll->printF("oldest");
    add(ll, xoff, yoff+dy*3);
   
    ll = new Label(f);
    ll->printF("newest");
    add(ll, xoff, yoff+dy*4);
   
    yoff+=20;
    xoff+=50;
    SDL_Surface* surf[3];
    surf[0]=pix["bs_up"];
    surf[1]=pix["bs_middle_v"];
    surf[2]=pix["bs_down"];
   
    //c_db
    b_override = new NButton(3, surf);
    b_override->setAction(new C_Override(this));
    add(b_override, xoff, yoff);

    override_mode=PolyNote::OFF;
   
   
   
    xoff=135;
    yoff=200;
    dy=10;
   
    ll = new Label(f);
    ll->printF("note recycle");
    add(ll, xoff, yoff+dy);
   
    ll = new Label(f);
    ll->printF("first release");
    add(ll, xoff, yoff+dy*2);
   
    ll = new Label(f);
    ll->printF("min index");
    add(ll, xoff, yoff+dy*3);
   
    //c_db
    b_recycle = new Button(pix["bs_up"], pix["bs_down"]);
    b_recycle->setPressed(false);
    b_recycle->stayPressed(true);
    b_recycle->setAction(new C_Recycle(this));
    add(b_recycle, xoff+106, yoff+18);

    recycle_mode=PolyNote::FIRST_RELEASE;
   
    xoff=130;
    yoff=130;
   
    ll = new Label(f);
    ll->printF("all off");
    add(ll, xoff, yoff+dy);
   
    Button * b_alloff = new Button(pix["up_ns"], pix["down_ns"]);
    b_alloff->stayPressed(false);
    b_alloff->setAction(new C_Alloff(this));
    add(b_alloff, xoff+20, yoff+20);
}

GMIDIkey::~GMIDIkey()
{
    ground.remInput((Value**)&drain);
   
    delete drain;
   
    delete pitchwheel;
}

void GMIDIkey::setNPolyUsed(uint n)
{
    poly_used=n;
    ns_poly->setValue(poly_used);
    ns_poly->action();
}


void GMIDIkey::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)drain->setSustain(out);
}

void GMIDIkey::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)drain->setSustain(NULL);
}

Value** GMIDIkey::getOutput(unsigned int id)
{
    if(id==ID_PW)return (Value**)&pitchwheel;
    else if(id<ID_GATE_OFF)
    {
        return notes.getHz(id-ID_HZ_OFF);
    }
    else if(id<ID_VEL_OFF)
    {
        return (Value**)notes.getGate(id-ID_GATE_OFF);
    }
    else if(id<ID_KEYNUM_OFF)
    {
        return notes.getVel(id-ID_VEL_OFF);
    }
    return notes.getKeyNum(id-ID_KEYNUM_OFF);
   
    //return NULL;
}

bool GMIDIkey::retrigNote(int n)
{
    return notes.retrigNote(n);
}

bool GMIDIkey::noteOn(int n, smp_t vel)
{
    //cout << "midikey:" << n << endl;
    //cout << "maps to: " << noteMap(n+4-24, octave) << " :" << noteStr(n+4-24, octave) << endl;
    //cout << "   also: " << Note(n+4, octave) << endl;
    //return notes.noteOn(n, noteMap(n+4-24, octave),vel);
   
    int key = n-20; /*see midibase.h*/
   
    bool done = notes.noteOn(key, noteHz(key, octave), vel, override_mode, recycle_mode);
   
    if(done)l_lpress->printF("key:%s\nmidi:%d", noteStr(key, octave).c_str(), n);
   
    return done;
}

bool GMIDIkey::noteOff(int n)
{
    int key = n-20;
   
    if(sustain_on)
    {
        sustain_list.push_back(n);
        return true;
    }
   
    return notes.noteOff(key);
}


void GMIDIkey::setPitchwheel(smp_t val)
{
    pwheel=val;
}

/*CALLBACK*/

GMIDIkey::C_POLYUSED::C_POLYUSED(GMIDIkey* src)
{
    this->src=src;
}

void GMIDIkey::C_POLYUSED::action(Component* c)
{
    src->notes.setNumNotes(src->poly_used);
}

GMIDIkey::C_Alloff::C_Alloff(GMIDIkey* src)
{
    this->src=src;
}

void GMIDIkey::C_Alloff::action(Component* c)
{
    src->notes.allNotesOff();
}

GMIDIkey::C_Override::C_Override(GMIDIkey* src)
{
    this->src=src;
}

void GMIDIkey::C_Override::action(Component* c)
{
    //make sure these correspond to the enum
    if(src->b_override->getValue()==0)
    {
        src->override_mode=PolyNote::OFF;
    }
    else if(src->b_override->getValue()==1)
    {
        src->override_mode=PolyNote::OLDEST;
    }
    else if(src->b_override->getValue()==2)
    {
        src->override_mode=PolyNote::NEWEST;
    }
}

GMIDIkey::C_Recycle::C_Recycle(GMIDIkey* src)
{
    this->src=src;
}

void GMIDIkey::C_Recycle::action(Component* c)
{
    if(src->b_recycle->getValue()==0)
    {
        src->recycle_mode=PolyNote::FIRST_RELEASE;
    }
    else if(src->b_recycle->getValue()==1)
    {
        src->recycle_mode=PolyNote::MIN_FREE_INDEX;
    }

}


/*******/
/*STATE*/
/*******/

void GMIDIkey::getState(SoundCompEvent* e)
{
    State s(poly_used, override_mode, recycle_mode);
   
    e->copyData((char*)&s, sizeof(State));
}

void GMIDIkey::setState(SoundCompEvent* e)
{
    if(e->empty())return;
   
    /*
      if(e->getSize()!=sizeof(State))
      {
      DERROR("size missmatch");
      if(e->getSize() < sizeof(State))return;
	
      DERROR("trying to read...");
      }
    */
   
    const State* s = (const State*)e->getData();
   
    poly_used=s->n;
    notes.setNumNotes(poly_used);
    ns_poly->setValue(poly_used);
   
    if(progstate.getCurrentLoadVersion()>=8)
    {
        override_mode=s->override_mode;
        b_override->setValue((int)override_mode);
    }
   
    if(progstate.getCurrentLoadVersion()>=9)
    {
        recycle_mode=s->recycle_mode;
        b_recycle->setValue((int)recycle_mode);
    }
   
}


/*Drain: for sustain only*/


GMIDIkey::Drain::Drain(GMIDIkey* src)
{
    this->src=src;
    sus=NULL;
}

void GMIDIkey::Drain::setSustain(Value** sus)
{
    this->sus=sus;
}


smp_t GMIDIkey::Drain::nextValue()
{
    if(sus==NULL)return 0;
   
    smp_t sus_v = 0;
    sus_v = (*sus)->nextValue();
   
    if(sus_v <= GATE_ZERO_LVL)
    {
        if(src->sustain_on)
        {
            src->sustain_on=false;
	     
            //clear all notes sustained
            list<int>::iterator it;
	     
            for(it = src->sustain_list.begin();it != src->sustain_list.end();it++)
            {
                src->noteOff(*it);
            }
	     
            src->sustain_list.clear();
        }
	
    }
    if(sus_v>GATE_ZERO_LVL)
    {
        src->sustain_on = true;
    }
   
    return 0;
}

int GMIDIkey::outputGiverId(const string& name, uint port)
{
    if(name == "hz")
    {
        return port+ID_HZ_OFF;
    }
    if(name == "gate")
    {
        return port+ID_GATE_OFF;
    }
    if(name == "vel")
    {
        return port+ID_VEL_OFF;
    }
    if(name == "pwheel")
    {
        return ID_PW;
    }
    return -1;
}

int GMIDIkey::inputTakerId(const string& name, uint port)
{
    if(name == "sustain")
    {
        return 0;
    }
   
    return -1;
}
