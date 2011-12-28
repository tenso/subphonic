#include "g_tracker.h" 

GTracker::GTracker(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    setStaySelected(true);
   
    //add eveything
    Pixmap* back_pix = new Pixmap(pix["840x800"]);
    add(back_pix,0,0);
    add(new MoveBorder(this, back_pix->pos.w,back_pix->pos.h),0,0);
   
   
    Label* l = new Label(fnt["label"]);
    l->printF("tracker");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    /*   int xoff=0;
         int yoff=0;*/
   
   
    int xoff=10;
    int yoff=25;
   
    //STEP
   
    l = new Label(f);
    l->printF("step");
    add(l, xoff, yoff);
   
    InputTaker* it = new InputTaker(pix["in"], this, 0);
    addInputTaker(it);
    add(it, xoff+10, yoff+15);
   
    //SYNC
    xoff+=50;
    l = new Label(f);
    l->printF("sync");
    add(l, xoff, yoff);
   
    it = new InputTaker(pix["in"], this, 1);
    addInputTaker(it);
    add(it, xoff, yoff+15);
   
    xoff+=20;
   
    OutputGiver* og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, xoff, yoff+15);
   
    xoff=10;
    yoff=TRACKER_PH;
   
    int yspc=15;
   
    l = new Label(f);
    l->printF("ch");
    add(l, xoff, yoff);
   
    l = new Label(f); 
    l->printF("hz");
    add(l, xoff, yoff+yspc);
   
    l = new Label(f);
    l->printF("gate");
    add(l, xoff, yoff+yspc*2);
   
    l = new Label(f);
    l->printF("gate pulse");
    add(l, xoff, yoff+yspc*3);
   
    l = new Label(f);
    l->printF("ctrl");
    add(l, xoff, yoff+yspc*4);
   
    xoff+=100;
    yspc-=1;
   
    //OUTPUTS
    for(uint i=0;i<MAX_CHAN;i++)
    {
        l = new Label(f);
        l->printF("%d", i);
        add(l, xoff, yoff);
	
        og = new OutputGiver(pix["out"],this, i*3+1); //note
        addOutputGiver(og);
        add(og, xoff, yoff+yspc);
	
        og = new OutputGiver(pix["out"],this, i*3+1+1); //gate
        addOutputGiver(og);
        add(og, xoff, yoff+yspc*2);
	
        //gate mode button
        b_gate_mode[i] = new Button(pix["but8x8off"],pix["but8x8on"]);
        b_gate_mode[i]->setPressed(false);
        b_gate_mode[i]->stayPressed(true);
        b_gate_mode[i]->setAction(new C_GateMode(this, i));
        add(b_gate_mode[i], xoff+1, yoff+yspc*3+2);
	
        og = new OutputGiver(pix["out"],this, i*3+2+1); //ctrl
        addOutputGiver(og);
        add(og, xoff, yoff+yspc*4);
	
        xoff+=30;
    }
   
   
    //tracker = new Tracker(font["black"]);
    tracker = new Tracker(fnt["track"]);
    add(tracker, 40,80);
   
    plen=128;
    ntracks=5;
    octave=4;
    ch=0;
   
    //-----------TOP ROW------------
   
    //play---------
    xoff=160;
    yoff=25;
   
    l = new Label(f);
    l->printF("play\n(space)");
    add(l, xoff-10, yoff);
   
    led_on = new LedsLevel<bool>(pix["led_on"], pix["led_off"]);
    led_on->setData(tracker->getPlayPtr());
    add(led_on, xoff+10, yoff+10);
   
    xoff=240;
    yoff=25;
   
    //pat.len----------
    l = new Label(f);
    l->printF("patt.len");
    add(l, xoff, yoff);
   
    xoff+=10;
   
    s_plen = new NumberSelect<int>(f, 4, 0, &plen);
    s_plen->setMin(1);
    s_plen->setAction(new C_UTRACKER(this));
    add(s_plen, xoff+20,yoff+12);
   
    l = new Label(f);
    l->printF("*2\n/2");
    add(l, xoff-18, yoff+13);
   
    Button* p_divmul = new Button(pix["uparrow_up"],pix["uparrow_down"]);
    p_divmul->stayPressed(false);
    p_divmul->setAction(new C_MulPLen(this));
    add(p_divmul, xoff, yoff+12);
   
    p_divmul = new Button(pix["downarrow_up"],pix["downarrow_down"]);
    p_divmul->stayPressed(false);
    p_divmul->setAction(new C_DivPLen(this));
    add(p_divmul, xoff, yoff+12+10); 
   
   
    xoff+=70;
      
    l = new Label(f);
    l->printF("n.tracks");
    add(l, xoff, yoff);
   
    //FIXME: num tracks >100 seems not good
    s_ntracks = new NumberSelect<int>(f, 2, 0, &ntracks);
    s_ntracks->setMin(1);
    s_ntracks->setMax(MAX_CHAN);
    s_ntracks->setAction(new C_UTRACKER(this));
    add(s_ntracks, xoff+20,yoff+12);
   
    xoff+=80;
   
    l = new Label(f);
    l->printF("oct");
    add(l, xoff, yoff);
   
    s_oct = new NumberSelect<int>(f, 1, 0, &octave);
    s_oct->setAction(new C_UTRACKER(this));
    add(s_oct, xoff+10,yoff+12);
   
   
    xoff+=60;
   
    l = new Label(f);
    l->printF("man. ch");
    add(l, xoff-10, yoff);
   
    s_ch = new NumberSelect<int>(f, 2, 0, &ch);
    s_ch->setMax(MAX_CHAN-1);
    s_ch->setAction(new C_UTRACKER(this));
    add(s_ch, xoff,yoff+12);
   
   
    b_ch = new Button(pix["up"],pix["down"]);
    b_ch->stayPressed(true);
    b_ch->setAction(new C_UTRACKER(this));
    add(b_ch, xoff+25, yoff+16);
   
    //--------RIGHT COLUMN---------
   
    xoff=TRACKER_PW;
    yoff=25;
      
    add_step=0;
    del_step=0;
   
    l = new Label(f);
    l->printF("addstep");
    add(l, xoff, yoff);
   
    ns_addstep = new NumberSelect<int>(f, 2, 0, &add_step, true);
    ns_addstep->setAction(new C_AddStep(this));
    add(ns_addstep, xoff+16, yoff+12);
   
    yoff+=45;
   
    l = new Label(f);
    l->printF("delstep");
    add(l, xoff, yoff);
   
    ns_delstep = new NumberSelect<int>(f, 2, 0, &del_step, true);
    ns_delstep->setAction(new C_DelStep(this));
    add(ns_delstep, xoff+16, yoff+12);
      
    tracker->setAddStep(add_step);
    tracker->setDelStep(del_step);
   
    updateTracker();
   
   
    markers_follow=true;
    tracker->setMarkersFollow(markers_follow);
   
   
    xoff=TRACKER_PW;
    yoff+=90;
    l = new Label(f);
    l->printF("follow");
    add(l, xoff, yoff);
   
    b_follow = new Button(pix["up"],pix["down"]);
    b_follow->stayPressed(true);
    b_follow->setPressed(markers_follow);
    b_follow->setAction(new C_Follow(this));
    add(b_follow, xoff+16, yoff+16);
   
    yoff+=40;
   
    l = new Label(f);
    l->printF("preview");
    add(l, xoff, yoff);
   
    do_preview=true;
    Button* b_preview = new Button(pix["up"],pix["down"]);
    b_preview->stayPressed(true);
    b_preview->setPressed(do_preview);
    b_preview->setAction(new C_Preview(this));
    add(b_preview, xoff+16, yoff+16);
   
    yoff+=40;
   
    l = new Label(f);
    l->printF("edit");
    add(l, xoff, yoff);
   
    b_edit = new Button(pix["up"],pix["down"]);
    b_edit->stayPressed(true);
    b_edit->setPressed(true);
    b_edit->setAction(new C_Edit(this));
    add(b_edit, xoff+16, yoff+16);
   
   
    //add to ground
    drain = new Drain(this);
    ground.addInput((Value**)&drain);
   

   
   
    /**************/
    /*EDIT BUTTONS*/
    /**************/
   
    /*tran*/
   
    xoff=TRACKER_PW;
    yoff=400;
   
    l = new Label(f);
    l->printF("transpose");
    add(l, xoff, yoff);
    yoff+=10;
   
    l = new Label(f);
    l->printF("track  all");
    add(l, xoff, yoff);
   
    yoff+=10;
    l = new Label(f);
    l->printF("1 12  1 12");
    add(l, xoff, yoff);
   
    Button* b_med = new Button(pix["uparrow_up"],pix["uparrow_down"]);
    b_med->stayPressed(false);
    b_med->setAction(new C_Tran(this, 1, 0/*0=single. 1=all*/));
    add(b_med, xoff+4, yoff+16);
   
    b_med = new Button(pix["downarrow_up"],pix["downarrow_down"]);
    b_med->stayPressed(false);
    b_med->setAction(new C_Tran(this, -1, 0/*0=single. 1=all*/));
    add(b_med, xoff+4, yoff+24);
   
    xoff+=15;
    b_med = new Button(pix["uparrow_up"],pix["uparrow_down"]);
    b_med->stayPressed(false);
    b_med->setAction(new C_Tran(this, 12, 0/*0=single. 1=all*/));
    add(b_med, xoff+4, yoff+16);
   
    b_med = new Button(pix["downarrow_up"],pix["downarrow_down"]);
    b_med->stayPressed(false);
    b_med->setAction(new C_Tran(this, -12, 0/*0=single. 1=all*/));
    add(b_med, xoff+4, yoff+24);
   
    xoff+=35;
   
    b_med = new Button(pix["uparrow_up"],pix["uparrow_down"]);
    b_med->stayPressed(false);
    b_med->setAction(new C_Tran(this, 1, 1/*0=single. 1=all*/));
    add(b_med, xoff+4, yoff+16);
   
    b_med = new Button(pix["downarrow_up"],pix["downarrow_down"]);
    b_med->stayPressed(false);
    b_med->setAction(new C_Tran(this, -1, 1/*0=single. 1=all*/));
    add(b_med, xoff+4, yoff+24);
   
    xoff+=15;
    b_med = new Button(pix["uparrow_up"],pix["uparrow_down"]);
    b_med->stayPressed(false);
    b_med->setAction(new C_Tran(this, 12, 1/*0=single. 1=all*/));
    add(b_med, xoff+4, yoff+16);
   
    b_med = new Button(pix["downarrow_up"],pix["downarrow_down"]);
    b_med->stayPressed(false);
    b_med->setAction(new C_Tran(this, -12, 1/*0=single. 1=all*/));
    add(b_med, xoff+4, yoff+24);
   
   
    /*clear*/
   
    xoff=TRACKER_PW;
    yoff=300;
   
    l = new Label(f);
    l->printF("clear");
    add(l, xoff+16, yoff);
    yoff+=10;
   
    l = new Label(f);
    l->printF("track  all");
    add(l, xoff, yoff);
   
    b_med = new Button(pix["up_ns"],pix["down_ns"]);
    b_med->stayPressed(false);
    b_med->setAction(new C_Clear(this, 0/*0=single. 1=all*/));
    add(b_med, xoff+8, yoff+16);
   
    xoff+=45;
   
    b_med = new Button(pix["up_ns"],pix["down_ns"]);
    b_med->stayPressed(false);
    b_med->setAction(new C_Clear(this, 1/*0=single. 1=all*/));
    add(b_med, xoff+8, yoff+16);
   
   
   
   
    /*****/
    /*REC*/
    /*****/
   
    xoff=TRACKER_PW;
    yoff=640;
    l = new Label(f);
    l->printF("rec\ngate/hz/ctr");
    add(l, xoff, yoff);
   
    it = new InputTaker(pix["in"], this, 2);
    addInputTaker(it);
    add(it, xoff+5+3, yoff+20);
   
    it = new InputTaker(pix["in"], this, 3);
    addInputTaker(it);
    add(it, xoff+35+3, yoff+20);
   
    it = new InputTaker(pix["in"], this, 4);
    addInputTaker(it);
    add(it, xoff+65+3, yoff+20);
   
   
    /*
      dd = new Label(f);
      dd->printF("--");
      add(dd, 0, 0);
      ndd=0;*/
   

}

GTracker::~GTracker()
{
    ground.remInput((Value**)&drain);
    delete drain;
}


bool GTracker::giveInput(Input& in)
{
    //same keys as traker_base::* works beause gui gives deepest comp input first
    bool handled=false;
   
    handled=tracker->giveInput(in);
    if(!handled)
    {
	
        if(in.keySet(SDLK_1, true))
        {
            int as = ns_addstep->getValue();
            if(as>-99)
            {
                ns_addstep->setValue(as-1);
                tracker->setAddStep(as-1);
            }
            handled=true;
        }
        if(in.keySet(SDLK_2, true))
        {
            int as = ns_addstep->getValue();
            if(as<99)
            {
                ns_addstep->setValue(as+1);
                tracker->setAddStep(as+1);
            }
            handled=true;
        }
        if(in.keySet(SDLK_3, true))
        {
            int as = ns_delstep->getValue();
            if(as>-99)
            {
                ns_delstep->setValue(as-1);
                tracker->setDelStep(as-1);
            }
            handled=true;
        }
        if(in.keySet(SDLK_4, true))
        {
            int as = ns_delstep->getValue();
            if(as<99)
            {
                ns_delstep->setValue(as+1);
                tracker->setDelStep(as+1);
            }
            handled=true;
        }
	
        if(in.keySet(SDLK_COMMA, true))
        {
            if(octave>0)
            {
                octave--;
                tracker->setOctave(octave);
                s_oct->setValue(octave);
                handled=true;
            }
	     
        }
        if(in.keySet(SDLK_PERIOD, true))
        {
            if(octave<9)
            {
                octave++;
                tracker->setOctave(octave);
                s_oct->setValue(octave);
                handled=true;
            }
	     
        }
    }
   
    //if(!handled)handled=tracker->giveInput(in);
   
    return handled;
}


void GTracker::updateTracker()
{
    //it is ok to set things that have not changed, minimal overhead
    tracker->setNRows(plen);
    tracker->setNViewRows(TRACKER_N_ROW_VIS);
   
    tracker->setNTracks(ntracks);
    tracker->setNViewTracks(TRACKER_N_TRACKS_VIS);
   
    tracker->setOctave(octave);
   
    if(b_ch->isPressed())tracker->setDefChannel(ch);
    else tracker->setDefChannel(-1);
}


Value** GTracker::getOutput(unsigned int id)
{
    if(id==0) //sync
    {
        return drain->getSync();
    }
    else
    {
        int ch = (id-1)/3;
	
        if( (id-1)%3==0) return drain->getNote(ch);
        else if((id-1)%3==1)return drain->getGate(ch);
        else return drain->getCtrl(ch);
    }
   
    DERROR("err");
    return NULL; //ERROR
}

void GTracker::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0) //step
    {
        drain->setStep(out);
    }
    if(fromid==1) //sync
    {
        drain->setSync(out);
    }
    if(fromid==2)
    {
        drain->setRecGate(out);
    }
    if(fromid==3)
    {
        drain->setRecHz(out);
    }
    if(fromid==4)
    {
        drain->setRecCtrl(out);
    }
}

void GTracker::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0) //step
    {
        drain->setStep(NULL);
    }
    if(fromid==1) //sync
    {
        drain->setSync(NULL);
    }
    if(fromid==2)
    {
        drain->setRecGate(NULL);
    }
    if(fromid==3)
    {
        drain->setRecHz(NULL);
    }
    if(fromid==4)
    {
        drain->setRecCtrl(NULL);
    }
}


int GTracker::syncToIndex(uint index)
{
    //ndd=0;
   
    if(markers_follow)
    {
        tracker->setSelRow(0);
        tracker->setPlayRow(0);
        return tracker->getSelRow();
    }
    else
    {
        tracker->setPlayRow(0);
        return tracker->getPlayRow();
    }
    ERROR("never here!");
    return -1;
}


/***********/
/*CALLBACKS*/
/***********/

GTracker::C_UTRACKER::C_UTRACKER(GTracker* src)
{
    this->src=src;
}

void GTracker::C_UTRACKER::action(Component* c)
{
    src->updateTracker();
}


GTracker::C_MulPLen::C_MulPLen(GTracker* src)
{
    this->src=src;
}

void GTracker::C_MulPLen::action(Component* c)
{
    src->plen*=2;
    src->s_plen->setValue(src->plen);
    src->updateTracker();
}


GTracker::C_DivPLen::C_DivPLen(GTracker* src)
{
    this->src=src;
}

void GTracker::C_DivPLen::action(Component* c)
{
    src->plen/=2;
    src->s_plen->setValue(src->plen);
    src->updateTracker();
}


GTracker::C_AddStep::C_AddStep(GTracker* src)
{
    this->src=src;
}

void GTracker::C_AddStep::action(Component* c)
{
    src->tracker->setAddStep(src->ns_addstep->getValue());
}

GTracker::C_DelStep::C_DelStep(GTracker* src)
{
    this->src=src;
}

void GTracker::C_DelStep::action(Component* c)
{
    src->tracker->setDelStep(src->ns_delstep->getValue());
}

GTracker::C_GateMode::C_GateMode(GTracker* src, uint i)
{
    this->i = i;
    this->src=src;
}

void GTracker::C_GateMode::action(Component* c)
{
    src->drain->setGateMode(i, !src->b_gate_mode[i]->getValue());
}


GTracker::C_Edit::C_Edit(GTracker* src)
{
    this->src=src;
}

void GTracker::C_Edit::action(Component* c)
{
    src->tracker->setOnlyPreview(!src->b_edit->getValue());
}

GTracker::C_Follow::C_Follow(GTracker* src)
{
    this->src=src;
}

void GTracker::C_Follow::action(Component* c)
{
    src->markers_follow=((Button*)c)->getValue();
    src->tracker->setMarkersFollow(src->markers_follow);
   
    if(src->markers_follow)
    {
        src->tracker->setSelRow(src->tracker->getPlayRow());
    }
   
}

GTracker::C_Tran::C_Tran(GTracker* src, int dir, bool all)
{
    this->src=src;
    this->dir=dir;
    this->all=all;
}

void GTracker::C_Tran::action(Component* c)
{
    if(!all)
    {
        uint ctrack = src->tracker->getSelTrack();
        src->tracker->transposeTrackNotes(ctrack, dir);
    }
    else
    {
        src->tracker->transposeAllNotes(dir);
    }
}


GTracker::C_Clear::C_Clear(GTracker* src, bool all)
{
    this->src=src;
    this->all=all;
}

void GTracker::C_Clear::action(Component* c)
{
    if(!all)
    {
        uint ctrack = src->tracker->getSelTrack();
        src->tracker->clearTrackEntries(ctrack);
    }
    else
    {
        src->tracker->clearAllEntries();
	
    }
}

GTracker::C_Preview::C_Preview(GTracker* src)
{
    this->src=src;
}

void GTracker::C_Preview::action(Component* c)
{
    src->do_preview=((Button*)c)->getValue();
}


/*******/
/*STATE*/
/*******/

void GTracker::getState(SoundCompEvent* e)
{
    DASSERT(e!=NULL);
   
    //HERE: partial copy data in head.from_row, etc
    //FIXME: partial row copy: copy starts from marker
    if(e->getWantPartial())
    {
        //still get all tracks(somewhat wastefull...)
        e->setIsPartial(true);
        e->setPartialNo(tracker->getSelTrack());
    }
   
   
    uint size = sizeof(StateHead);
   
    vector<TrackerEntry> ent;
    tracker->getEntries(ent);
   
    ubyte* ent_data=NULL;
    uint ent_size = sizeof(TrackerEntry)*ent.size();
   
    if(ent.size()>0)
    {
        ent_data = new ubyte[ent_size];
	
        ubyte* ent_data_off=ent_data;
        for(uint i=0;i<ent.size();i++)
        {
            memcpy(ent_data_off, (ubyte*)&ent[i], sizeof(TrackerEntry));
            ent_data_off+=sizeof(TrackerEntry);
        }
	
        size+=ent.size()*sizeof(TrackerEntry);
    }
   
    size_t mode_size = MAX_CHAN*sizeof(bool);
    size+=mode_size;
   
    char* data = new char[size];
   
    StateHead head;
    head.head_size=sizeof(StateHead);
   
    head.ch = ch;
    //disable this:
    head.octave = 0;//octave;
    head.plen = plen;
    head.ntracks = ntracks;
    head.ch_on=b_ch->getValue();
    //disable this:
    head.edit=false;//b_edit->getValue();
   
    head.play_on=tracker->getPlay();
   
    head.entry_size=sizeof(TrackerEntry);
    head.nentry=ent.size();
   
    head.num_gate_mode=MAX_CHAN;
   
    //disable:
    head.add_step=0;//ns_addstep->getValue();
    head.del_step=0;//ns_delstep->getValue();
   
    head.total_size = sizeof(StateHead)+ent_size+mode_size;
   
    memcpy(data, (char*)&head, sizeof(StateHead));
   
    if(ent.size()>0)
    {
        memcpy(data+sizeof(StateHead), ent_data, ent_size);
        delete[] ent_data;
    }
   
    if(head.num_gate_mode > 0)
    {
        bool* mode_data = new bool[mode_size];
	
        for(uint i=0;i<MAX_CHAN;i++)mode_data[i] = b_gate_mode[i]->getValue();
	
        memcpy( data+sizeof(StateHead)+ent_size, mode_data, mode_size);
	
        delete[] mode_data;
    }
   
    //set return
    e->copyData(data, size);
   
    delete[] data;
}

void GTracker::setState(SoundCompEvent* e)
{
    if(e->empty())return;
   
    const char* data = e->getData();
   
    size_t read=0;
   
    StateHead* head = (StateHead*)data;
   
    if(head->head_size!=sizeof(StateHead))
    {
        DERROR("head size, continue...");
    }
   
    read+=sizeof(StateHead);
   
   
    if(e->getIsPartial())
    {
        //size missmatch should NOT happen: partial only in program
	
        //extract wanted track's data, leave rest
        //data is sorted
	
        if(e->getPartialNo()<0)
        {
            DERROR("data corrupt");
            return;
        }
	
	
        uint want_t = (uint)e->getPartialNo();
	
	
        TrackerEntry* ent_p = (TrackerEntry*)( (ubyte*)data+sizeof(StateHead));
	
        vector<TrackerEntry> extracted;
	
        uint row_off=tracker->getSelRow();
	
        for(int i=0;i < head->nentry; i++,ent_p++)
        {
            if(ent_p->track>want_t)break; //done
            if(ent_p->track<want_t)continue; //not wanted
	     
	     
            TrackerEntry ent = *ent_p;
            ent.row += row_off;
	     
            if(ent.row>=tracker->getNRows())break;
	     
            ent.track = tracker->getSelTrack();
	     
            extracted.push_back(ent);
        }
	
        if(plen<extracted.size())
        {
            extracted.resize(plen); //drop data
        }
	
        tracker->setEntries(extracted);
	
        return;
    }
   
    ch=head->ch;
    s_ch->setValue(ch);
   
    //disable:
    /*octave=head->octave;
      s_oct->setValue(octave);*/
   
    ntracks = head->ntracks;
    s_ntracks->setValue(ntracks);
    plen = head->plen;
    s_plen->setValue(plen);
   
    b_ch->setValue(head->ch_on);
   
    //disable this
    //b_edit->setValue(head->edit);
    //tracker->setOnlyPreview(!b_edit->getValue());
      
   
    //disable:
    /*ns_addstep->setValue(head->add_step);
      tracker->setAddStep(ns_addstep->getValue());
      ns_delstep->setValue(head->del_step);
      tracker->setDelStep(ns_delstep->getValue());*/
   
    tracker->setPlay( head->play_on>0 ); //it have happend that bool have value!=true/false
   
    //must always clear
    tracker->clearAllEntries();
   
    updateTracker(); //set needed tracks/rows etc before load
   
   
    //READ TRACK DATA
   
    if(head->nentry<0)
    {
        DERROR("nentry<0");
        cout << "(E) data error" << endl;
        return;
    }
   
    if(head->nentry>0)
    {
        uint file_sz=head->entry_size;
        uint want_sz=sizeof(TrackerEntry);
	
        if(file_sz != want_sz)
        {
            DERROR("entry size");
            cout << "(E) data error" << endl;
            //return;
        }
	
        //read entrys
        TrackerEntry empty;
        empty.zero();
        vector<TrackerEntry> ent(head->nentry, empty);
	
        bool force=true;
	
        if(file_sz==want_sz)
        {
            TrackerEntry* ent_p = (TrackerEntry*)( (ubyte*)data+sizeof(StateHead));
	     
            for(uint i=0;i<ent.size();i++)
            {
                ent[i]=*ent_p;
                ent_p++;
		  
                read+=sizeof(TrackerEntry);
            }
        }
        else if(force)
        {
            //DERROR("forcing read");
            ERROR("forcing read");
	     
            ubyte* edata = (ubyte*)(data+sizeof(StateHead));
	     
            uint do_sz = (want_sz>file_sz) ? file_sz : want_sz;
	     
	     
            for(uint i=0;i<ent.size();i++)
            {
                memcpy(&ent[i], edata, do_sz);
                edata+=file_sz;
                //REMOVEME
                ent[i].have_note=true;
		  
                read+=file_sz;
            }
	     
        }
        tracker->setEntries(ent);
    }
   
    //READ gate_mode(s)
    if(progstate.getCurrentLoadVersion()<4)return;
   
   
    if(head->num_gate_mode>0)
    {
        DASSERT(read+head->num_gate_mode*sizeof(bool) == head->total_size);
	
        if(read+head->num_gate_mode*sizeof(bool) > head->total_size)return;
	
        bool* edata = (bool*)(data+read);
	
        //BUGG: funny stuff here: can't input MAX_CHAN directly: something with minOf template?
        uint m = MAX_CHAN;
        uint min = minOf(head->num_gate_mode, m);
        //uint min = minOf(head->num_gate_mode, MAX_CHAN);
	
        for(uint i=0;i<min;i++)
        {
            b_gate_mode[i]->setValue(edata[i]);
            drain->setGateMode(i, !b_gate_mode[i]->getValue());
        }
    }
}



/*******/
/*DRAIN*/
/*******/

GTracker::Drain::Drain(GTracker* src)
{
    this->src=src;
   
    dir=1;
   
    resync=true;
    restep=true;
   
    i_sync=NULL;
    i_step=NULL;
   
    for(uint i=0;i<MAX_CHAN;i++) 
    {
        o_note[i] = new Const(0);
        o_gate[i] = new Gate();
        o_ctrl[i] = new Const(0);
    }
   
    o_sync=new OneShot();
   
    sync_wait=0;
   
    preview_gate=false;
    preview_gate_ch=0;
   
    rec_gate=NULL;
    rec_hz=NULL;
    rec_ctrl=NULL;
    last_rec_hz=0;
    last_rec_ctrl=0;
    last_rec_row=0;
   
    rec_ch=0;
}

GTracker::Drain::~Drain()
{
    delete o_sync;
   
    for(uint i=0;i<MAX_CHAN;i++)
    {
        delete o_note[i];
        delete o_gate[i];
        delete o_ctrl[i];
    }
}


void GTracker::Drain::setRecGate(Value** v)
{
    rec_gate=v;
}

void GTracker::Drain::setRecHz(Value** v)
{
    rec_hz=v;
}

void GTracker::Drain::setRecCtrl(Value** v)
{
    rec_ctrl=v;
}


void GTracker::Drain::setStep(Value** v)
{
    i_step=v;
}


void GTracker::Drain::setSync(Value** v)
{
    i_sync=v;
}

Value** GTracker::Drain::getSync()
{
    return (Value**)&o_sync;
}


Value** GTracker::Drain::getCtrl(uint chan)
{
    DASSERT(chan<MAX_CHAN);
    if(chan>=MAX_CHAN)return NULL;
   
    return (Value**)&o_ctrl[chan];
}

Value** GTracker::Drain::getNote(uint chan)
{
    DASSERT(chan<MAX_CHAN);
    if(chan>=MAX_CHAN)return NULL;
   
    return (Value**)&o_note[chan];
}


Value** GTracker::Drain::getGate(uint chan)
{
    DASSERT(chan<MAX_CHAN);
    if(chan>=MAX_CHAN)return NULL;
   
    return (Value**)&o_gate[chan];
}

void GTracker::Drain::setGateMode(uint chan, bool sust)
{
    DASSERT(chan<MAX_CHAN);
    if(chan>=MAX_CHAN)return;
   
    o_gate[chan]->setOneShot(!sust);
}


smp_t GTracker::Drain::nextValue()
{
    //src->ndd++;
    //src->dd->printF("%d",src->ndd);
   
    //SYNC
    smp_t sy = 0;
    if(i_sync!=NULL)
    {
        sy = (*i_sync)->nextValue();
    }
   
   
    //STEP
    smp_t stp=0;
    if(i_step != NULL)
    {
        stp = (*i_step)->nextValue();
    }
   
    if(sync_wait>0)
    {
        sync_wait--;
        return 0;
    } //return for wainting after sync
   
    o_sync->unset(); //even if not drained it should reset
   
    if(src->tracker->getPlay())
    {
        if(stp > GATE_ZERO_LVL)
        {
            if(restep)
            {
                Tracker::POS p = src->tracker->playRowPos();
                if(dir==1)
                {
                    if(p==Tracker::END)
                    {
                        src->tracker->setPlayRow(0);
                        if(src->markers_follow)src->tracker->setSelRow(0);
			    
                        o_sync->set();
                        sync_wait = GCOMP_WAIT_SMPL_AFTER_SYNCOUT-1;
                        return 0;
                    }
                    else 
                    {
                        src->tracker->stepPlayRow(dir);
                        if(src->markers_follow)src->tracker->stepSelRow(dir);
                    }
		       
                } 
                else
                {
                    if(p==Tracker::BEG)
                    {
                        src->tracker->setPlayRow(src->tracker->getNRows()-1);
                        if(src->markers_follow)src->tracker->setSelRow(src->tracker->getNRows()-1);
			    
                        o_sync->set();
                        sync_wait = GCOMP_WAIT_SMPL_AFTER_SYNCOUT-1;
                        return 0;
                    }
                    else 
                    {
                        src->tracker->stepPlayRow(dir);
                        if(src->markers_follow)src->tracker->stepSelRow(dir);
                    }
		       
                }
                restep=false;
            }
        }
        else restep=true;
    }
   
    //sync after step: see g_punchpad.cpp
    if(sy > GATE_ZERO_LVL)
    {
        if(resync)
        {
            src->tracker->setPlayRow(0);
            if(src->markers_follow)
            {
                src->tracker->setSelRow(0);
            }
	     
            resync=false;
        }
        sync_wait=0;
    }
    else resync=true;
   
   
    //must kill preview gate(will be turned on later if still needed)
    if(preview_gate)
    {
        preview_gate=false;
        o_gate[preview_gate_ch]->setOn(false);
    }
   
   
    if(src->tracker->getPlay())
    {
        //it is enough that one chan is on, many=> highest tracknum is on
        for(uint i=0;i<src->tracker->getNTracks();i++)
        {
            if(src->tracker->playHaveUpdate(i))
            {
                src->tracker->playFlagReadUpdate(i);
		  
                //Remember: selGet.. is used to get the selected row's data(cached)
                //this is very good optimization: ~ 6-7 times faster, beacuse this is done so often
		  
                int ch = src->tracker->playGetCh(i);
                DASSERTP(ch<MAX_CHAN, "ch: " + ch);
		  
		  
                if(src->tracker->playHaveNote(i))
                {
                    //cout << "ch: " << ch << endl;
		       
                    //set hz 
                    int key = src->tracker->playGetNote(i);
		       
		       
                    //cout << "key: " << noteStr(key) << endl;
		       
                    double hz = noteMap(key);
                    o_note[ch]->set(hz);
		       
                    //set gate
                    o_gate[ch]->setOn(true);
                }
                else o_gate[ch]->setOn(false);
		  
                if(src->tracker->playHaveCtrl(i))
                {
                    smp_t ctrl = src->tracker->playGetCtrlDouble(i);
                    o_ctrl[ch]->set(ctrl);
		       
                    //cout << "ctrl:  " << ctrl << endl;
                }
            }
        }
    }
    else //optimization here: if(num_gates_on>0)
    {
        //play is off, some gates might have been on when stopping play, off all gates:
        //keep notes and ctrl as is...
        for(uint i=0;i<MAX_CHAN;i++)
        {
            o_gate[i]->setOn(false);
        }
	
    }
   
    if(src->do_preview)
    {
	
        //FIXME: this way preview is updated every frame
        //sound preview? (overwrites all other out on this ch)
        if(src->tracker->havePreview())
        {
            //preview is always atleast a note
            int key = src->tracker->getPreviewNote();
            double hz = noteMap(key);
	     
            uint ch = src->tracker->getPreviewCh();
            DASSERTP(ch<MAX_CHAN, "ch: " + ch);
	     
            //also ctrl?
            if(src->tracker->havePreviewCtrl())
            {
                double ctrl = src->tracker->getPreviewCtrl();
                o_ctrl[ch]->set(ctrl);
            }
	     
            o_note[ch]->set(hz);
            o_gate[ch]->setOn(true);
            preview_gate=true;
            preview_gate_ch=ch;
        }
    }
   
   
   
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
   
    //WARNING: rec follows play marker for now, using selection track(col)
    if(rec_ctrl!=NULL)
    {
        smp_t r_ctrl=0;
        r_ctrl = (*rec_ctrl)->nextValue();
	
        //beacause tracker only changes output with ctrl i.e not per rw basis
        //this is better:
        if(!feq(r_ctrl, last_rec_ctrl)/* || last_rec_row != src->tracker->getSelRow()*/)
        {
            uint set_track = src->tracker->getSelTrack();
            uint set_row = src->tracker->getPlayRow();
	     
            last_rec_row=src->tracker->getPlayRow();
            last_rec_ctrl=r_ctrl;
            src->tracker->setCtrl(set_track, set_row, toStr(r_ctrl));
	     
            DASSERTP(set_track<MAX_CHAN, "set_track: " + set_track);
            o_ctrl[set_track]->set(src->tracker->playGetCtrlDouble(set_track));
        }
	
    }
   
   
    if(r_gate>GATE_ZERO_LVL)
    {
        if(!feq(last_rec_hz, r_hz) || last_rec_row != src->tracker->getPlayRow())
        {
            last_rec_hz = r_hz;
            last_rec_row=src->tracker->getPlayRow();
	     
	     
            int key = hzNote(r_hz); //FIXME: punchpad does not use standard! -1
	     
            //REMOVEME: heavy debug(only for exact hz)
            //DASSERT( feq(r_hz, noteHz(key)) );
	     
            if(key<0)key=0;
	     
            /*
              int oct = key/12;
              key -= 12*oct;*/
	     
            uint set_track = src->tracker->getSelTrack();
            uint set_row = src->tracker->getPlayRow();
	     
	     
            int set_ch = src->tracker->getDefChannel();
            if(set_ch<0)set_ch = set_track;
            DASSERTP(set_ch<MAX_CHAN, "set_ch: " + set_ch);
	     
            src->tracker->setCh(set_track, set_row, set_ch);
            src->tracker->setNote(set_track, set_row, key);
	     
            double hz = noteMap(key);
            o_note[set_ch]->set(hz);
	     
            rec_ch=set_ch; //WARNING: this means that chan changes ONLY after key RElEASE
        }
	
        //always set gate when record is on
        o_gate[rec_ch]->setOn(true);
	
    }
   
    return 0;
}

void GTracker::Drain::setDir(int d)
{
    if(d==0)return;
   
    if(d>0)d=1;
    else d=-1;
   
    dir=d;
}

int GTracker::Drain::getDir()
{
    return dir;
}
