#include "g_bridge.h" 

const string GBridge::deflabel("");

/*when this is added it works as a bridge to another module,
 * when that module uses moduleload this comp is found and converted(IO flip) and only this is shown*/

/*INPUT IN "MODULE" BECOMES AN OUT WHEN FLIPPED AND VICEVERSA*/

GBridge::GBridge(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    this->f=f;
   
    flip=STANDARD;
    want_flip=false;
   
    midi_hooks_rem=false;
   
    cont=NULL;
    conn=NULL;
    states=NULL;
   
    filename = "";
   
    //background
    back_pix = new Pixmap( NULL );
    add(back_pix, 0,0);
   
    move_border = new MoveBorder(this, 0, 0);
    add(move_border, 0,0);
   
    title = new Label(fnt["label"]);
    title->setMaxChar( (COMP_W-20)/8);
   
    title->printF("modbridge");
    add(title, 10, GCOMP_LABEL_YOFF);
   

   
    //FIXME: these maybe auto calced
    out_xoff=30+COMP_W-176-8; //add here if adding label chars(i.e the space inbetween in and out(start))
    port_yoff=50;
   
    int extra_x=16;
    norm_out_xpush=extra_x;
    flip_label_xpush=norm_out_xpush;

   
    //build io
    for(uint i=0; i<STD_NUM_IO; i++)
    {
        installPort();
    }
   
    setBackSize();
   
    nport=STD_NUM_IO;
   
    nportsel = new NumberSelect<uint>(f, 2, 0, &nport);
    nportsel->setAction(new C_NumPort(this));
    add(nportsel, 90, 20);
   
    nportlab = new Label(f);
    nportlab->printF("num ports");
    add(nportlab, 10, 30);
   
    updateLabels();
}

void GBridge::setBackSize()
{
    pix_w = COMP_W;
    pix_h = port_yoff + ports.size()*18+20;
   
    if(!isFlipped())pix_h+=20; //make space for num_port sel
   
    back_pix->change(pix.getBG(pix_w, pix_h));
   
    move_border->setSize(pix_w, pix_h);
   
    recalcSize();
}


GBridge::~GBridge()
{
    //might not do this: a reset would not require it
    //DASSERTP(midi_hooks_rem,"midi hooks not removed");
   
    while(ports.size()!=0)
    {
        removeLastPort();
    }
   
   
    //MUST delete states before cont otherwise there are no comps to free the states
    //~SCState uses parent -(points into)-> cont
    delete states;
   
    delete conn;
    delete cont;
}

int GBridge::syncToIndex(unsigned int index)
{
    if(flip==STANDARD)return 0;
    DASSERT(cont!=NULL);
      
    comp_list* list = cont->getList();
   
    for(comp_list_it it = list->begin();it!=list->end();it++)
    {
        SoundComp* sct = (SoundComp*)*it;
        sct->syncToIndex(index);
    }
    return 0;
}


void GBridge::installPort()
{
    uint id = ports.size();
   
    IOEntry* ent = new IOEntry();
    ports.push_back(ent);
   
    ent->id=id;
    ent->have_out=false;
    ent->have_in=false;
   
    ent->std_uses_in=false;
    ent->std_uses_out=false;
   
    //CMODE, knob is def
    ent->cmode=IOEntry::KNOB;
   
    ent->knob = new KnobSlider(pix["knob16x16"]);
    ent->knob->setMouseMoveAction(new C_PortVal(this, id));
    ent->knob->setValue(0);
    int yadd=18;
    add(ent->knob, 90+out_xoff, port_yoff+id*yadd);
   
   
    ent->sel_c = ent->knob;
    ent->out_val = new Const(ent->knob->getValue());
   
    ent->forward = new ValueForward();
    ent->forward->setInput((Value**)&ent->out_val);
   
    ent->button_onoff = new Button(pix["up"], pix["down"]);
    ent->button_onoff->setStayPressed(true);
    ent->button_onoff->setPressed(false);
    ent->button_onoff->setAction(new C_PortVal(this, id));
   
    ent->no_comp = new Component();
   
    ent->ns_int2 = new NumberSelect<int>(f, 4,0, NULL, true);
    ent->ns_int2->setValue(0);
    ent->ns_int2->setAction(new C_PortVal(this, id)); 
   
    ent->sel_mode = new Button(pix["uparrow_up"],pix["uparrow_down"]);
    ent->sel_mode->setStayPressed(false);
    ent->sel_mode->setPressed(false);
    ent->sel_mode->setAction(new C_CyclePortMode(this, id));
    add(ent->sel_mode, 80+out_xoff, port_yoff+id*yadd);
   
    //add inputs
    ent->it = new InputTaker(pix["in"], this, id);
    addInputTaker(ent->it);
    add(ent->it, 8, port_yoff+2+id*yadd);
   
    //only have knobs on output for !flipped and hence input for flipped
    ent->in=(Value**)(&empty);
   
    //add output
    ent->og = new OutputGiver(pix["out"],this, id);
    addOutputGiver(ent->og);
    add(ent->og, 120+out_xoff+norm_out_xpush, port_yoff+2+id*yadd);
   
    //labels
    ent->name = deflabel;
    ent->label = new Label(f);
    add(ent->label, 26, port_yoff+5+id*yadd);
}

bool GBridge::removeLastPort()
{
    if(ports.size()==0)
    {
        DERROR("ports empty");
        return false;
    }
   
    IOEntry* ent = ports.back();
    ports.pop_back();
   
    //might have been removed already: e.g when flip
    if(ent->it!=NULL)
    {
        remInputTaker(ent->it);
        rem(ent->it);
        delete ent->it;
    }
   
    if(ent->og!=NULL)
    {
        remOutputGiver(ent->og);
        rem(ent->og);
        delete ent->og;
    }
   
   
    if(ent->sel_mode!=NULL)
    {
        rem(ent->sel_mode);
        delete ent->sel_mode;
        ent->sel_mode=NULL;
    }
   
   
    //must manually delete all GUI comp's: some are not in container(i.e no auto delete), do all to be sure
   
    if(ent->sel_c!=NULL)
    {
        rem(ent->sel_c);
    }
   
   
    /*if(ent->cmode==IOEntry::NONE)rem(ent->no_comp);
      else if(ent->cmode==IOEntry::KNOB)rem(ent->knob);
      else if(ent->cmode==IOEntry::BUTTON_ONOFF)rem(ent->button_onoff);
      else DERROR("no such cmode");*/
   
    delete ent->no_comp;
    ent->no_comp=NULL;
   
    delete ent->knob;
    ent->knob=NULL;
   
    delete ent->button_onoff;
    ent->button_onoff=NULL;
   
    delete ent->ns_int2;
    ent->ns_int2=NULL;
   
   
    if(ent->label!=NULL)
    {
        rem(ent->label);
        delete ent->label;
    }
   
    //ok if NULL
    delete ent->forward;
    delete ent->out_val;
   
    delete ent;
   
    return true;
}


bool GBridge::setLabel(const string& s, uint i)
{
    if(i >= ports.size())
    {
        return false;
    }
   
    int m = (s.length() < LABEL_NCHAR) ? s.length() : LABEL_NCHAR;
    ports[i]->name=string(s, 0, m);
   
    updateLabels();
   
    return true; 
}

void GBridge::updateLabels()
{
    for(uint i=0;i<ports.size();i++)
    {
        ports[i]->label->printF("%s", ports[i]->name.c_str());
    }
}

int GBridge::remAllMidiHooks(MIDIcall* midicall)
{
    midi_hooks_rem=true;
   
    if(cont==NULL)
    {
        /*this is not an error, if bridge for some reason is removed from a container
          before it is "loaded" pf ends up here"*/
	
        //DERROR("no container!");
        return 0;
    }
   
    //loop trhrough all comps owned by this removing all its hooks to
    //midi and also all containing bridges hooks
    comp_list* list = cont->getList();
    comp_list_it it;
    SoundComp* sct;
   
    int ret=0;
   
    for(it=list->begin();it!=list->end();it++)
    {
        sct = (SoundComp*)*it;
        if(sct->getSCType()==G_MIDIKEY)
        {
            VERBOSE1(cout << "MB: rem key" << endl;);
	     
            bool noerr = midicall->remKey((GMIDIkey*)sct);
            DASSERT(noerr);
	     
            ret++;
        }
        if(sct->getSCType()==G_MIDICTRL)
        {
            VERBOSE1(cout << "MB: rem ctrl" << endl;);
	     
            bool noerr = midicall->remCtrl((GMIDIctrl*)sct);
            DASSERT(noerr);
	     
            ret++;
        }
        if(sct->getSCType()==G_MIDIPGM)
        {
            VERBOSE1(cout << "MB: rem pgm" << endl;);
	     
            bool noerr = midicall->remPgm((GMIDIpgm*)sct);
            DASSERT(noerr);
	     
            ret++;
        }
        if(sct->getSCType()==G_BRIDGE)
        {
            VERBOSE1(cout << "MB: rem recurse bridge" << endl;);
	     
            ret += ((GBridge*)sct)->remAllMidiHooks(midicall);
	     
        }
    }
   
    return ret;
}


Value** GBridge::getOutput(unsigned int id)
{
    DASSERT(id<ports.size());
   
    ports[id]->have_out=true;
   
    if(flip==STANDARD)
    {
        ports[id]->std_uses_out=true;
        return (Value**)&ports[id]->forward;
    }
    else if(flip==LOADED_MODULE)
    {
        DASSERT(ports[id]->og!=NULL);
	
        //if comp has been flipped that means that the inputs to the unflipped now must be the outputs
        return ports[id]->in;
    }
   
    return NULL;
}

void GBridge::remOutput(Value** o, unsigned int id)
{
    DASSERT(id<ports.size());
   
    if(flip==STANDARD)
    {
        ports[id]->std_uses_out=false;
    }
   
    ports[id]->have_out=false;
}

void GBridge::addInput(Value** o, unsigned int id)
{
    DASSERT(id<ports.size());
   
    ports[id]->have_in=true;
   
    if(flip==STANDARD)
    {
        DASSERT(ports[id]->it != NULL);
	
        ports[id]->std_uses_in = true;
        ports[id]->in = o;
    }
    else if(flip==LOADED_MODULE)
    {
        ports[id]->forward->setInput(o);
    }
}

void GBridge::remInput(Value** o, unsigned int id)
{
    if(flip==STANDARD)
    {
        DASSERT(ports[id]->it != NULL);
	
        ports[id]->std_uses_in=false;
        ports[id]->in = &empty;
    }
    else if(flip==LOADED_MODULE)
    {
        DASSERT(ports[id]->out_val != NULL);
	
        ports[id]->forward->setInput( (Value**)&ports[id]->out_val );
    }
   
    ports[id]->have_in=false;
}

void GBridge::flipComp(const string name) //oneway action
{
    want_flip=false;
    flip=LOADED_MODULE;
    filename.assign(name);
    string strip = name.substr(0,name.find_last_of("."));
    title->printF("%s", strip.c_str());
   
    //remake ports to LOADED_MODULE mode
    //unused ports are still kept until ~GBridge()
    for(uint i=0;i<ports.size();i++)
    {
        //reusing same inputtaker/outgiver now when flipped.
        //this is the "reset": (taker/giver should no longer update position
        //of the "old" line(it is not visible)
        getInputTaker(i)->remLine();
        getOutputGiver(i)->remLine();
	
        //remove cmode selector
        rem(ports[i]->sel_mode);
        delete ports[i]->sel_mode;
        ports[i]->sel_mode=NULL;
	
        //move in vals selectors to other side: out is now in...
        ports[i]->label->relMove(flip_label_xpush+30, 0);
        ports[i]->knob->relMove(-65-out_xoff, 0);
        ports[i]->button_onoff->relMove(-65-out_xoff, 0);
        ports[i]->ns_int2->relMove(-65-out_xoff, 0);
        ports[i]->no_comp->relMove(-65-out_xoff, 0); //maybe unnessesary
	
        if(ports[i]->std_uses_out)
        {
        }
        else
        {
            //dont use out val:
            rem(ports[i]->sel_c);
            ports[i]->sel_c=NULL;
	     
            remInputTaker(ports[i]->it);
            rem(ports[i]->it);
            delete ports[i]->it;
            ports[i]->it=NULL;
        }
	
        if(ports[i]->std_uses_in)
        {
        }
        else
        {
            remOutputGiver(ports[i]->og);
            rem(ports[i]->og);
            delete ports[i]->og;
            ports[i]->og=NULL;
        }
	
        //remove?
        /*  rem(ports[i]->knob);
            delete ports[i]->knob; //wont be auto deleted if removed
            ports[i]->knob=NULL;*/
    }
   
    //remove num port selection
    rem(nportsel);
    delete nportsel;
   
    rem(nportlab);
    delete nportlab;
   
    //recalc size
    setBackSize();
}


bool GBridge::isFlipped()
{
    return flip;
}

bool GBridge::wantFlip()
{
    return want_flip;
}

string GBridge::getFilename()
{
    return filename;
}
void GBridge::setContent(Container* cont, Connection* conn, BenchStates* states)
{
    this->cont=cont;
    this->conn=conn;
    this->states=states;
}

bool GBridge::setNumPorts(uint n)
{
    DASSERT(!isFlipped()); //WHY? : 
   
    if(ports.size()==n)return true;
   
    while(ports.size()<n)
    {
        installPort();
    }
    while(ports.size()>n)
    {
        //removing when have in/output: problem
        //  some comp depends on this port; can't just remove.
        //  So: prevent that and make user disconnect port before allowing it
        // 
        if(ports.back()->have_out || ports.back()->have_in)
        {
            nportsel->setValue(ports.size());
            setBackSize();
            return false;
        }
	
        removeLastPort();
    }
   
    //need to update sel
    nportsel->setValue(ports.size());
    setBackSize();
   
    return true;
}


/**********/
/*APPENDIX*/
/**********/

DiskComponent GBridge::getDiskComponent()
{
    DiskComponent dc(getSCType(), seq, pos);
   
    Uint8* data;
    int dlen;
   
    assembleApdx(&data, &dlen);
    dc.setAppendix(data, dlen);
   
    delete[] data;
   
    return dc;
}

void GBridge::setAppendix(const Uint8* data, unsigned int len)
{
   
    uint off=0;
    Appendix::Head* head;
   
    //head comes first:
    head = (Appendix::Head*)data;
    off+=sizeof(Appendix::Head);
   
   
   
    //REMOVE: LOAD OLD(when approp)
    if(progstate.getCurrentLoadVersion()==0)
    {
	
        DERROR("warning: loading OLD file version");
        bool ok = setNumPorts(16);
        DASSERTP(ok, "set num ports failed");
        off -= sizeof(uint);
    }
    else 
    {
        //set ports
        bool ok = setNumPorts(head->num_io);
        DASSERTP(ok, "set num ports failed");
    }
   
   
    //load name
    if(head->flipped)
    {
        want_flip=true;
        DASSERT(head->namesize);
    }
   
    if(head->namesize != 0)
    {
        //W:name better be null terminated!
        char* name = (char*)(data+off);
        filename.assign(name);
        off+=head->namesize;
    }
   
   
   
    //set labels
    DASSERT(head->nlabel>=0);
    DASSERT(head->nlabel<=head->num_io);
   
    for(uint i=0;i<head->nlabel;i++)
    {
        Appendix::LabelHead* lh = (Appendix::LabelHead*)(data+off);
        off+=sizeof(Appendix::LabelHead);
	
        if(lh->num >= ports.size())
        {
            DERROR("label index to big:" << lh->num);
            break;
        }
	
        ports[lh->num]->name.assign((char*)(data+off));
        off+=lh->size;
    }
   
    updateLabels();
   
   
    if(progstate.getCurrentLoadVersion()>11)
    {
        for(uint i=0;i<head->num_io;i++)
        {
            Appendix::IOEntryData* iod = (Appendix::IOEntryData*)(data+off);
	     
            setPortMode(i, iod->cmode, false);
	     
            off+=iod->thissize;
        }
	
        DASSERT(off==len);
    }
   
}

void GBridge::assembleApdx(Uint8** retdata, int* retlen)
{
    Appendix::Head head;
    head.flipped=flip;
   
    if(flip)
    {
        DASSERT(filename.length()!=0);
    }
   
    head.namesize=(filename.length()+1)*sizeof(char); //+1: '\0'
   
    head.num_io=ports.size();
   
    uint nlab=0;
    uint labsize=0;
    for(uint i=0;i<head.num_io;i++)
    {
        if(ports[i]->name != deflabel)
        {
            nlab++;
            labsize += (ports[i]->name.length()+1)*sizeof(char)+sizeof(Appendix::LabelHead); //+1: '\0'
        }
	
    }
    head.nlabel=nlab;
   
    uint ioe_size = head.num_io*sizeof(Appendix::IOEntryData);
   
    uint needbytes=sizeof(Appendix::Head) + head.namesize + labsize + ioe_size;
    Uint8* data = new Uint8[needbytes];
   
    uint off=sizeof(Appendix::Head);
    memcpy(data, (char*)&head, off);
    memcpy(data+off,(char*)filename.c_str(), head.namesize);
    off+=head.namesize;
   
    //save labels
    for(uint i=0;i<head.num_io;i++)
    {
        if(ports[i]->name != deflabel)
        {
            Appendix::LabelHead lh;
            lh.num=i;
            lh.size=(ports[i]->name.length()+1)*sizeof(char);
	     
            memcpy( (char*)(data+off),(char*)&lh, sizeof(Appendix::LabelHead));
            off+=sizeof(Appendix::LabelHead);
            memcpy(data+off,(char*)ports[i]->name.c_str(), lh.size);
	     
            off+=lh.size;
        }
    }
   
    //save cmodes
    for(uint i=0;i<head.num_io;i++)
    {
        Appendix::IOEntryData iodata;
        iodata.thissize=sizeof(Appendix::IOEntryData);
        iodata.cmode = ports[i]->cmode;
	
        memcpy( (char*)(data+off),(char*)&iodata, iodata.thissize);
        off+=iodata.thissize;
    }
   
    DASSERT(off==needbytes);
   
    *retlen=needbytes;
    *retdata=data;
}




/*******/
/*STATE*/
/*******/



void GBridge::getState(SoundCompEvent* e)
{
    byte* data=NULL;
    uint size=0;
   
    //remeber: buidState allocates 'data'
    buildState(&data, &size);
   
    e->copyData(data, size);
   
    delete data;
}

void GBridge::setState(SoundCompEvent* e)
{
    if(e->empty())return;
   
   
    //REMOVE: LOAD OLD(when approp)
    if(progstate.getCurrentLoadVersion()==0)
    {
        DERROR("warning: loading OLD file version");
        if(e->getSize()!=sizeof(State_V0))
        {
            DERROR("size missmatch");
            if(e->getSize() < sizeof(State_V0))return;
	     
            DERROR("trying to read...");
        }
        const State_V0* s = (const State_V0*)e->getData();
	
        int min= ports.size()<16 ? ports.size() : 16;
	
        for(int i=0;i<min;i++)
        {
            //WARNING: should be careful here if knobs "come and go"
            if(ports[i]->knob==NULL)continue;
	     
            ports[i]->knob->setValue(s->knob_vals[i]);
            ports[i]->out_val->set(ports[i]->knob->getValue());
	     
        }
	
    }
    else
    {
        //REMOVE: old ver load
        if(progstate.getCurrentLoadVersion()<=11)
        {
            const byte* rawdata = e->getData();
            StateHead* head = (StateHead*)rawdata;
            double* data = (double*)(rawdata+sizeof(StateHead));
	     
            //only set the state for the number of ports this comp have
            uint min = (ports.size()<head->num_io) ? ports.size() : head->num_io;
	     
            for(uint i=0;i<min;i++)
            {
                //only knob existed
                ports[i]->knob->setValue(data[i]);
                ports[i]->out_val->set(ports[i]->knob->getValue());
		  
            }
        }
        else
        {
            const byte* rawdata = e->getData();
            StateHead* head = (StateHead*)rawdata;
            double* data = (double*)(rawdata+sizeof(StateHead));
            IOEntry::CMODE* modes = (IOEntry::CMODE*)(rawdata+sizeof(StateHead)+head->num_io*sizeof(double));
	     
            //only set the state for the number of ports this comp have
            uint min = (ports.size()<head->num_io) ? ports.size() : head->num_io;
	     
            for(uint i=0;i<min;i++)
            {
                setPortMode(i, modes[i], false);
                setPortData(i, data[i]);		  
            }
        }
    }
   
}

void GBridge::buildState(byte** ret_data, uint* ret_size)
{
    double* data = new double[ports.size()];
    IOEntry::CMODE* modes = new IOEntry::CMODE[ports.size()];
   
    size_t dsize = sizeof(double)*ports.size();
    size_t msize = sizeof(IOEntry::CMODE)*ports.size();
   
    StateHead head;
    head.num_io = ports.size();
    size_t hsize = sizeof(StateHead);
   
    size_t fsize = hsize+dsize+msize;
    byte* final_data = new byte[fsize];
   
   
    for(uint i=0;i<ports.size();i++)
    {
        modes[i] = ports[i]->cmode;
	
        data[i] = getPortData(i);
	
    }
   
    memcpy(final_data, &head, hsize);
    memcpy(final_data+hsize, data, dsize);
    memcpy(final_data+hsize+dsize, modes, msize);
   
    delete data;
    delete modes;
   
    *ret_data = final_data;
    *ret_size = fsize;
}

void GBridge::setPortMode(uint n, IOEntry::CMODE cmode, bool rearange)
{
    IOEntry* port = ports[n];
    DASSERT(port!=NULL);
   
    if(port->sel_c==NULL)
    {
        return; //no mode present
    }
   
   
    DASSERT(port->knob!=NULL);
    DASSERT(port->button_onoff!=NULL);
    DASSERT(port->ns_int2!=NULL);
    DASSERT(port->no_comp!=NULL);
   
    port->cmode = cmode;
   
    if(port->cmode==IOEntry::KNOB)
    {
        replace(port->sel_c, port->knob, true); 
        port->sel_c=port->knob;
    }
    else if(port->cmode==IOEntry::BUTTON_ONOFF)
    {
        replace(port->sel_c, port->button_onoff, true);
        port->sel_c=port->button_onoff;
    }
    else if(port->cmode==IOEntry::INTEGER2)
    {
        replace(port->sel_c, port->ns_int2, true);
        port->sel_c=port->ns_int2;
    }
    else if(port->cmode==IOEntry::NONE)
    {
        replace(port->sel_c, port->no_comp, true);
        port->sel_c=port->no_comp;
    }
    else DERROR("no such cmode");
   
    if(rearange)
    {
        if(n<ports.size()-1)
        {
            if(port->cmode==IOEntry::INTEGER2)
            {
                setPortMode(n+1, IOEntry::NONE, false);
            }
        }
        if(n>=1)
        {
            if(ports[n-1]->cmode==IOEntry::INTEGER2)
            {
                setPortMode(n-1, IOEntry::NONE, false);
            }
        }
    }
   
    ports[n]->updateOutVal();
}

double GBridge::getPortData(uint i)
{
    double data=0;
   
    if(ports[i]->cmode==IOEntry::NONE)
    {
        data = 0;
    }
    else if(ports[i]->cmode==IOEntry::KNOB)
    {
        data = ports[i]->knob->getValue();
    }
    else if(ports[i]->cmode==IOEntry::BUTTON_ONOFF)
    {
        data = ports[i]->button_onoff->getValue();
    }
    else if(ports[i]->cmode==IOEntry::INTEGER2)
    {
        data = ports[i]->ns_int2->getValue();
    }
    else DERROR("no such cmode");
   
    return data;
}


void GBridge::setPortData(uint i, double val)
{
   
    if(ports[i]->cmode==IOEntry::NONE)
    {
    }
    else if(ports[i]->cmode==IOEntry::KNOB)
    {
        ports[i]->knob->setValue(val);
    }
    else if(ports[i]->cmode==IOEntry::BUTTON_ONOFF)
    {
        ports[i]->button_onoff->setValue((int)val);
    }
    else if(ports[i]->cmode==IOEntry::INTEGER2)
    {
        ports[i]->ns_int2->setValue((int)val);
    }
    else DERROR("no such cmode");
   
    ports[i]->updateOutVal();
}

void GBridge::IOEntry::updateOutVal()
{
    if(cmode==IOEntry::KNOB)
    {
        out_val->set(knob->getValue());
    }
    else if(cmode==IOEntry::BUTTON_ONOFF)
    {
        out_val->set(button_onoff->getValue());
    }
    else if(cmode==IOEntry::INTEGER2)
    {
        out_val->set(ns_int2->getValue());
    }
    else if(cmode==IOEntry::NONE)
    {
        out_val->set(0);
    }
    else DERROR("no such cmode");
   
}


/**********/
/*CALLBACK*/
/**********/

GBridge::C_PortVal::C_PortVal(GBridge* src, int n)
{
    this->src=src;
    this->n=n;
}

void GBridge::C_PortVal::action(Component* c)
{
    //DASSERT(src->ports[n]->knob != NULL);
    src->ports[n]->updateOutVal();
}

GBridge::C_CyclePortMode::C_CyclePortMode(GBridge* src, int n)
{
    this->src=src;
    this->n=n;
}

void GBridge::C_CyclePortMode::action(Component* c)
{
    //NOT a IOEntry function: req access to GBridge...
   
    IOEntry* port = src->ports[n];
   
    port->cmode = (IOEntry::CMODE)(port->cmode+1);
    if(port->cmode==IOEntry::EEND)port->cmode=(IOEntry::CMODE)0;
   
    src->setPortMode(n, port->cmode);
}


GBridge::C_NumPort::C_NumPort(GBridge* src)
{
    this->src=src;
}

void GBridge::C_NumPort::action(Component* c)
{
    src->setNumPorts(src->nport);
}

int GBridge::inputTakerId(const string& name, uint port)
{
    //DASSERT();
   
    for(uint i=0;i<ports.size();i++)
    {
        if(ports[i]->name==name)
        {
            return ports[i]->id;
        }
    }
    return -1;
}

int GBridge::outputGiverId(const string& name, uint port)
{
    //DASSERT();
   
    for(uint i=0;i<ports.size();i++)
    {
        if(ports[i]->name==name)
        {
            return ports[i]->id;
        }
    }
    return -1;
}
