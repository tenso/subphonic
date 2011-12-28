#include "g_wav.h" 

//FIXME: left/right chan unsynced

using namespace std;

WavSample::WavSample(const string& n, SampleValue* s)
{
    stereo=false;
    name=n;
    samp[0] = s;
			    
    //make zero sample for right chan, this is somewhat wastefull but code is better
    //! make it two long so it works for stereo also
    Sample* empty = new Sample(2);
    empty->zero();
   
    samp[1] = new SampleValue(empty);
   
}

WavSample::WavSample(const string& n, SampleValue* left, SampleValue* right)
{
    stereo=true;
   
    name=n;
    samp[0] = left;
    samp[1] = right;
}

WavSample::~WavSample() //whane this happends SampleValue will also free Sample
{
    delete samp[0];
    delete samp[1];
}

void WavSample::setGateMode(SampleValue::GATE_MODE mode)
{
    g_mode=mode;
    for(uint i=0;i<2;i++)
    {
        samp[i]->setGateRetrigMode(g_mode);
    }
}

/************/
/*SampleHold*/
/************/

SampleHold::SampleHold()
{
}

SampleHold::~SampleHold()
{
    if(samples.size()==0)return;
   
    for(unsigned int i=0;i<samples.size();i++)
    {
        delete samples[i];
    }
}

int SampleHold::loadDir(const string& path)
{
    ProgramState& progstate = ProgramState::instance();
   
    if(path.size()==0)
    {
        DERROR("path empty");
        return 1;
    }
   
    FileDir fd;
    bool exist = fd.read(path, false, ".wav", false, false);
    if(!exist)
    {
        DERROR("(E) SampleHold loadDIR(): no such dir!");
        return 1;
    }
   
    vector<string> files;
    fd.getFiles(files);
    
    if(files.size()==0)return 0;
   
    for(unsigned int i=0;i<files.size();i++)
    {
        Wav w;
	
        string str=path+files[i];
	
        //FIXME: better errorchecking!
        Wav::err ret = w.load(str);
        //w.info();
	
        if(ret==Wav::FILE_NOTFOUND_ERR)continue;
	
        //something went wrong but probably file does not exist...
        if(!w.ok())
        {
            DERROR("something wrong happend");
            continue;
        }
	
        WavSample* samp;
        if(w.isStereo()) 
        {
            SampleValue* l = new SampleValue(w.makeSample(Wav::EXTRACT_LEFT));
            l->setResampleRate(w.getSampleRate(), progstate.getSampleRate());
	     
            SampleValue* r = new SampleValue(w.makeSample(Wav::EXTRACT_RIGHT));
            r->setResampleRate(w.getSampleRate(), progstate.getSampleRate());

	     
            samp = new WavSample(files[i], l,r);
        }
        else 
        {
            SampleValue* m = new SampleValue(w.makeSample(Wav::EXTRACT_ALL));
            m->setResampleRate(w.getSampleRate(), progstate.getSampleRate());
	     
            samp = new WavSample(files[i], m);
        }
        samples.push_back(samp);
	
        w.del();
    }
   
    return 0;
}

int SampleHold::singleZeroSample()
{
    Sample* samp = new Sample(2);
    samp->zero();
   
    SampleValue* sv = new SampleValue(samp); //! make it two long so it works for stereo also
    WavSample* sampl = new WavSample(string("no files/dir: empty sample"), sv);
    samples.push_back(sampl);
    return 0;
}

int SampleHold::getNum() const
{
    return samples.size();
}

WavSample* SampleHold::getSample(unsigned int i)
{
    DASSERT(i<samples.size());
    if(i>=samples.size())return NULL;
   
    return samples[i];
}

WavSample* SampleHold::operator[](unsigned int i)
{
    return getSample(i);
}


void SampleHold::setGateMode(SampleValue::GATE_MODE mode)
{
    for(unsigned int i=0;i<samples.size();i++)
    {
        samples[i]->setGateMode(mode);
    }
}


GWav::GWav(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    c_pos=NULL;
    c_start_i=NULL;
    c_stop_i=NULL;
   
    loaded=false;
    speed = new Const(1.0);
      
    nsamples=0;
    current[0]=NULL;
    current[1]=NULL;
    curr=0;
    curr_is_stereo=false;
      
    zero=0;
      
    Button* up = new Button(pix["uparrow_up"],pix["uparrow_down"]);
    up->setAction(new C_CHANGE(this, true));
    Button* down = new Button(pix["downarrow_up"],pix["downarrow_down"]);
    down->setAction(new C_CHANGE(this,false));
   
    l = new Label(f);
    l->setMaxChar(29);
   
    //background
    Pixmap* back_pix = new Pixmap(pix["270x255"]);
    add(back_pix,0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* label = new Label(fnt["label"]);
    label->printF("         sample");
    add(label,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    int xoff=64;
    int yoff=50;
   
    //Pixmap* back_ws = new Pixmap(pix["ws_grid95x79"]);
    Pixmap* back_ws = new Pixmap(pix["vback192x96"]);
    add(back_ws, xoff,yoff);
   
    //wave plot
    //pd = new DataPlot<short>(96,80);
    pd = new DataPlot<short>(192,96);
   
    /*SDL_Color color;
      color.r=47;
      color.g=243; 
      color.b=255;
      pd->setColor(color);*/
   
    ProgramState& ps = ProgramState::instance();
    pd->setColor(ps.getGraphColor());
    add(pd,xoff,yoff);
   
    //output
   
      
    label = new Label(f);
    label->printF("L");
    add(label,5, 5);
   
    og[0] = new OutputGiver(pix["out"], this, 0);
    addOutputGiver(og[0]);
    add(og[0],15, 3);   
   
   
    label = new Label(f);
    label->printF("R");
    add(label,5+40, 5);
   
    og[1] = new OutputGiver(pix["out"], this, 1);
    addOutputGiver(og[1]);
    add(og[1],15+40+11, 3);

   
    stereo_led = new LedsLevel<bool>(pix["led_on"], pix["led_off"]);
    stereo_led->setData(&curr_is_stereo);
    add(stereo_led, 15+40,-5);
   
   
    add(up, 4, 26);
    add(down, 4, 34);
   
    Pixmap* p = new Pixmap(pix["30c_back"]);
    add(p,12,27);
    add(l,16,30);

   
    //speed slide
   
    xoff=10;
    yoff=200;
    label = new Label(f);
    label->printF("speed");
    add(label,xoff, yoff+4);
   
    len_slide = new Slider(pix["slide_knob"], pix["slide_under"]);
    len_slide->setMouseMoveAction(new C_Len(this));
    len_slide->setVert(false);
    len_slide->setLimits(-2,2);
    len_slide->setValue(1.0);
   
    add(len_slide, xoff+50+20, yoff);
   
    //reset len
    Button* rb = new Button(pix["up"],pix["down"]);
    rb->setAction(new R_RESET(this));
    add(rb,xoff+30+100+10,yoff);
   
    //in
    InputTaker* it = new InputTaker(pix["in"], this, 1);
    addInputTaker(it);
    add(it, xoff+50, yoff+2);
   
    xoff=10;
    yoff=50;
   
    //gate
    label = new Label(f);
    label->printF("gate");
    add(label,xoff, yoff);
   
    it = new InputTaker(pix["in"], this, 0);
    addInputTaker(it);
    add(it,xoff+10,yoff+10);
   
    leds = new LedsLevel<smp_t>(pix["led_on"], pix["led_off"]);
    leds->setThreshold(0.00001); 
    add(leds, xoff+30,yoff+3);
   
    //POS
    xoff=10;
    yoff=120;
    label = new Label(f);
    label->printF("index"); 
    add(label,xoff,yoff);
   
    it = new InputTaker(pix["in"], this, 2);
    addInputTaker(it);
    add(it, xoff+10, yoff+10);
   
    //STOP
    xoff=10;
    yoff=80;
    label = new Label(f);
    label->printF("stop"); 
    add(label,xoff,yoff);
   
    it = new InputTaker(pix["in"], this, 3);
    addInputTaker(it);
    add(it, xoff+10, yoff+10);
   
    preview=false;
   
    xoff=10;
    yoff=155;
   
    label = new Label(f);
    label->printF("play/pause");
    add(label, xoff, yoff);
   
    b_play = new Button(pix["up_ns"],pix["down_ns"]);
    b_play->stayPressed(false);
    b_play->setAction(new C_PlayPause(this));
    add(b_play, xoff+90, yoff);
   
    playing_led = new LedsLevel<bool>(pix["led_on"], pix["led_off"]);
    //playing_led->setData(  ); //set in updateCurrSample()
    add(playing_led, xoff+90+20, yoff-5);
   
    yoff+=20;
   
    //xoff+=120;
   
    label = new Label(f);
    label->printF("reset");
    add(label,xoff, yoff);
   
    b_play = new Button(pix["up_ns"],pix["down_ns"]);
    b_play->stayPressed(false);
    b_play->setAction(new C_ResetPlay(this));
    add(b_play, xoff+90, yoff);
   
    yoff=155;
    xoff=160;
   
    label = new Label(f);
    label->printF("loop");
    add(label,xoff, yoff);
   
    b_loop = new Button(pix["up"],pix["down"]);
    b_loop->stayPressed(true);
    b_loop->setValue(false);
    b_loop->setAction(new C_Loop(this));
    add(b_loop, xoff+80, yoff);
   
    do_sel_name=false;
    have_app=false;
   
    gate_doppler = new ValueDoppler(1);
    speed_doppler = new ValueDoppler(1);
    speed_doppler->setInput((Value**)&speed);
   
    pos_doppler = new ValueDoppler(1);
    stop_doppler = new ValueDoppler(1);
   
    start_i_doppler = new ValueDoppler(1);
    stop_i_doppler = new ValueDoppler(1);
   
    //gate mode
    yoff+=20;
   
    label = new Label(f);
    label->printF("soft gate");
    add(label,xoff, yoff);
   
    b_gmode = new Button(pix["up"],pix["down"]);
    b_gmode->stayPressed(true);
    b_gmode->setAction(new C_GMode(this));
    add(b_gmode, xoff+80, yoff);
   
   
    //LENGTH
    yoff=200;
    xoff=180;
    smp_len = new Const(0);
   
    label = new Label(f);
    label->printF("len\nsmp");
    add(label, xoff, yoff);
   
    OutputGiver* og_l = new OutputGiver(pix["out"], this, 2);
    addOutputGiver(og_l);
    add(og_l,xoff+30, yoff);
   
    //start/stop index
    yoff=230;
    xoff=10;
   
    label = new Label(f);
    label->printF("start index");
    add(label, xoff, yoff);
   
    it = new InputTaker(pix["in"], this, 4);
    addInputTaker(it);
    add(it, xoff+96, yoff);
   
    yoff=230;
    xoff=10+140;
   
    label = new Label(f);
    label->printF("stop index");
    add(label, xoff, yoff);
   
    it = new InputTaker(pix["in"], this, 5);
    addInputTaker(it);
    add(it, xoff+96-8, yoff);
}

GWav::~GWav()
{
    delete speed;
    delete gate_doppler;
    delete speed_doppler;
    delete pos_doppler;
    delete stop_doppler;
   
    delete start_i_doppler;
    delete stop_i_doppler;
}

void GWav::updateCurrSamp()
{
    for(int i=0;i<2;i++)
    {
        current[i] = samples[curr]->samp[i];
	
        if(i==0)
        {
            current[i]->setTrigger((Value**)&gate_doppler);
            if(c_pos!=NULL)current[i]->setPosIn((Value**)&pos_doppler);
            current[i]->setStopIn((Value**)&stop_doppler);
            current[i]->setSpeedIn((Value**)&speed_doppler);
	     
            if(c_start_i!=NULL)current[i]->setStartIndex((Value**)&start_i_doppler);
            if(c_stop_i!=NULL)current[i]->setStopIndex((Value**)&stop_i_doppler);
        }
        else 
        {
            current[i]->setTrigger(gate_doppler->getSlave());
            if(c_pos!=NULL)current[i]->setPosIn(pos_doppler->getSlave());
            current[i]->setStopIn(stop_doppler->getSlave());
            current[i]->setSpeedIn(speed_doppler->getSlave());
	     
            if(c_start_i!=NULL)current[i]->setStartIndex(start_i_doppler->getSlave());
            if(c_stop_i!=NULL)current[i]->setStopIndex(stop_i_doppler->getSlave());
        }
	

	
        current[i]->setLoop(b_loop->getValue());
	
        //sound?
        if(preview)current[i]->play();
    }
   
   
    //stereo ok: length of one channel
    smp_len->setValue(current[0]->getLength());

   
    trigval = current[0]->onTrigger();
   
    leds->setData(trigval);
   
    curr_is_stereo=samples[curr]->stereo;
   
    playing_led->setData(current[0]->getIsPlayingPtr());
   
    l->printF("%s",samples[curr]->name.c_str());
   
    //FIXME: show both chans (stereo)
    pd->setData(current[0]->getSample()->getSmpls(), current[0]->getSample()->getLen(), SSHORT_MAX);
   
}


void GWav::loadPath(const string& path)
{
    DASSERT(!have_app);
   
    if(loaded)
    {
        DERROR("already loaded");
        return;
    }
   
    if(path.size()==0)
    {
        DERROR("path empty");
        return;
    }
   
    pathname = path;
   
    string use_path=path;
   
    if(samples.loadDir(use_path)==1)
    {
        VERBOSE1(cout << "path illegal: " << use_path << " , empty sample made" << endl;);
        samples.singleZeroSample();
    }
    else if(samples.getNum()==0)
    {
        VERBOSE1(cout << "no samples to load, empty sample made" << endl;);
        samples.singleZeroSample();
    }
   
    nsamples=samples.getNum();
   
    if(curr > nsamples-1)
    {
        DERROR("loaded (?) curr invalid");
        curr=nsamples-1;
    }
   
    if(curr < 0)
    {
        DERROR("loaded (?) curr invalid");
        curr=0;
    }
   
    updateCurrSamp();
   
    loaded=true;
}

void GWav::loadByAppendix(const string& prefix)
{
    if(loaded)
    {
        DERROR("already loaded");
        return;
    }
    if(!have_app)
    {
        DERROR("wrong fun");
        return;
    }
   
    if(pathname.size()==0)
    {
        DERROR("path empty");
        return;
    }
   
    string use_path;
    if(prefix.size()>0)
    {
        use_path = prefix + pathname;
    }
    else use_path=pathname;
   
    if(samples.loadDir(use_path)==1)
    {
        VERBOSE1(cout << "path illegal: " << use_path << " empty sample made" << endl;);
        samples.singleZeroSample();
    }
    else if(samples.getNum()==0)
    {
        VERBOSE1(cout << "no samples to load, empty sample made" << endl;);
        samples.singleZeroSample();
    }
   
    nsamples=samples.getNum();
   
    DASSERT(curr==0);
   

    if(do_sel_name)
    {
        bool found=false;
	
        for(int i=0;i<nsamples;i++)
        {
            if(samples[i]->name==sel_name)
            {
                curr=i;
                found=true;
                break;
            }
        }
	
        if(!found)
        {
            DERROR("sample not found: " << sel_name);
        }
    }
      
   
    if(curr > nsamples-1)
    {
        DERROR("loaded (?) curr invalid");
        curr=nsamples-1;
    }
   
    if(curr < 0)
    {
        DERROR("loaded (?) curr invalid");
        curr=0;
    }
   
    updateCurrSamp();
   
    loaded=true;
}


Value** GWav::getOutput(unsigned int id)
{
    DASSERTP(loaded,"obj incomplete, loadPath() not run");
   
    if(id<2)return (Value**)&current[id];
    return (Value**)&smp_len;
}

void GWav::addInput(Value** out, unsigned int fromid)
{
    DASSERTP(loaded,"obj incomplete, loadPath() not run");
   
    if(fromid==0)
    {
        gate_doppler->setInput(out);
	
        trigval=current[0]->onTrigger();
        leds->setData(trigval);
    }
    else if(fromid==1)
    {
        speed_doppler->setInput(out);
    }
    else if(fromid==2)
    {
        c_pos=out;
        pos_doppler->setInput(out);
	
        current[0]->setPosIn((Value**)&pos_doppler);
        current[1]->setPosIn(pos_doppler->getSlave());
    }
    else if(fromid==3)
    {
        stop_doppler->setInput(out);
    }
    else if(fromid==4)
    {
        c_start_i=out;
        start_i_doppler->setInput(out);
	
        current[0]->setStartIndex((Value**)&start_i_doppler);
        current[1]->setStartIndex(start_i_doppler->getSlave());
    }
    else if(fromid==5)
    {
        c_stop_i=out;
        stop_i_doppler->setInput(out);
	
        current[0]->setStopIndex((Value**)&stop_i_doppler);
        current[1]->setStopIndex(stop_i_doppler->getSlave());
    }
      
}

void GWav::remInput(Value** out, unsigned int fromid)
{
    DASSERTP(loaded,"obj incomplete, loadPath() not run");
   
    if(fromid==0)
    {
        gate_doppler->setInput(NULL);
	
        leds->setData(&zero);
    }
    else if(fromid==1)
    {
        speed_doppler->setInput((Value**)&speed);
    }
    else if(fromid==2)
    {
        c_pos=NULL;
        pos_doppler->setInput(NULL);
	
        current[0]->setPosIn(NULL);
        current[1]->setPosIn(NULL);
	
    }
    else if(fromid==3)
    {
        stop_doppler->setInput(NULL);
    }
    else if(fromid==4)
    {
        c_start_i=NULL;
        start_i_doppler->setInput(NULL);
	
        current[0]->setStartIndex(NULL);
        current[1]->setStartIndex(NULL);
    }
    else if(fromid==5)
    {
        c_stop_i=NULL;
        stop_i_doppler->setInput(NULL);
	
        current[0]->setStopIndex(NULL);
        current[1]->setStopIndex(NULL);
    }

}



/*APPENDIX
 *int curr (unused)
 *char pathname
 *char currname
 * */

DiskComponent GWav::getDiskComponent()
{
    DASSERT(loaded);
   
    DiskComponent dc(getSCType(), seq, pos);
   
    string cname(samples[curr]->name);
   
    int dsize = sizeof(int)+pathname.length()+1+cname.length()+1;
    Uint8* data = new Uint8[dsize];
    memcpy(data, &curr, sizeof(int));
    memcpy(data+sizeof(int),pathname.c_str(), pathname.length()+1);
    memcpy(data+sizeof(int)+pathname.length()+1,cname.c_str(), cname.length()+1);
   
    dc.setAppendix(data, dsize); 
   
    return dc;
}

void GWav::setAppendix(const Uint8* data, unsigned int len)
{
    curr = *((int*)data);
   
    size_t off = sizeof(int);
    pathname.assign((char*)(data+off));
    off+=pathname.length()+1;
      
    if(progstate.getCurrentLoadVersion()>=2)
    {
        //find sample by name instead
        sel_name.assign((char*)(data+off));
        do_sel_name=true;
        curr=0;
    }
    else do_sel_name=false;
   
    have_app=true;
}

int GWav::syncToIndex(unsigned int index)
{
    if(index!=0)DERROR("sync !=0 not implemented");
   
    current[0]->reset();
    current[1]->reset();
   
    return 0;
}


/*CALLBACKS*/


GWav::C_Len::C_Len(GWav* c)
{
    src = c;
}

void GWav::C_Len::action(Component* co)
{
    DASSERTP(src->loaded,"obj incomplete, loadPath() not run");
      
    Slider* s = (Slider*)co;
   
    smp_t v = s->getValue();
    src->speed->set(v);
   
}

//reset speed slide

GWav::R_RESET::R_RESET(GWav* s)
{
    src=s;
}
void GWav::R_RESET::action(Component* co)
{
    DASSERTP(src->loaded,"obj incomplete, loadPath() not run");
   
    src->speed->set(1.0);
   
    src->len_slide->setValue(1);
}


void GWav::C_CHANGE::action(Component* co)
{
    DASSERTP(src->loaded,"obj incomplete, loadPath() not run");
   
    src->current[0]->reset();
    src->current[1]->reset();
   
    if(dir)
    {
        src->curr++;
        if(src->curr > src->nsamples-1)src->curr=src->nsamples-1;
    }
    else
    {
        src->curr--;
        if(src->curr < 0)src->curr=0;
    }
   
    src->updateCurrSamp();
   
}


GWav::C_PlayPause::C_PlayPause(GWav* s)
{
    src=s;
}
void GWav::C_PlayPause::action(Component* co)
{
    DASSERTP(src->loaded,"obj incomplete, loadPath() not run");
   
    bool on = src->current[0]->playing();
   
    for(int i=0;i<2;i++)
    {
        if(on)src->current[i]->pause();
        else src->current[i]->play();
    }
}

GWav::C_ResetPlay::C_ResetPlay(GWav* s)
{
    src=s;
}
void GWav::C_ResetPlay::action(Component* co)
{
    DASSERTP(src->loaded,"obj incomplete, loadPath() not run");
   
    for(int i=0;i<2;i++)
    {
        src->current[i]->reset();
    }
}


GWav::C_Loop::C_Loop(GWav* s)
{
    src=s;
}
void GWav::C_Loop::action(Component* co)
{
    DASSERTP(src->loaded,"obj incomplete, loadPath() not run");
   
    for(int i=0;i<2;i++)
    {
        src->current[i]->setLoop(src->b_loop->getValue());
    }
}


GWav::C_GMode::C_GMode(GWav* s)
{
    src=s;
}
void GWav::C_GMode::action(Component* co)
{
    DASSERTP(src->loaded,"obj incomplete, loadPath() not run");
   
   
    if(src->b_gmode->getValue())
        src->samples.setGateMode(SampleValue::SOFT_GATE);
    else
        src->samples.setGateMode(SampleValue::HARD_GATE);
}



/*******/
/*STATE*/
/*******/

void GWav::getState(SoundCompEvent* e)
{
    State s(len_slide->getValue(), b_gmode->getValue());
   
    e->copyData((char*)&s, sizeof(State));
}

void GWav::setState(SoundCompEvent* e)
{
    DASSERT(e!=NULL);
    if(e->empty())return;
    if(e->getSize()!=sizeof(State))
    {
        DERROR("size missmatch");
        DERROR("trying to read...");
    }
    const State* s = (const State*)e->getData();
   
    len_slide->setValue(s->speed);
    len_slide->mouseMoveAction();
   
    b_gmode->setValue(s->gmode);
    b_gmode->action();
   
    //updateCurrSamp();
}
