#include "m_main.h" 
#include "../main/config.h"

using namespace std;
//using namespace spl;
/***************/
/*StateCopyHold*/
/***************/

SoundComp* StateCopyHold::getParent()
{
    return parent;
}

StateCopyHold::ERR StateCopyHold::copyState(SoundComp* from, bool partial)
{
    if(from==NULL)return NO_SRC;
    if(parent!=NULL)purge();
   
    //must reset this
    data.setIsPartial(false);
   
    if(partial)
    {
        data.setWantPartial(true);
    }
    else
    {
        data.setWantPartial(false);
    }
   
   
    parent=from;
    parent->getState(&data);
    return OK;
}

StateCopyHold::ERR StateCopyHold::pasteState(SoundComp* to)
{
    if(to==NULL)return NO_DEST;
    if(parent==NULL)return NO_COPY;
   
    if(parent->getSCType()!=to->getSCType())return NOTSAME_COMP_TYPE;
   
    to->setState(&data);
   
    return OK;
}

void StateCopyHold::purge()
{
    if(parent==NULL)return;
   
    data.free();
    parent=NULL;
}


/*WARNING:
 * make sure ro remove a SoundComp from the ground mixer if it adds itself to it because it is static in program
 * this is NOT good practise*/

ProgramState& Main::progstate = ProgramState::instance();


Main::Main(int x, int y, Screen* s, BitmapFont* font, Console* con)
{
    midi=NULL;
    this->con=con;
    selected_comp=NULL;
    this->s=s;
    f=font;
   
    //WARNING: this container MUST NOT contain anything but SoundComps i.e no Components!
    //subrutines expect SC's
    cont = new Container(x,y,s->W(),s->H());
    cont->resetHome(false);
   
    cont_off.x=0;
    cont_off.y=0;
    co_doclamp=false;
   
    //FIXME: WARNING: min must be 0 !
    setCompOffBound(progstate.getWSxmin(), progstate.getWSymin(), progstate.getWSxmax(), progstate.getWSymax());
   
    keyboard=NULL;
    play=NULL;
    statetick=NULL;
   
    mainmix = new Mixer();
   
    //pointer to a singleton?
    //SingleGround& ground = SingleGround::instance();
    //groundp=&ground;
    ground = new GroundMixer();
    mainmix->addInput((Value**)(&ground)); //always present
   
    menuon=false;
    buildMenu();
   
    bench = new MainBench(0+175,s->H()-96+54, s, f);
   
    smpls=NULL; //better do this before makeSampleBuffer()
    nsample_buff=0;
   
    //default to same as device
    makeSampleQueue(progstate.getSampleGenBuffer(), progstate.getSampleGenLen());
   
    lmouse_x=0;
    lmouse_y=0;
   
    last_menu_add=G_NONE;

    setPrefix("./");
}


Main::~Main()
{
    sc_copy.purge();
   
    freeSampleQueue();
   
    //make sure to delete bench(with states) BEFORE cont as it uses cont in destructor!
    delete bench;
   
    delete cont;//all done
    delete menu;
   
    delete ground;
}

void Main::buildMenu()
{
    SurfaceHoldAutoBuild& pix = SurfaceHoldAutoBuild::instance();
   
    Menu* submenu;
    //Menu* subsubmenu;
   
    menu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
    /*************/
    /*SOURCE MENU*/
    /*************/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]); 
   
    submenu->addItem("oscillator",new MenuAction(G_OSC, this));
    submenu->addItem("dc",new MenuAction(G_DC, this));
    submenu->addItem("sampler",new MenuAction(G_WAV, this));
    submenu->addItem("metronome",new MenuAction(G_METRO, this));
   
    menu->addSubMenu("source", submenu);
   
    /************/
    /*MIXER MENU*/
    /************/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
    submenu->addItem("mixer",new MenuAction(G_MIX, this));
    submenu->addItem("vector mix", new MenuAction(G_VECTOR, this));
    submenu->addItem("crossfade",new MenuAction(G_CROSSFADE, this));
   
    menu->addSubMenu("mix", submenu);
   
    /*****************/
    /*CONNECTION MENU*/
    /*****************/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
    submenu->addItem("doppler",new MenuAction(G_DOPPLER, this));
    submenu->addItem("select in",new MenuAction(G_SELECTIN, this));
    submenu->addItem("gate to pulse",new MenuAction(G_GATETOPULSE, this));
    submenu->addItem("sample&hold",new MenuAction(G_SAMPLE_AND_HOLD, this));
    submenu->addItem("drain",new MenuAction(G_DRAIN, this));
    submenu->addItem("changepulse",new MenuAction(G_CHANGEPULSE, this));
    submenu->addItem("onoff",new MenuAction(G_ONOFF, this));
    submenu->addItem("buffer",new MenuAction(G_BUFFER, this));
   
    menu->addSubMenu("connection", submenu);
   
   
    /***************/
    /*ENVELOPE MENU*/
    /***************/
   
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
   
    submenu->addItem("ahdsr",new MenuAction(G_AHDSR, this));
    //submenu->addItem("adsr",new MenuAction(G_ADSR, this));
    submenu->addItem("graph env", new MenuAction(G_DRAWENV, this));
    submenu->addItem("env follower",new MenuAction(G_ENV, this));
   
    menu->addSubMenu("envelope", submenu);
   
   
    /*************/
    /*FILTER MENU*/
    /*************/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
   
    submenu->addItem("bw lp,hp,bp",new MenuAction(G_FILTER, this));
    submenu->addItem("fft eq",new MenuAction(G_FFT_FILT, this));
    submenu->addItem("slew lim.",new MenuAction(G_SLEW, this));
   
    menu->addSubMenu("filter", submenu);
   
    /**************/
    /*PROCESS MENU*/
    /**************/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
   
    submenu->addItem("gain",new MenuAction(G_GAIN, this));
    submenu->addItem("delay",new MenuAction(G_DELAY, this));
    submenu->addItem("edelay",new MenuAction(G_EDELAY, this));
    submenu->addItem("clip",new MenuAction(G_CLIP, this));
    submenu->addItem("bitcrush",new MenuAction(G_BITCRUSH, this));
    //submenu->addItem("waveshape",new MenuAction(G_WS, this));
   
    menu->addSubMenu("process", submenu);
   
    /***********/
    /*NOTE MENU*/
    /***********/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
    submenu->addItem("note dc",new MenuAction(G_NOTE, this));
    submenu->addItem("notegain",new MenuAction(G_NOTEGAIN, this));
    submenu->addItem("keyextract",new MenuAction(G_KEYEXTR, this));
   
    menu->addSubMenu("note", submenu);
   
    /****************/
    /*VISUALIZE MENU*/
    /****************/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
    submenu->addItem("scope",new MenuAction(G_SCOPE, this));
    submenu->addItem("spectrum", new MenuAction(G_SPEC, this));
    submenu->addItem("print",new MenuAction(G_PRINT, this));
   
    menu->addSubMenu("visualize", submenu);
   
    /***********/
    /*MIDI MENU*/
    /***********/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
    submenu->addItem("midikey",new MenuAction(G_MIDIKEY, this));
    submenu->addItem("midictrl",new MenuAction(G_MIDICTRL, this));
    submenu->addItem("midipgm",new MenuAction(G_MIDIPGM, this));
   
    menu->addSubMenu("midi", submenu);
   
    //menu->addItem("",new MenuAction(G_NONE, this));
   
    /*********/
    /*HW MENU*/
    /*********/
#ifdef USE_ALT0
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
    submenu->addItem("alt0",new MenuAction(G_ALTZEROCTRL, this));
    menu->addSubMenu("hw", submenu);
#endif
   
    /***************/
    /*SEQUENCE MENU*/
    /***************/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
    submenu->addItem("tracker", new MenuAction(G_TRACKER, this));
    submenu->addItem("stepmat",new MenuAction(G_STEPMAT, this));
    submenu->addItem("punchpad",new MenuAction(G_PUNCHPAD, this));
   
   
    menu->addSubMenu("sequence", submenu);
   
    /***********/
    /*MATH MENU*/
    /***********/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
    submenu->addItem("linear",new MenuAction(G_LINEAR, this));
    submenu->addItem("sin",new MenuAction(G_SIN, this));
    submenu->addItem("sign",new MenuAction(G_SIGN, this));
    submenu->addItem("abs",new MenuAction(G_ABS, this));
    submenu->addItem("plus",new MenuAction(G_PLUS, this));
    submenu->addItem("minus",new MenuAction(G_MINUS, this));
    submenu->addItem("mult",new MenuAction(G_MULT, this));
    submenu->addItem("divide",new MenuAction(G_DIVIDE, this));
    submenu->addItem("exp",new MenuAction(G_EXP, this));
    submenu->addItem("log",new MenuAction(G_LOG, this));
    submenu->addItem("",new MenuAction(G_NONE, this));
    submenu->addItem("mod",new MenuAction(G_MOD, this));
    submenu->addItem("min",new MenuAction(G_MIN, this));
    submenu->addItem("max",new MenuAction(G_MAX, this));
    submenu->addItem("floor",new MenuAction(G_FLOOR, this));
    submenu->addItem("ceil",new MenuAction(G_CEIL, this));
    submenu->addItem("trunc",new MenuAction(G_TRUNC, this));
    submenu->addItem("",new MenuAction(G_NONE, this));
    submenu->addItem("integrate",new MenuAction(G_INTEGRATE, this));
    submenu->addItem("lessthan",new MenuAction(G_LESSTHAN, this));
    submenu->addItem("lessthaneq",new MenuAction(G_LESSTHANEQUAL, this));
    submenu->addItem("",new MenuAction(G_NONE, this));
    submenu->addItem("logisticmap",new MenuAction(G_LOGISTICMAP, this));
    submenu->addItem("threshold",new MenuAction(G_THRESHOLD, this));
    submenu->addItem("sort",new MenuAction(G_SORT, this));
   
    menu->addSubMenu("math", submenu);
   
    /**********/
    /*SYS MENU*/
    /**********/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]);
   
    submenu->addItem("speaker", new MenuAction(G_PLAY, this));
    submenu->addItem("keyboard",new MenuAction(G_KEYBOARD, this));
    submenu->addItem("joystick",new MenuAction(G_JOYSTICK, this));
    submenu->addItem("statetick", new MenuAction(G_STATETICK, this));
    submenu->addItem("modulebridge", new MenuAction(G_BRIDGE, this));
    submenu->addItem("samplerate",new MenuAction(G_SAMPLERATE, this));
    submenu->addItem("",new MenuAction(G_NONE, this));
    submenu->addItem("cmd process",new MenuAction(G_EXECCMD_PROCESS, this));
    submenu->addItem("cmd dumpforce",new MenuAction(G_EXECCMD_DUMPFORCE, this));
   
    menu->addSubMenu("system", submenu);
   
   
    dirmenu = new DirReaderMenu(pix["menuback_b"], pix["menuselback_b"],
    f , pix["menuback_b"]->w/8, pix["menuback_sub_b"]
    , pix["menuselback_sub_b"]);
   
    menu->addItem("",new MenuAction(G_NONE, this));
   
    /*************/
    /*MODULE READ*/
    /*************/
   
    dirmenu->setDir(progstate.getModuleLoadPath());
    dirmenu->setDirAction(new C_MenuDir(this));
    dirmenu->setOnlyPostfix(".kwf");
    dirmenu->setSkipPrefix("_");
    dirmenu->setDisplayPostfix(false);
   
    menu->addSubMenu("modpath", dirmenu);
   
    /***************/
    /*TEMPLATE READ*/
    /***************/
   
    tempmenu = new DirReaderMenu(pix["menuback_b"], pix["menuselback_b"],
    f , pix["menuback_b"]->w/8, pix["menuback_sub_b"]
    , pix["menuselback_sub_b"]);
   
    tempmenu->setDir(progstate.getTemplatePath());
    tempmenu->setDirAction(new C_TempDir(this));
    tempmenu->setOnlyPostfix(".kwf");
    tempmenu->setSkipPrefix("_");
    tempmenu->setDisplayPostfix(false);
   
    menu->addSubMenu("templates", tempmenu);
   
    /**********/
    /*DIR READ*/
    /**********/
   
    filemenu = new DirReaderMenu(pix["menuback_b"], pix["menuselback_b"],
    f , pix["menuback_b"]->w/8, pix["menuback_sub_b"]
    , pix["menuselback_sub_b"]);
   
    filemenu->setDir("./");
    filemenu->setDirAction(new C_FileDir(this));
    filemenu->setOnlyPostfix(".kwf");
    filemenu->setSkipPrefix("_");
    filemenu->setDisplayPostfix(false);
   
    menu->addSubMenu("./", filemenu);
   
   
   
    menu->addItem("",new MenuAction(G_NONE, this));
   
    /**********/
    /*COMMANDS*/
    /**********/
   
    submenu = new Menu(pix["menuback_b"], pix["menuselback_b"], f ,
    pix["menuback_b"]->w/8, pix["menuback_sub_b"], pix["menuselback_sub_b"]); 
   
    submenu->addItem("reset",new MenuCmdAction("reset", this));
    submenu->addItem("save",new MenuCmdAction("save", this));
    submenu->addItem("rload",new MenuCmdAction("rload", this));
    submenu->addItem("process",new MenuCmdAction("process", this));
    submenu->addItem("sync",new MenuCmdAction("sync", this));
    submenu->addItem("quit",new MenuCmdAction("quit", this));
    submenu->addItem("line_fg",new MenuCmdAction("line_fg", this));
    submenu->addItem("fullscreen",new MenuCmdAction("fullscreen", this));
   
    menu->addSubMenu("cmd", submenu);
}


void Main::relMoveAllComps(int x, int y)
{
    comp_list* list = cont->getList();
   
    for(comp_list_it it = list->begin();it!=list->end();it++)
    {
        SoundComp* sc = (SoundComp*)*it;
        sc->relMove(x,y);
	
        clipSCPos(sc);
    }
}

bool Main::clipSCPos(SoundComp* sc)
{
    DASSERT(sc!=NULL);
    if(sc==NULL)
    {
        ERROR("sc NULL: abort,  prog corrupt");
        return false;
    }
   
    bool ret=false;
   
    int x =  sc->getPos().x+cont_off.x;
    int y =  sc->getPos().y+cont_off.y;
   
    int w =  sc->getPos().w;
    int h =  sc->getPos().h;
   
    int r = x+w;
    int b = y+h;
   
    int xmax = s->W()+progstate.getWSxmax();
    int ymax = s->H()+progstate.getWSymax();
   
    if(x < 0)
    {
        sc->relMove(-x, 0);
        ret=true;
    }
    if(r > xmax)
    {
        sc->relMove( -r+xmax, 0);
        ret=true;
    }
    if(y < 0)
    {
        sc->relMove(0, -y);
        ret=true;
    }
    if(b > ymax)
    {
        sc->relMove(0,  -b+ymax);
        ret=true;
    }
   
    return ret;
}



void Main::setCompOff(int x, int y)
{
    if(co_doclamp)
    {
        if(x<co_min_x)x=co_min_x;
        if(y<co_min_y)y=co_min_y;
        if(x>co_max_x)x=co_max_x;
        if(y>co_max_y)y=co_max_y;
    }
   
    int dx = x-cont_off.x;
    int dy = y-cont_off.y;
   
    cont_off.x=x;
    cont_off.y=y;
   
    if(connection.doingCon())
    {
        //- here to, i.e view moves left lines move right
        connection.outpos.x-=dx;
        connection.outpos.y-=dy;
    }
   
    //must move comps the other way tseen from the "view" i.e if the view is heading left
    //comps must move right
    //IMPORTANT: this is only a 'running program' transform must restore cont pos before save...
   
    cont->setPos(-cont_off.x, -cont_off.y);
   
    //clip comps
    comp_list* list = cont->getList();
    for(comp_list_it it = list->begin();it!=list->end();it++)
    {
        SoundComp* sc = (SoundComp*)*it;
	
        clipSCPos(sc);
    }
}

void Main::addCompOff(int x, int y)
{
    if(co_doclamp)
    {
        if(cont_off.x+x<co_min_x)x=co_min_x-cont_off.x;
        if(cont_off.y+y<co_min_y)y=co_min_y-cont_off.y;
        if(cont_off.x+x>co_max_x)x=co_max_x-cont_off.x;
        if(cont_off.y+y>co_max_y)y=co_max_y-cont_off.y;
    }
   
    cont_off.x+=x;
    cont_off.y+=y;
   
    if(connection.doingCon())
    {
        connection.outpos.x-=x;
        connection.outpos.y-=y;
    }
   
    cont->setPos(-cont_off.x, -cont_off.y);
}

SDL_Rect Main::getCompOff()
{
    return cont_off;
}

void Main::setCompOffBound(int min_x, int min_y, int max_x, int max_y)
{
    if(min_x>max_x)swap(min_x, max_x);
    if(min_y>max_y)swap(min_y, max_y);
   
    co_doclamp=true;
    co_min_x=min_x;
    co_min_y=min_y;
    co_max_x=max_x;
    co_max_y=max_y;
}

m_err Main::saveProgram(const string& file)
{  
    ofstream out(file.c_str(), ios::binary);
    if(!out)return FILE_CREATE_ERR;
   
    ProgramFile filestate;
   
    filestate.start_marker=bench->getStartMarkerIndex();
    filestate.stop_marker=bench->getStopMarkerIndex();
    filestate.current_slot=bench->getCurrentMarkerIndex();
   
    //write head
    out.write((char*)&filestate, sizeof(ProgramFile));
    if(!out.good())
    {
        DERROR("write failed");
        out.close();
        return FILE_IO_ERR;
    }
   
    //write data
   
    //cont might be offsetted by cont_off, MUST NOT SAVE THAT... reset offset to zero before save
    cont->setPos(0,0);
   
    m_err err;
    err =  saveComponents(cont, &out);
    if(err!=OK)
    {
        out.close();
        DERROR("write comp failed");
        return err;
    }
   
    //reset offsets
    cont->setPos(-cont_off.x, -cont_off.y);
   
    err = saveConnections(&connection, &out);
    if(err!=OK)
    {
        out.close();
        DERROR("write con failed");
        return err;
    }
   
    err = saveAllStates(bench->getStates(), &out);
    if(err!=OK)
    {
        out.close();
        DERROR("write state failed");
        return err;
    }
   
    out.close();
   
    return OK;
}


void Main::setLastLoad(const string& s)
{
    lastload=s;
}

string Main::getLastLoad() const
{
    return lastload;
}


void Main::setPrefix(const string& s)
{
    prefix=s;
    if (prefix.length() == 0)prefix = "/";
    else if(prefix.at(prefix.length()-1) != '/')
    {
        prefix = prefix + "/";
    }
    filemenu->setDir(prefix);
}

string Main::getPrefix() const
{
    return prefix;
}


m_err Main::loadProgram(const string& file, bool try_force)
{
   
    ifstream in(file.c_str(), ios::binary);
    if(!in)return FILE_NOTFOUND_ERR;
   
    ProgramFile filestate;
   
    //read head
    in.read((char*)&filestate.head, sizeof(ProgramFile::Head));
    if(!in.good())
    {
        DERROR("read head failed");
        in.close();
        return FILE_IO_ERR;
    }
   
    //set programstate to this load version so other things know
    DASSERT(filestate.head.ver>=0);
   
    //FIXME: check this version stuff
    int old_lver = progstate.getCurrentLoadVersion();
    progstate.setCurrentLoadVersion(filestate.head.ver);
   
    if(filestate.head.name[0]!='k' && filestate.head.name[1]!='w')
    {
        DERROR("file missmatch or corrupt");
	
        progstate.setCurrentLoadVersion(old_lver);
        return FILE_CORRUPT_ERR;
    }
   
    int want = sizeof(ProgramFile)-sizeof(ProgramFile::Head);
    int rest = filestate.head.state_size;
   
    int startmark=0;
    int stopmark=0;
    int cslot=0;
   
    if(want!=rest)
    {
        DERROR("PROGSTATE on file size missmatch, skipping");
	
        //throw away rest
        in.seekg(rest, ios::cur);
	
    }
    else //read OK: read and load
    {
        in.read((char*)(&filestate)+sizeof(ProgramFile::Head), want);
        if(!in.good())
        {
            DERROR("read failed");
            in.close();
	     
            progstate.setCurrentLoadVersion(old_lver);
            return FILE_IO_ERR;
        }
	
	
        startmark = filestate.start_marker;
        stopmark = filestate.stop_marker;
        cslot =filestate.current_slot;
    }
   
    int sc_seq_off=SoundComp::getSeqCount();
   
    m_err err =  loadComponentsMain(&in, sc_seq_off);
    if(err!=OK)
    {
        if(err==SOME_COMP_FAIL)
        {
            DERROR("some comps failed, continue: "  << file);
        }
        else
        {
            DERROR("load comps failed:"  << file);
            in.close();
	     
            progstate.setCurrentLoadVersion(old_lver);
            return err;
        }
    }
   
    err = loadConnections(cont, &connection, &in, sc_seq_off);
    if(err==CON_FAIL_ERR)
    {
        DERROR("some con failed" << file);
    }
    if(err==FILE_IO_ERR)
    {
        in.close();
	
        progstate.setCurrentLoadVersion(old_lver);
        return err;
    }
   
    err = loadStatesMain(&in, sc_seq_off, try_force);
   
    if(err!=OK)
    {
        DERROR("state failed: " << file << " try force: " << try_force);
    }
   
    //convert state versions?
    if(filestate.head.ver != FILEFORMAT_VERSION)
    {
        /*cout << endl << "WARNING: states in file is old version; converting to new; " << 
          "consider running 'save' from console to update file" << endl;*/
	
        //also in: m_main_help.cpp
        //make this a silent operation: states will change, cant re save all everytime,
        //BUT make sure setState()-s handle different verions!
	
        bench->getStates()->resaveAllInCurrLoadVer(filestate.head.ver);
    }
   
    //states are resaved must do this before applying final(setCurrent...):
    progstate.setCurrentLoadVersion(old_lver);
   
    in.close();
   
    //set state LAST(important)
    bench->setStartMarkerIndex(startmark);
    bench->setStopMarkerIndex(stopmark);
    bench->setCurrentMarkerIndex(cslot);
   
   
    return err;
}



int Main::resetSeqNums()
{
    unsigned int count=0;
    comp_list* list = cont->getList();
   
    for(comp_list_it it = list->begin();it!=list->end();it++)
    {
        SoundComp* sc = (SoundComp*)*it;
        sc->setSeqNum(count++);
    }
    return count;
}

void Main::freeSampleQueue()
{
    if(smpls==NULL)return;
   
    smp_queue = queue<Sample*>();
   
    if(smpls!=NULL)
    {
        for(unsigned int i=0;i<nsample_buff;i++)delete smpls[i];
        delete[] smpls;
    }
   
    smpls=NULL;
}

void Main::makeSampleQueue(uint n, uint buff_len)
{
    //DASSERT(smpls==NULL);
    if(smpls!=NULL)freeSampleQueue();
    DASSERT(n>0);
   
    csmp=0; //better reset this also so it does not point nowhere
   
    nsample_buff=n;
    smpls = new Sample*[nsample_buff];
    for(unsigned int i=0;i<nsample_buff;i++)smpls[i] = new Sample(buff_len);
}


/*
  const Sample* Main::makeSample()
  {
  //FIXMENOW: this is not good!
  mainmix->make(smpls[csmp]->getSmpls(), smpls[csmp]->getLen(), SSHORT_MAX);
  * 
  int old = csmp;
  * 
  csmp++;
  csmp%=nsample_buff;
  * 
  return smpls[old];
  }*/

uint Main::smpQueueSize()
{
    return nsample_buff;
}


int Main::smpQueuePush()
{
    if(smp_queue.size()==nsample_buff)
    {
        DERROR("buffer full");
        return 1;
    }
   
    //FIXMENOW: this is not good!
    //1. quantized to SSHORT_MAX should not be done here!
    //mainmix->make(smpls[csmp]->getSmpls(), smpls[csmp]->getLen(), SSHORT_MAX/*, true*//*clip output*/);
    mainmix->make(smpls[csmp], SSHORT_MAX);
    smp_queue.push(smpls[csmp]);
   
    csmp++;
    csmp%=nsample_buff;
   
    return 0;
}

uint Main::smpQueueIn()
{
    return smp_queue.size();
}


const Sample*  Main::smpQueuePop()
{
    DASSERT(smp_queue.size()>0);
    Sample* elm = smp_queue.front();
    smp_queue.pop();
   
    return elm;
}

void Main::smpQueueClear()
{
    smp_queue = queue<Sample*>();
    csmp=0;
}


m_err Main::loadComponentsMain(istream* fin, int sc_seq_off)
{
    DASSERT(fin!=NULL);
   
    int bytes;
    fin->read((char*)&bytes, sizeof(int));
    if(!fin->good())
    {
        DERROR("FILE IO ERR");
        return FILE_IO_ERR;
    }
   
    //FIXME: better solution than hard fail
    if(bytes!=sizeof(DiskComponent::Data))
    {
        DERROR("(E) comp size in program disagree with file!");
        return COMP_FAIL_ERR;
    }
   
   
    int n,l;
    fin->read((char*)&n, sizeof(int));
    if(!fin->good())
    {
        DERROR("FILE IO ERR");
        return FILE_IO_ERR;
    }
   
    VERBOSE1(cout << "loading " << n << " comps...";   cout.flush(););
   
    l=n;
    bool all_ok=true;
   
    //must set SoundComp::st_seq to match that which is loaded
    //to prevet comps getting same id!
    unsigned int max_seq=0;
   
    while(n>0)
    {
        DiskComponent::Data data;
        fin->read((char*)&data, sizeof(DiskComponent::Data));
        if(!fin->good())
        {
            DERROR("FILE IO ERR");
            return FILE_IO_ERR;
        }
	
        data.offsetSeq(sc_seq_off);
	
        bool do_appendix=false;
        Uint8* appendix=NULL;
	
        if(data.appendix_len>0)
        {
            do_appendix=true;
            VERBOSE1(cout << "read appendix: " << endl;);
            appendix = new Uint8[data.appendix_len];
            fin->read((char*)appendix, data.appendix_len);
        }
	
        //cout << endl << "  * loaded comp... id:" << data.sc_type << " pos(" << data.pos.x <<","<<data.pos.y<<") seqnum: " << data.seq << endl;
        m_err ret = addComp(data.sc_type, data.pos.x, data.pos.y);
        if(ret==COMP_FAIL_ERR)
        {
            all_ok=false;
            cout << endl << " component " << l-n <<" failed";
            if(appendix!=NULL)delete[] appendix;
        }
        else
        {
            //DASSERT(ret==OK);
	     
            //reset seqno
            SoundComp* last = (SoundComp*)cont->getLast();
            last->setSeqNum(data.seq);
	     
            if(do_appendix)
            {
                last->setAppendix(appendix, data.appendix_len);
                delete[] appendix;
		  
                if(last->getSCType()==G_WAV)
                {
                    GWav* g = (GWav*)last;
                    g->loadByAppendix(getPrefix());
                }
		  
                //must treat module bridge different
                else if(last->getSCType()==G_BRIDGE)
                {
                    GBridge* bridge = (GBridge*)last;
		       
                    //if bridge does not want flip treat it as std comp
		       
                    //want flip, must load what bridge want from disk
                    if(bridge->wantFlip())
                    {
			    
                        VERBOSE2(cout << "modbridge in load: " << bridge->getFilename() << endl;);
			    
                        //will throw away last but use it to create and load 
                        //a modbridge ready to use
			    
			    
                        //1.prefix
                        //2.mod path
			    
                        SoundComp* b;
                        m_err err = loadModuleBridge(bridge->getFilename(), getPrefix(),
                        f, &b, getMIDIcall(), getGround());
			    
                        if(err==FILE_NOTFOUND_ERR)
                        {
                            err = loadModuleBridge(bridge->getFilename(), progstate.getModuleLoadPath(), f, &b, getMIDIcall(), getGround());
                        }
			    
                        bool add_ok=true;
                        if(err!=OK)
                        {
                            add_ok=false;
                            /*if(err==BRIDGE_NOTFOUND_ERR)con->show("MODULEBRIDGE not found, abort"); 
                              if(err==FILE_NOTFOUND_ERR)con->show("some file part missing:%s", fname.c_str());
                              if(err==FILE_IO_ERR)con->show("file error, abort");*/
                            stringstream errout;
                            errout << "modbridge failed: ";
				 
                            all_ok=false;
				 
                            if(err==BRIDGE_NOTFOUND_ERR)
                            {
                                errout << "MODULEBRIDGE not found, abort";
                            }
                            else if(err==FILE_NOTFOUND_ERR)
                            {
                                errout << "file missing: " << getPrefix() << bridge->getFilename();
                            }
                            else if(err==FILE_IO_ERR)
                            {
                                errout << "file error, abort";
                            }
                            else if(err==FILE_CORRUPT_ERR)
                            {
                                errout << "file corrupt, abort";
                            }
                            else 
                            {
                                errout << "trying to continue...";
                                add_ok=true; //try to add it
                            }
				 
                            errout << " :" << err << endl;
                            DERROR(errout.str());
                        }
			    
                        if(add_ok)
                        {
                            DASSERT(b!=NULL && bridge!=NULL); 
				 
                            //cont->add(b, bridge->pos.x, bridge->pos.y);
                            cont->add(b, b->getPos().x, b->getPos().y);
				 
                            //b has taken bridge's place, inherit its id's
                            b->setSeqNum(bridge->getSeqNum());
				 
                            //FIXMENOW: this is more like a hack: because setPos() only sets the pos of the container
                            //this it the only way right now, fixit!
                            b->relMove(bridge->getPos().x-b->getPos().x, bridge->getPos().y-b->getPos().y);
				 
                            selectComp(b);
				 
                            //done with 'bridge'
                            remComp(last);
                        }
                    }//end wantFlip
		       
                }//end last==G_BRIDGE
		  
            }//end doappendix
	     
            if(data.seq>max_seq)max_seq=data.seq;
	     
            VERBOSE2(cout << "load add: " << last->getSeqNum() << endl;);
        }
	
        n--;
    }
   
    SoundComp::setSeqCount(max_seq+1); //+1 is because: next comp add now gets max_seq+1 where max_seq is taken
   
    VERBOSE1(if(all_ok)cout << "ok" << endl;else cout << endl;);
   
    if(all_ok)return OK;
    else return SOME_COMP_FAIL;
}


m_err Main::loadStatesMain(istream* in, int sc_seq_off, bool try_force)
{
    DASSERT(in!=NULL);
   
    m_err ret;
    if( (ret = loadAllStates(cont, bench->getStates(), in, sc_seq_off, try_force)))
    {
        DERROR("(E) state load fail");
        return ret;
    }
    VERBOSE1(cout << "states loaded, setting...";);
    cout.flush();
   
    //set first state
    bench->setCurrentMarkerIndex(0);
    bench->setAllStates();
   
    //?
    //now must update state for selecte_comp(it was selected before load states)
    bench->currentState(getSelectedComp());
   
    VERBOSE1(cout << "ok" << endl;);
   
    return OK;
}

SoundComp* Main::getMouseOver(int x, int y)
{
    SoundComp* tmp = (SoundComp*)cont->checkSelection(x, y);
    return tmp;
}

void Main::remComp(SoundComp* sct)
{
    if(sct==NULL)
    {
        DERROR("try to rem NULL");
        return;
    }
   
    //remove selected?
    if(sct==selected_comp)
    { 
        selected_comp=NULL;
    }
   
    //special treatment
    if(sct==keyboard)keyboard=NULL;
    if(sct->getSCType()==G_JOYSTICK)
    {
        list<GJoystick*>::iterator it;
        it=find(joysticks.begin(), joysticks.end(),sct);
        DASSERT(it!=joysticks.end());
        joysticks.erase(it);
    }
    if(sct==play)
    {
        mainmix->remInput(play->getOutput());
        play=NULL;
    }
    if(sct==statetick)statetick=NULL;
   
    //remove MIDI comps from callbacks
    if(sct->getSCType()==G_MIDIKEY)
    {
        bool ret = midicall.remKey((GMIDIkey*)sct);
        DASSERT(ret);
    }
    if(sct->getSCType()==G_MIDICTRL)
    {
        bool ret = midicall.remCtrl((GMIDIctrl*)sct);
        DASSERT(ret);
    }
    if(sct->getSCType()==G_MIDIPGM)
    {
        bool ret = midicall.remPgm((GMIDIpgm*)sct);
        DASSERT(ret);
    }
   
    //must remove all potaential midicall/key in bridge from callback
    if(sct->getSCType()==G_BRIDGE)
    {
        GBridge* b = (GBridge*)sct;
        /*int num =*/ b->remAllMidiHooks(&midicall);
    }
   
    //precaution
    SoundComp* safe = findComp(cont,sct->getSeqNum());
    DASSERT(safe!=NULL);
   
    //remove this components states from bench
    bench->removeCompState(sct);
   
    //if comp have current state copy, MUST remove it
    if(sc_copy.getParent()==sct)
    {
        sc_copy.purge();
    }
   
   
   
    //remove inputs
    map<uint, InputTaker*>* in = sct->getAllIntakers();
    map<uint, InputTaker*>::iterator it;
   
    for(it=in->begin();it!=in->end();it++)
    {
        InputTaker* tak = it->second;
        DASSERT(tak!=NULL);
        if(tak->line!=NULL)
        {
            bool r =connection.removeLine(tak->line);
            DASSERT(r);
        }
	
    }
   
    //remove outputs
    map<uint, OutputGiver*>* out = sct->getAllOutgivers();
    map<uint, OutputGiver*>::iterator oit;
   
    for(oit=out->begin();oit!=out->end();oit++)
    {
        OutputGiver* tak = oit->second;
        DASSERT(tak!=NULL);
	
        if(tak->line!=NULL)
        {
            bool r = connection.removeLine(tak->line);
            DASSERT(r);
        }
    }
   
    //delete component;
    cont->rem(sct);
    delete sct;
   
}

StateCopyHold::ERR Main::stateCopySelected(bool partial)
{
    StateCopyHold::ERR err = sc_copy.copyState(getSelectedComp(), partial);
    return err;
}

StateCopyHold::ERR Main::statePasteSelected()
{
    StateCopyHold::ERR err = sc_copy.pasteState(getSelectedComp());
    return err;
}


bool Main::remMouseOver(int x, int y)
{
    SoundComp* tmp = getMouseOver(x, y);
   
    if(tmp!=NULL)
    {
        tmp = (SoundComp*)tmp->getHome();
	
        DASSERT(tmp!=NULL);
	
        remComp(tmp);
        return true;
    }
    return false;
}

SoundComp* Main::selectComp(SoundComp* c)
{
    if(selected_comp==c)return c;
   
    selected_comp = c;
   
    //if it's NULL it will deselect
    bench->currentState(selected_comp);
   
    if(c==NULL)return c;
    //apply state(if it exist)
   
   
    cont->moveToTop(selected_comp);
   
    return selected_comp;
}



SoundComp* Main::handleClickOver(int x, int y, int button, bool l_shift)
{
    //bench presidents
    if(bench->handleClickOver(x,y, button, l_shift))return NULL;
   
    if(button==1) //middle button
    {
    }
    if(button==0) //left button
    {
        //get clicked component after doing it's actions
        Component* clicked_comp = cont->clickSelected(x, y);
	
        if(clicked_comp==NULL)
        {
            //this de-selects comp if clicked on workspace; for comps that stays SELECTED after mouse rel
            if(cont->haveSelected())
            {
                cont->releaseSelected();
            }
	     
            selectComp(NULL);
            return NULL;
        }
	
	
        //FIXMENOW: OutputGiver is NOT a SoundComp!!!
        //adding input-line?
        if(clicked_comp->compID()==OUTPUT_GIVER)
        {
            connection.startCon((OutputGiver*)clicked_comp);
        }
	
        //set selected_comp approp, and move it to be drawn first in list
        //must use home to get the whole container not just one comp(e.g a button)
        selectComp((SoundComp*)clicked_comp->getHome());
    }
    else if(button==2) //right
    {
	
        SoundComp* last = getMouseOver(x,y );
	
        //menu
        if(last==NULL/* && !menuon*/)
        {
            menuon=true;
            menu->setPos(x, y);
            addpos.x=x+cont_off.x;
            addpos.y=y+cont_off.y; //add at current pos
            menu->setActionPos(addpos);
	     
            //---
            menu->onOpen();
        }
	
    }
    return getSelectedComp();
}


bool Main::handleReleaseOver(int x, int y)
{
    if(menuon)
    {
        menu->onRelease();
        menuon=false;
    }
   
    //get component released over
    Component* tmp = cont->checkSelection(x, y);
   
    if(tmp!=NULL && tmp->compID() == INPUT_TAKER && connection.doingCon())
    {
        connection.finishCon( (InputTaker*)tmp);
    }
    else if(connection.doingCon())connection.reset(); //terminate con
   
    if(cont->haveSelected())
    {
        if(!cont->getSelectedComp()->getStaySelected())cont->releaseSelected();
    }
   
   
    bench->handleReleaseOver();
    return true;
}

bool Main::handleInput(Input& in)
{
   
    int x = in.xMouse();
    int y = in.yMouse();
   
    lmouse_x=x;
    lmouse_y=y;
   
   
    if(in.mouseReleaseEvent())
    {
        handleReleaseOver(x, y);
    }
   
    if(in.mousePressEvent())
    {
        in.startNextMButton();
        int next=in.nextMButton();
        while(next!=-1)
        {
            handleClickOver(x,y, next, in.keySet(SDLK_LSHIFT));
	     
            next=in.nextMButton();
        }
    }
    bool handled=false;
    if(cont->haveSelected()) //this gives mouse motion to selected sliders etc, i.e. only the deepes component
    {
        handled = cont->giveInput(in);
        //#warning this might be wrong
        //IS NOW(not checked if correct), probably correct
        clipSCPos((SoundComp*)(cont->getSelectedComp()->getHome()));
	
        //WAS BEFORE:
        //clipSCPos(getSelectedComp()); //can be NULL, not often though
    }
   
    //this gives input to "home": good when the home want to do stuff
    //e.g. g_tracker: gives input to the "tracker" whenever g_tracker is selected
    if(getSelectedComp()!=NULL) 
    {
        if(!handled)
        {
            handled = getSelectedComp()->giveInput(in);
        }
	
    }
   
   
    //this must be done after cont->giveInput(): otherwise this will reset mouse move
    int x_m = in.xMouseMove();
    int y_m = in.yMouseMove();
   
   
    //because menu is not in a container it needs to get input manualy:
    if(menuon)
    {
        //menu->mousePos(x,y);
        menu->giveInput(in);
    }
   
    bench->handleMouseMotion(x_m, y_m); //unused for now
   
    return handled;
}

void Main::draw()
{
    //fill bg-color
    s->fill(s->makeColor(progstate.bg_color));
   
    //draw grid?
    if(progstate.draw_grid)
    {
        double sp=progstate.grid_spc;
	
        //draw vert
        double d = cont_off.x-co_min_x;
        double xoff = (floor(d/sp)+1)*sp-d;
	
        while(xoff<s->W())
        {
            s->line((int)xoff, 0, (int)xoff, s->H(), s->makeColor(progstate.grid_color));
            xoff+=sp;
        }
	
        //draw horiz
        d = cont_off.y-co_min_y;
        double yoff = (floor(d/sp)+1)*sp-d;
	
        while(yoff<s->H())
        {
            s->line(0, (int)yoff, s->W(), (int)yoff, s->makeColor(progstate.grid_color));
            yoff+=sp;
        }
    }
   
   
    //draw have state
   
    comp_list* cl = cont->getList(); 
    BenchStates* bs = bench->getStates();
    Uint32 color = s->makeColor(140,140, 10);
    Uint32 color_o = s->makeColor(158,158,158);
   
    for(comp_list_it it = cl->begin();it!=cl->end();it++)
    {
        SoundComp* sc = (SoundComp*)*it;
        int tx = sc->getPos().x;
        int ty = sc->getPos().y;
        int tw = sc->getPos().w;
        int th = sc->getPos().h;
	
        if(progstate.getShowHaveState())
        {
            CompStates* cs = bs->getCompStates(sc);
            if(cs!=NULL)
            {
                if(cs->indexInUse(bench->getCurrentMarkerIndex()))
                {
                    s->wirebox(tx-1,ty-1,tx+tw+1,ty+th+1, color);
                }
            }
	     
        }
	
        s->wirebox(tx,ty,tx+tw,ty+th, color_o);
    }
   
   
    if(!progstate.getDrawLinesFG())connection.draw(s, progstate.fancy_lines, progstate.line_w);
   
    //COMPS
    cont->draw(s);
   
    //outline last clicked
    if(selected_comp!=NULL)
    {
        int tx = selected_comp->getPos().x;
        int ty = selected_comp->getPos().y;
        int tw = selected_comp->getPos().w;
        int th = selected_comp->getPos().h; 
	
        //FIXME: make opt
        Uint32 color = s->makeColor(155,255,5);
	
        s->wirebox(tx,ty,tx+tw,ty+th, color);
	
        /*	
            s->line(tx,ty,tx+tw,ty, color); //left->right : up
            s->line(tx,ty+th-1,tx+tw,ty+th-1, color); //left->right : down
            s->line(tx,ty,tx,ty+th, color); 
            s->line(tx+tw,ty,tx+tw,ty+th, color);*/
    }
   
   
   
   
    if(progstate.getDrawLinesFG())connection.draw(s, progstate.fancy_lines, progstate.line_w);
   
    if(connection.doingCon())
    {
	
        //get color
        SDL_Color s_color;
        SDL_Color e_color;
	
        Uint32 color;
        color=s->makeColor(progstate.getLineColor());
	
        e_color = progstate.getLineColor();
	
	
        float c_scale=0.5;
        s_color.r = (Uint8)(e_color.r*c_scale);
        s_color.g = (Uint8)(e_color.g*c_scale);
        s_color.b = (Uint8)(e_color.b*c_scale);
	
        SurfaceHoldAutoBuild& pix = SurfaceHoldAutoBuild::instance();
	
        s->blit(pix["inout_used"], connection.outpos.x-pix["inout_used"]->w/2, connection.outpos.y-pix["inout_used"]->h/2);
	
        if(progstate.fancy_lines)
            connection.drawPatchCord(connection.outpos.x, connection.outpos.y, lmouse_x, lmouse_y,
            progstate.draw_line_tension, s, s_color, e_color, progstate.line_w);
        else 
            s->roundedLine(connection.outpos.x, connection.outpos.y, lmouse_x, lmouse_y,
            s_color, e_color,progstate.line_w);
    }
   
    bench->draw();
   
    if(menuon)
    {
        menu->draw(s);
    }
   
}


m_err Main::addComp(SoundComp* s, int x, int y)
{
    cont->add(s, x, y);
    clipSCPos(s);
   
    return OK; //can't fail
}

//sets selectedComp()
m_err Main::addComp(SCType id, int x, int y) 
{
    GExecCmd* exec=NULL;
   
    VERBOSE2(cout << "add: " << SoundComp::getSeqCount() << endl;);
   
    SoundComp* last = SCFactory(id, x, y, f, getGround());
   
    if(last==NULL)
    {
        DERROR("comp create failed");
        return COMP_FAIL_ERR;
    }
   
    //some comps need special treatment
    //some unnessesary creation in these cases
    switch(id)
    {
        case G_JOYSTICK:
            joysticks.push_back((GJoystick*)last);
            break;
        case G_KEYBOARD:
            if(keyboard!=NULL)
            {
                VERBOSE1(cout << "keyboard present" << endl;);
                delete last;
                return NO_COMP_ADDED;
            }
            keyboard = (GKeyboard*)last;
            break;
	
        case G_PLAY:
            if(play!=NULL)
            {
                VERBOSE1(cout << "play present" << endl;);
                delete last;
                return NO_COMP_ADDED;
            }
            play = (GPlay*)last;
            play->setNChan(progstate.getNChan());
            mainmix->addInput(play->getOutput()); //add play to output stream
            break;
	
        case G_STATETICK:
            if(statetick!=NULL)
            {
                VERBOSE1(cout << "statetick present" << endl;);
                delete last;
                return NO_COMP_ADDED;
            }
            statetick = (GStateTick*)last;
            statetick->setBench( bench);
            break;
	
        case G_MIDIKEY:
            midicall.addKey((GMIDIkey*)last);
            break;
        case G_MIDICTRL:
            midicall.addCtrl((GMIDIctrl*)last);
            break;
        case G_MIDIPGM:
            midicall.addPgm((GMIDIpgm*)last);
            break;
	
            //only do this here in main, not in modulebridge(let these be latent in a mb):
        case G_EXECCMD_DUMPFORCE:
        case G_EXECCMD_PROCESS:
            exec = (GExecCmd*)last;
            exec->setCmdQueue(&cmd_queue);
            break;
	
	
        default:
            break;
    }
   
    cont->add( last , x, y);
    clipSCPos(last);
   
    selectComp(last);
   
    return OK;
}

bool Main::keyboardSelected()
{
    if(keyboard==NULL)return false;
    if(selected_comp==keyboard)return true;
    return false;
}

bool Main::keyboardStay()
{
    if(keyboard==NULL)return false;
    return keyboard->stay;
}


void Main::giveKeyboard(Input* in)
{
    //FIXME: move logic to g_keyboard...
    static char keys[26] = {'z','x','c','v','b','n','m','a','s','d','f','g',
                            'h','j','k','l','q','w','e','r','t','y','u','i','o','p'};
   
    for(int i=0;i<26;i++)
    {
        if(in->keySet((SDLKey)keys[i]))
        {
            keyboard->setHz(i+1);
            keyboard->gateOn(true);
            return;
        }
    }
    keyboard->gateOn(false);
}

void Main::giveJoystick(Input* in)
{
    if(joysticks.size()==0)return;
   
    list<GJoystick*>::iterator it;
   
    for(it=joysticks.begin();it!=joysticks.end();it++)
    {
        GJoystick* joy = *it;
        int id=joy->getId();
        joy->setOpen(false);//maybe overkill?
        if(!in->joyActive(id))continue;
	
        joy->setOpen(true);
        for(uint i=0;i<in->nJoyAxis(id);i++)joy->giveAxisPos(i, in->joyAxisPosNormal(i,false,id));
        for(uint i=0;i<in->nJoyButton(id);i++)joy->giveButtonState(i, in->joyButton(i,false,id));
	
    }
}


bool Main::midiCompPresent()
{
    return midicall.numComps()>0;
   
}

MIDIcallbacks* Main::getMIDIcallbacks()
{
    return &midicall;
}

MIDIcall* Main::getMIDIcall()
{
    return &midicall;
}


void Main::syncAllToIndex(unsigned int index)
{
    comp_list* list = cont->getList();
   
    for(comp_list_it it = list->begin();it!=list->end();it++)
    {
        SoundComp* sct = (SoundComp*)*it;
        sct->syncToIndex(index);
    }
   
}

void Main::setLastMenuAdd(SCType t)
{
    last_menu_add=t;
}

SCType Main::getLastMenuAdd()
{
    return last_menu_add;
}


/************/
/*MenuAction*/
/************/

MenuAction::MenuAction(SCType mode, Main* src)
{
    this->src=src;
    this->mode=mode;
}

void MenuAction::action(Component* c)
{
    if(mode==G_NONE)return; //do nothing
   
    Menu* m = (Menu*)c;
   
    m_err ret = src->addComp(mode, m->getActionpos().x, m->getActionpos().y);
    if(ret==COMP_FAIL_ERR)
    {
        DERROR("add comp failed");
        return;
    }
    if(ret==NO_COMP_ADDED)return; //nothing to do
   
    SoundComp* last = (SoundComp*)src->getSelectedComp();
    if(last==NULL)
    {
        //this might happen; if deleting selected comp and say keyboard present
        //then addComp will not add anything but it will neither fail so here we are then...return
        return;
    }
   
    if(last->getSCType()==G_WAV)
    { 
        GWav* g = (GWav*)last;
        g->loadPath(src->progstate.getSamplePath());
    }
   
    src->setLastMenuAdd(mode);
}


/************/
/*MenuAction*/
/************/

MenuCmdAction::MenuCmdAction(const string& cmd, Main* src)
{
    this->src=src;
    this->cmd=cmd;
}

void MenuCmdAction::action(Component* c)
{
    src->pushCmd(cmd);
}




/*********/
/*MENUDIR*/
/*********/

C_MenuDir::C_MenuDir(Main* src)
{
    this->src=src;
}

void C_MenuDir::action(Component* c /*=NULL*/)
{
    const ProgramState& progstate = ProgramState::instance();
   
    //ActionDir is absolute dir to module path:
    //extra prefix needs to be a part of the module name: saved for later use: so that extra path is saved in name
    //remove base_prefix:
    string extra = getActionDir().substr(progstate.getModuleLoadPath().length());
    if(extra.size()!=0 && *(extra.end()-1) != '/')extra += "/";
   
    string fname = extra+getActionName();
   
    /*cout << extra << endl;
      cout << getActionName();*/
   
   
    /*FIXMEFIXMEFIXME*/
    /*copypaste from cmd.h! NOT GOOD*/
   
   
    //FIXME:maybe fix setCurrentLoadVersion()
   
   
    if(progstate.getModuleLoadPath().size()==0)
    {
        return;
    }
   
    //only loads in module path
    SoundComp* bridge;
   
    m_err err = loadModuleBridge(fname, progstate.getModuleLoadPath(), src->f, &bridge, src->getMIDIcall(), src->getGround());
   
    if(err!=OK)
    {
        if(err==FILE_NOTFOUND_ERR)
        {
            cout << "(W) file missing: (should not happen)" << fname.c_str() << endl;
            return;
        }
        if(err==BRIDGE_NOTFOUND_ERR)
        {
            cout << "(E) MODULEBRIDGE not found, abort" << endl;
            return;
        }
        if(err==FILE_IO_ERR)
        {
            cout << "(E) file error, abort" << endl;
            return;
        }
        if(err==SOME_COMP_FAIL)
        {
            cout << "(W) some comp(s) failed to load" << endl;
        }
        if(err==SOME_STATE_FAIL)
        {
            cout << "(W) some state(s) failed to load" << endl;
        }
        if(err==FILE_CORRUPT_ERR)
        {
            cout << "(E) file corrupted, failed to load" << endl;
            return;
        }
    }
   
    DASSERT(bridge!=NULL); 
   
    DirReaderMenu* m = (DirReaderMenu*)c;
    src->addComp(bridge, m->getActionpos().x, m->getActionpos().y);
   
}


/*********/
/*TEMPDIR*/
/*********/

C_TempDir::C_TempDir(Main* src)
{
    this->src=src;
}

void C_TempDir::action(Component* c /*=NULL*/)
{
    string extra = getActionDir();//.substr(src->getPrefix().length());
    if(extra.size()!=0 && *(extra.end()-1) != '/')extra += "/";
   
    string s = getActionName();

    src->pushCmd("load_abs " + extra+s);
}


/*********/
/*MENUDIR*/
/*********/

C_FileDir::C_FileDir(Main* src)
{
    this->src=src;
}

void C_FileDir::action(Component* c /*=NULL*/)
{
    //const ProgramState& progstate = ProgramState::instance();
   
    //ActionDir is absolute dir to module path:
    //extra prefix needs to be a part of the module name: saved for later use: so that extra path is saved in name
    //remove base_prefix:
    /*string extra = getActionDir();
      if(extra.size()!=0 && *(extra.end()) != '/')extra += "/";
   
      string fname = extra+getActionName();*/
   
   
    /*FIXMEFIXMEFIXME*/
    /*copypaste from cmd.h! NOT GOOD*/
   
   
   
    //only loads in module path
    /*
      src->con->show("load: %s",fname.c_str());
   
      m_err err = src->loadProgram(fname, progstate.getForceLoad());
      if(err!=OK)
      {
      if(err==FILE_NOTFOUND_ERR)src->con->show("no such file:%s",fname.c_str()); 
      if(err==COMP_FAIL_ERR)src->con->show("comp(s) load fail");
      if(err==FILE_CORRUPT_ERR)src->con->show("file corrupt: %s",fname.c_str());;
      if(err==CON_FAIL_ERR)src->con->show("(E) line src->connection(s) failed");
      if(err==FILE_IO_ERR)src->con->show("(E) file io error");
      if(err==ERROR)src->con->show("(W) something wrong happened");
      ERROR("load error");
      }
   
      src->setLastLoad(getActionName());*/
    //cout << "load:" << getActionName() << endl;
   
    //get "extra" prefix i.e part 
    string extra = getActionDir().substr(src->getPrefix().length());
    if(extra.size()!=0 && *(extra.end()-1) != '/')extra += "/";
   
    string s = getActionName();
    s = s.substr(0, s.find_last_of("."));
    src->pushCmd("load " + extra+s);
}


void Main::pushCmd(const string& cmd)
{
    cmd_queue.push(cmd);
}

   
//used to "upload"
bool Main::haveNextCmd()
{
    return !cmd_queue.empty();
}

string Main::getNextCmd()
{
    DASSERT(!cmd_queue.empty());
   
    string c = cmd_queue.front();
    cmd_queue.pop();
   
    return c;
}

void Main::setMIDI(MIDI* midi)
{
    this->midi=midi;
}
