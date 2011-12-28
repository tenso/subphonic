#include "m_main_help.h" 

ProgramFile::ProgramFile()
{
    //start minimum
   
    head.name[0]='k';
    head.name[1]='w';
   
    head.ver=FILEFORMAT_VERSION;
   
    head.state_size=sizeof(ProgramFile)-sizeof(Head);
   
    //end minimum
   
    //progstate
   
}


/*LOADING*/

m_err loadModuleBridge(const string& name, const string& prefix, BitmapFont* f, 
SoundComp** bridge_ret, MIDIcall* midicall, GroundMixer& ground, bool try_force)
{
    (*bridge_ret)=NULL;
   
    string fname = prefix+name;
   
    //now condense the complete prefix into 'prefix'
    string c_prefix = prefix;
    size_t sub_pf = name.find_last_of("/");
    if(sub_pf!=string::npos)
        c_prefix = prefix + name.substr(0,sub_pf+1);
   
    VERBOSE1(cout << "loading: " << fname << endl;);
   
    ifstream in(fname.c_str() ,ios::binary);
    if(!in)
    {
        return FILE_NOTFOUND_ERR;
    }
   
    //read head
   
    ProgramFile filestate;
   
    //read head
    in.read((char*)&filestate.head, sizeof(ProgramFile::Head));
    if(!in.good())
    {
        DERROR("read failed");
        in.close();
        return FILE_IO_ERR;
    }
   
    //must save old, this might be recursive
    ProgramState& progstate=ProgramState::instance();
   
    int old_lver = progstate.getCurrentLoadVersion();
    progstate.setCurrentLoadVersion(filestate.head.ver);
   
    if(filestate.head.name[0]!='k' && filestate.head.name[1]!='w')
    {
        DERROR("file missmatch or corrupt:" << fname);
	
        progstate.setCurrentLoadVersion(old_lver);
        return FILE_CORRUPT_ERR;
    }
   
   
    int want = sizeof(ProgramFile)-sizeof(ProgramFile::Head);
    int rest = filestate.head.state_size;
   
    int c_sel_slot=0;
   
    if(want!=rest)
    {
        DERROR("state on file size missmatch, skipping:" << fname);
        in.seekg(rest, ios::cur);
    }
    else
    {
        in.read((char*)(&filestate)+sizeof(ProgramFile::Head), want);
        if(!in.good())
        {
            DERROR("read failed" << fname);
            in.close();
	     
            progstate.setCurrentLoadVersion(old_lver);
            return FILE_IO_ERR;
        }
	
        c_sel_slot = filestate.current_slot;
    }
   
    //read data
   
    Container* cont = new Container();
    Connection* conn = new Connection();
    BenchStates* states = new BenchStates(); 
   
    //because all loaded stuff live in own containers sc_off should be 0
    //BUT bridge need to have conforming id so do this now, not bad atleast
    int sc_off=SoundComp::getSeqCount();
   
    SoundComp* b;  //load comps first
    m_err err = loadComponentsModule(cont, f, &in, sc_off, &b, midicall, c_prefix, states, filestate, ground);
   
    bool ok=true;
    if(err!=OK)
    {
        if(err==SOME_COMP_FAIL)
        {
        }
        else ok=false;
    }
   
    if(ok)
    {
        DASSERT(b!=NULL);
	
        err = loadConnections(cont, conn, &in, sc_off);
        if(err!=OK)
        {
            if(err==SOME_CON_FAIL) //still try to continue
            {
            }
            else ok=false;
        }
    }
    if(ok)
    {
        err = loadStatesModule(cont, states, &in, sc_off, c_sel_slot, try_force);
	
        //convert state versions?
        if(filestate.head.ver != FILEFORMAT_VERSION)
        {
            /*cout << endl << "WARNING: states in module file is old version; converting to new(for this session);" <<
              "consider loading module and running 'save' to update it: " << name << endl;
            */
	     
            //also in: m_main.cpp
            //make this a silent operation: states will change, cant re save all everytime,
            //BUT make sure setState()-s handle different verions!
	     
            states->resaveAllInCurrLoadVer(filestate.head.ver);
        }
	
        if(err!=OK)
        {
            if(err==SOME_STATE_FAIL) //try to continue on some state fail
            {
                cout << endl << "some STATE failed  in: " << name << endl << endl;
            }
            else ok=false;
        }
    }
   
    //done with file
    in.close();
   
    if(!ok)
    {
        delete cont;
        delete conn;
        delete states;
        /*VERBOSE1(*/cout << "(E) bridge load failed!" << endl;//);
	
        progstate.setCurrentLoadVersion(old_lver);
        return err;
    }
   
   
    DASSERT(b!=NULL);
   
    GBridge* bridge = (GBridge*)b;
   
    //must remove bridge duplicates
    states->removeCompStates(bridge);//FIXMENOW: need this?
    cont->rem(bridge);
   
    bridge->setContent(cont,conn,states);
   
    bridge->flipComp(name); //flip in->out  & viceversa
    bridge->setHome(bridge);//WARNING:check me out
   
    //states are resaved must do this before applying final(setCurrent...):
    progstate.setCurrentLoadVersion(old_lver);
   
    //apply the selected state(seeking left):
    states->applyAll(filestate.current_slot, true);
   
    (*bridge_ret)=bridge;
   
    progstate.setCurrentLoadVersion(old_lver);
    return err;
}



/*CONNECTIONS*/

m_err loadConnections(Container* cont, Connection* connection, istream* fin, int sc_seq_off)
{
    DASSERT(fin!=NULL);
   
    m_err ret=OK;
   
    //bytes saved
    uint have_bytes;
    fin->read((char*)&have_bytes, sizeof(int));
   
    if(!fin->good())
    {
        DERROR("(E) read data");
        return FILE_IO_ERR;
    }
   
   
    bool only_cp=false;
    uint skip_bytes=0;
   
    uint want_bytes = sizeof(ConPair)+sizeof(Uint8)*3;
   
    if(have_bytes!=want_bytes)
    {
        if(have_bytes>=sizeof(ConPair))
        {
            DERROR("(W) Data size disagree...continue read");
            only_cp=true;
            skip_bytes=have_bytes-sizeof(ConPair);
        }
        else 
        {
            DERROR("(E) Line data size disagree...FAIL");
            return CON_FAIL_ERR;
        }
    }
   
    int n,l;
    fin->read((char*)&n, sizeof(int));
   
    if(!fin->good())
    {
        DERROR("(E) read data");
        return FILE_IO_ERR;
    }
   
    VERBOSE1(cout << "loading " << n << " lines...";);
    cout.flush();
    l=n;
    bool all_ok=true;
    Uint8 colorbytes[3];
    SDL_Color color;
   
    while(n>0)
    {
        //read con data
        ConPair cp;
        fin->read((char*)&cp, sizeof(ConPair));
        if(!fin->good())
        {
            DERROR("(E) read data");
            return FILE_IO_ERR;
        }
	
        if(only_cp)
        {
            //skip the rest
            fin->seekg(skip_bytes , ios_base::cur);
        }
        else
        {
            //read color
            fin->read((char*)colorbytes, sizeof(Uint8)*3);
            if(!fin->good())
            {
                DERROR("(E) read data");
                return FILE_IO_ERR;
            }
            color.r=colorbytes[0];
            color.g=colorbytes[1];
            color.b=colorbytes[2];
            connection->addColor(color);
        }
	
        cp.offsetSeq(sc_seq_off);
	
        if(addConnection(cont, connection, cp)==false)
        {
            all_ok=false;
            ret=SOME_CON_FAIL;
            ERROR(cout << "load line " << l-n <<" failed, seq_in:" << cp.seq_in << " seq_out: " << cp.seq_out << endl);
	     	     
            SoundComp* in = findComp(cont, cp.seq_in);
            SoundComp* out = findComp(cont, cp.seq_out);
	     
            if(in!=NULL)ERROR(cout << "seq IN comp is: " << in->name() << " at:" << in->getPos().x << "," << in->getPos().y);
            else ERROR(cout << "seq IN comp does not exist");
	     
            if(out!=NULL)ERROR(cout << "seq OUT comp is: " << out->name() << " at:" << out->getPos().x << "," << out->getPos().y);
            else ERROR(cout << "seq OUT comp does not exist");
        }
        n--;
    }
   
    //MUST DO THIS
    connection->defaultAddColor();
   
    VERBOSE1(if(all_ok)cout << "ok" << endl;else cout << endl;);
   
    return ret;
}


m_err saveConnections(Connection* connection, ostream* of)
{
    DASSERT(of!=NULL);
   
    con_list* lines = connection->getList();
   
    int n=lines->size();
   
    VERBOSE1(cout << "saving " << n << " lines...";);
    cout.flush();
   
    //write expected
    int s=sizeof(ConPair)+sizeof(Uint8)*3;
    of->write((char*)&s,sizeof(int));
    if(!of->good())
    {
        DERROR("(E) write data");
        return FILE_IO_ERR;
    }
   
    //write num lines
    of->write((char*)&n,sizeof(int));
    if(!of->good())
    {
        DERROR("(E) write data");
        return FILE_IO_ERR;
    }
   
    for(con_list_it it = lines->begin();it!=lines->end();it++)
    {
        LineData* data = *it;
	
        //write con data
        ConPair cp = data->getConPair();
        of->write((char*)&cp, sizeof(ConPair));
        if(!of->good())
        {
            DERROR("(E) write data");
            return FILE_IO_ERR;
        }
        //write color
        of->write((char*)&data->color, sizeof(Uint8)*3);
        if(!of->good())
        {
            DERROR("(E) write data");
            return FILE_IO_ERR;
        }
    }
    of->flush();
   
    VERBOSE1(cout << "ok" << endl;);
   
    return OK;
}

bool addConnection(Container* cont, Connection* connection, ConPair cp)
{
    //get components
   
    SoundComp* sci=NULL;
    SoundComp* sco=NULL;
   
    comp_list* list = cont->getList();
   
    for(comp_list_it it = list->begin();it!=list->end();it++)
    {
        SoundComp* tmp = (SoundComp*)*it;
        if(tmp->getSeqNum()==cp.seq_out)sco=tmp;
        if(tmp->getSeqNum()==cp.seq_in)sci=tmp;
	
        if(sco!=NULL && sci!=NULL)break;
    }
   
    if(sci==NULL || sco==NULL)
    {
        ERROR(cout << "input/output component not found");
        return false;
    }
   
    //get input/putput from comp's
    InputTaker* in = sci->getInputTaker(cp.seq_in_id);
    OutputGiver* out = sco->getOutputGiver(cp.seq_out_id);
   
    if(in==NULL || out==NULL)
    {
        if(in==NULL)
        {
            ERROR(cout << "input taker not found, wanted id:" << cp.seq_in_id << endl);
        }
        if(out==NULL)
        {
            ERROR(cout << "output giver not found, wanted id:" << cp.seq_out_id << endl);
        }
	
	
        return false;
    }
   
    if(out->compID()==OUTPUT_GIVER && in->compID()==INPUT_TAKER)
    {
        connection->startCon(out);
        connection->finishCon(in);
	
        return true;
    }
   
    return false;
}

/* COMPONENTS*/

m_err saveComponents(Container* cont, ostream* of)
{
    DASSERT(of!=NULL);
   
    comp_list* list = cont->getList();
   
    int s = sizeof(DiskComponent::Data);
    of->write((char*)&s,sizeof(int));
    if(!of->good())
    {
        DERROR("(E) write data");
        return FILE_IO_ERR;
    }
   
    int n=list->size();
    of->write((char*)&n,sizeof(int));
    if(!of->good())
    {
        DERROR("(E) write data");
        return FILE_IO_ERR;
    }
   
    VERBOSE1(cout << "saving " << n << " components...";   cout.flush(););
   
    for(comp_list_it it = list->begin();it!=list->end();it++)
    {
        SoundComp* comp = (SoundComp*)*it;
	
        DiskComponent dc = comp->getDiskComponent();
        DiskComponent::Data data = dc.getData();
	
        /*	VERBOSE2(cout << endl << "  * saving comp... id:" << dc.sc_type << " pos(" << dc.pos.x <<\
            ","<<dc.pos.y<<") seqnum: " << dc.seq << endl;);*/
	
        of->write((char*)&data, sizeof(data));
        if(!of->good())
        {
            DERROR("(E) write data");
            return FILE_IO_ERR;
        }
	
        //save appendix data?
        if(dc.haveAppendix())
        {
            VERBOSE1(cout << "write appendix" << endl;);
            //the size was already written in 'data'
            of->write((char*)dc.getAppendixData(), dc.getAppendixSize());
        }
	
    }
   
    of->flush();
    VERBOSE1(cout << "ok" << endl;);
   
    return OK;
}

/*RETURNS MODULEBRIDGE, if one is found*/
m_err loadComponentsModule(Container* cont, BitmapFont* f, istream* fin, int sc_seq_off
, SoundComp** ret, MIDIcall* midicall, const string& load_prefix
, BenchStates* states, const ProgramFile& filestate, GroundMixer& ground)
{
    DASSERT(fin!=NULL);
   
    m_err err=OK;
   
    *ret=NULL;
   
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
    VERBOSE1(cout << "loading " << n << " comps...";);
    cout.flush();
    l=n;
   
    bool all_ok=true;
    unsigned int max_seq=0;
   
    for(int i=0; i<n; i++)
    {
        DiskComponent::Data data;
        fin->read((char*)&data, sizeof(data));
        if(!fin->good())
        {
            DERROR("(E) read data");
            return FILE_IO_ERR;
        }
        //SPECIAL COMPS
	
        //DONT load keyboard or play in module
        if(data.sc_type==G_PLAY)
        {
            VERBOSE2(cout << "skip play" << endl;);
            DASSERT(data.appendix_len==0);
            continue;
        }
        if(data.sc_type==G_KEYBOARD)
        {
            VERBOSE2(cout << "skip keyb" << endl;);
            DASSERT(data.appendix_len==0);
            continue;
        }
	
        /*if(data.sc_type==G_STATETICK)
          {
          VERBOSE2(cout << "skip state" << endl;);
          DASSERT(data.appendix_len==0);
          continue;
          }*/
	
        //END DONT
	
        //END SPECIAL
	
        bool do_appendix=false;
        Uint8* appendix=NULL;
	
        if(data.appendix_len>0)
        {
            do_appendix=true;
            VERBOSE1(cout << "read appendix: " << endl;);
            appendix = new Uint8[data.appendix_len];
            fin->read((char*)appendix, data.appendix_len);
        }
	
	
        //to conform to program running seq_no
        data.offsetSeq(sc_seq_off);
	
        SoundComp* last;
        if( (last = newComponent(cont, f, data.sc_type, data.pos.x, data.pos.y, ground)) ==NULL)
        {
            all_ok=false;
            cout << endl << " component " << l-n <<" failed ";
            cout << " pos:" << data.pos.x << " " << data.pos.y << endl;
            err=SOME_COMP_FAIL;
            continue;
        }
        //reset seqno
        last->setSeqNum(data.seq);
	
        if(do_appendix)
        {
            last->setAppendix(appendix, data.appendix_len);
            delete[] appendix;
	     
            if(last->getSCType()==G_WAV)
            {
                GWav* g = (GWav*)last;
                g->loadByAppendix(load_prefix);
            }
	     
            //must treat module bridge different, if here it means that module contains modules of it's own
            if(last->getSCType()==G_BRIDGE)
            {
                GBridge* bridge = (GBridge*)last;
                VERBOSE2(cout << "modbridge in modbridge load: " << bridge->getFilename() << endl;);
		  
                if(bridge->wantFlip())
                {
                    VERBOSE2(cout << endl << "found flipped iobridge component" << endl;);
		       
                    SoundComp* b;
		       
		       
                    //only load from load prefix
                    //m_err err = loadModuleBridge(bridge->getFilename(), load_prefix, f, &b, midicall);
		       
                    //1.prefix
                    //2.mod path
                    ProgramState& progstate=ProgramState::instance();
                    m_err err = loadModuleBridge(bridge->getFilename(), load_prefix, f, &b, midicall, ground);
                    if(err==FILE_NOTFOUND_ERR)
                    {
                        err = loadModuleBridge(bridge->getFilename(), progstate.getModuleLoadPath(), f, &b, midicall, ground);
                    }
		       
		       
                    if(err!=OK)
                    {
                        stringstream errmess;
                        errmess << "modbridge fail: ";
                        if(err==BRIDGE_NOTFOUND_ERR)
                            errmess << "MODULEBRIDGE not found, abort" << endl;
                        if(err==FILE_NOTFOUND_ERR)
                            errmess << "file missing: " << bridge->getFilename() << " (is modulepath correct?) " << endl;
                        if(err==FILE_IO_ERR)
                            errmess << "file error, abort" << endl;
                        if(err==FILE_CORRUPT_ERR)
                            errmess << "file corrupt, abort" << endl;
                        else
                            errmess << " err num: " << err << endl;
			    
                        DERROR(errmess.str());
                    }
                    else
                    {
                        DASSERT(bridge!=NULL); 
                        cont->add(b, bridge->pos.x, bridge->pos.y);
			    
                        //b has taken bridge's place, inherit its id's
                        b->setSeqNum(bridge->getSeqNum());
			    
                        //done with 'bridge'
                        delete (Container*)cont->rem(last); 
                    }
                }//end want flip
                else
                {
                    VERBOSE2(cout << endl << "found unflipped IOBRIDGE component" << endl;);
                    *ret = last;
                }
            }//end G_BRIDGE
	     
        }//end do_appendix
	
	
	
        //LAST FOR THIS COMP: DO SOME SPECIAL STUFF?
	
        GStateTick* st;
	
        switch(last->getSCType())
        {
            case G_MIDIKEY:
                midicall->addKey((GMIDIkey*)last);
                break;
            case G_MIDICTRL:
                midicall->addCtrl((GMIDIctrl*)last);
                break;
            case G_MIDIPGM:
                midicall->addPgm((GMIDIpgm*)last);
                break;
	     
            case G_STATETICK:
                /*HERE*/
                st = (GStateTick*)last;
                st->setBenchStates(states, filestate.current_slot, filestate.start_marker,
				filestate.stop_marker, MainBench::MAX_STATES/*hm*/);
                break;
	     
            default:
                break;
        }
	
        if(data.seq>max_seq)max_seq=data.seq;
    }
   
    SoundComp::setSeqCount(max_seq+1);
   
    VERBOSE1(if(all_ok)cout << "ok" << endl; else cout << endl;);
   
    if(*ret==NULL)err=BRIDGE_NOTFOUND_ERR;
   
    return err;
}

SoundComp* newComponent(Container* cont, BitmapFont* f, SCType id, int x, int y, GroundMixer& ground)
{
    SoundComp* last = SCFactory(id, x, y, f, ground);
    if(last==NULL)
    {
        DERROR("got NULL");
        return NULL;
    }
   
    cont->add(last, x, y);
   
    //SoundComp* last = (SoundComp*)cont->getLast();
    //FIXME:this should be hard in class no need to set!
   
    //last->setSCType(id);
   
    return last;
}

//STATES


m_err saveAllStates(BenchStates* states, ostream* out)
{
    DASSERT(out!=NULL);
   
    unsigned int unused = 0;//states->numSlots(); //states->nstate;
   
    //VERBOSE1(cout << "save for " << max_slots << " slots" << endl;);
   
    out->write((char*)&unused, sizeof(int));
    if(!out->good())
    {
        DERROR("(E) write data");
        return FILE_IO_ERR;
    }
   
    int n = states->numCompStates();
   
    VERBOSE1(cout << "saving " << n << " different component's states " << endl;);
   
    out->write((char*)&n,sizeof(int));
    if(!out->good())
    {
        DERROR("(E) write data");
        return FILE_IO_ERR;
    }
   
    state_map* compstat = states->getAllStates();
    //iterate over comp's
    for(state_map_it it = compstat->begin();it!=compstat->end();it++)
    {
        CompStates* next = it->second;
	
        /*if(next->getNum()!=max_slots)
          {
          DERROR("CompState dissagree with BenchStates(NOT GOOOD!) skipping");
          continue;
          }*/
	
        m_err err = appendStateToFile(out, next);
        if(err!=OK)
        {
            DERROR("append state failed: " << err);
            break;
        }
    }
    out->flush();
   
    return OK;
}

m_err appendStateToFile(ostream* out, /*const*/ CompStates* state)
{
    DASSERT(out!=NULL);
   
    //TYPE
    SoundComp* parent = state->getParent();
    DASSERT(parent!=NULL);
   
    ios_base::iostate is = out->exceptions();
    out->exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
   
    try
    {
        SCType ptype = parent->getSCType();
        out->write((char*)&ptype, sizeof(SCType));
	
	
        //SEQNO
        int parent_seqno = parent->getSeqNum();
        out->write((char*)&parent_seqno, sizeof(int));
	
	
        //NUM STATES
        int count=state->getNumUsed();
	
        out->write((char*)&count, sizeof(int));
	
        //loop over all, saving 
        CompStates::s_map map = state->getStates();
	
        for(CompStates::s_it it= map.begin();it!=map.end();it++)
        {
	     
            SCState* s = it->second;
            DASSERT(s!=NULL);
	     
            SoundCompEvent* event = s->getEvent();
            DASSERT(event!=NULL);
	     
            DASSERT(!event->empty());
	     
            //SIZE OF STATE
            int len = event->getSize();
            if(len==0)
            {
                DERROR("save state of size 0");
            }
	     
            out->write((char*)&len, sizeof(int));
	     
            //INDEX NUM
            int index = it->first;
	     
            out->write((char*)&index, sizeof(int));
	     
            //DATA
            if(event->empty() && len!=0)
            {
                cout << "no data! saving empty instead: " << parent->name() << endl;
		  
                parent->getState(event);
                len = event->getSize();
		  
                DASSERTP(!event->empty(), "really WRONG");
            }
	     
            if(len!=0)
            {
                const char* data = event->getData();
		  
                out->write(data, len);   
            }
	     
        }
    }
    catch(ios_base::failure e)
    {
        DASSERT(!out->good());
        DERROR("(E) FILE IO:");
	
        if(out->bad())
        {
            cout << "badbit" << endl;
        }
        else if(out->fail())
        {
            cout << "failbit" << endl;
        }
        else if(out->eof())
        {
            cout << "eofbit" << endl;
        }
        else
        {
            cout << "REALLY WRONG" << endl;
        }
	
        out->exceptions(is);
        return FILE_IO_ERR;
    }
   
    out->exceptions(is);
   
    return OK;
}

m_err loadStatesModule(Container* cont, BenchStates* states, istream* in
, int sc_seq_off, int sel_slot_index, bool try_force)
{
    DASSERT(in!=NULL);
   
    int unused;
    in->read((char*)&unused, sizeof(int));
    if(!in->good())
    {
        DERROR("(E) read data");
        return FILE_IO_ERR;
    }
   
    //VERBOSE1(cout << "load for " << max_slots << " state slots(bench)" << endl;);
    //VERBOSE1(cout << "program wants: " << states->numSlots() << endl;);
   
    //NUM
    int n;
    in->read((char*)&n, sizeof(int));
    if(!in->good())
    {
        DERROR("(E) read data");
        return FILE_IO_ERR;
    }
    VERBOSE1(cout << "loading " << n << " different component's states" << endl;);
   
    //load each comp's states
   
    m_err ret=OK;
   
    //read n states 
    for(int i=0;i<n;i++)
    {
        switch(loadSingleState(cont, states, in, sc_seq_off, SKIP_PROG_SINGLE_COMPS, try_force))
        {
            case FILE_CORRUPT_ERR:
                DERROR("(E) state file corrupt, discontinue");
                return FILE_CORRUPT_ERR;
                break;
	     
            case FILE_IO_ERR:
                DERROR("(E) file IO error, discontinue");
                return FILE_IO_ERR;
                break;
	     
            case COMP_NOTFOUND:
                DERROR("(E) state load fail, continue");
                ret=SOME_STATE_FAIL;
                break;
	     
            case STATE_NOTLOADED:
                ret=SOME_STATE_FAIL;
                break;
	     
            case STATE_SKIP:
                //NOT an error
                break;
	     
            case OK:
                break;
	     
            default:
                cout << "(E) (modulestate)...something happend... " << endl;
                break;
        }
    }
   
    VERBOSE1(cout << "states loaded, setting #1...";cout.flush(););
   
    states->applyAll(sel_slot_index, true/*seek backwards*/);
   
    VERBOSE1(cout << "ok" << endl;);
   
    return ret;
}



m_err loadAllStates(Container* cont, BenchStates* states, istream* in, int sc_seq_off, bool try_force)
{
    DASSERT(in!=NULL);
   
    int unused;
    in->read((char*)&unused, sizeof(int));
    if(!in->good())
    {
        DERROR("(E) read data");
        return FILE_IO_ERR;
    }
   
    //VERBOSE1(cout << "load for " << max_slots << " state slots(bench)" << endl;);
    //VERBOSE1(cout << "program wants: " << states->numSlots() << endl;);
   
    //NUM
    int n;
    in->read((char*)&n, sizeof(int));
    if(!in->good())
    {
        DERROR("(E) read data");
        return FILE_IO_ERR;
    }
    VERBOSE1(cout << "loading " << n << " states" << endl;);
   
    m_err ret=OK;
   
    //load each comp's states
    for(int i=0;i<n;i++)
    {
        switch(loadSingleState(cont, states, in, sc_seq_off, ALL, try_force))
        {
            case FILE_CORRUPT_ERR:
                DERROR("(E) state file corrupt, discontinue");
                return FILE_CORRUPT_ERR;
                break;
	     
            case FILE_IO_ERR:
                DERROR("(E) file IO error, discontinue");
                return FILE_IO_ERR;
                break;
	     
            case COMP_NOTFOUND:
                DERROR("(E) state load fail, continue");
                break;
	     
            case STATE_NOTLOADED:
                ret=SOME_STATE_FAIL;
                break;
	     
            case STATE_SKIP:
                //this is ok
                break;
	     
            default:
                //DERROR("something REALLY wrong");
                break;
        }
    }
   
    return ret;
}



m_err loadSingleState(Container* cont, BenchStates* states, istream* in, 
int sc_seq_off, LOADMODE mode, bool try_force)
{
    DASSERT(in!=NULL);
    DASSERT(states!=NULL);
   
    SoundComp* parent=NULL;
   
    //TYPE
    SCType ptype;
    in->read((char*)&ptype, sizeof(SCType));
   
    //SEQNUM
    int parent_seqno;
    in->read((char*)&parent_seqno, sizeof(int));
   
    //+OFFSET(program's current num)
    parent_seqno+=sc_seq_off;
   
   
    ios_base::iostate is = in->exceptions();
    in->exceptions(~ios_base::goodbit);
   
    bool skip_ahead=false;
   
    try
    {
        //COUNT of event's to load from disk:
        int count=0;
        in->read((char*)&count, sizeof(int));
	
        bool dry_read=false;
        if(mode==SKIP_PROG_SINGLE_COMPS)
        {
            if(ptype==G_PLAY || ptype==G_KEYBOARD || ptype==G_STATETICK)dry_read=true;
            else parent = findComp(cont, parent_seqno);
        }
        else
        {
            parent = findComp(cont, parent_seqno);
        }
	
        if(dry_read)
        {
            skip_ahead=true;
        }
        else
        {
            if(parent==NULL)
            {
                ERROR(cout << "component not found for state, seq_no: " << parent_seqno);
                skip_ahead=true;
            }
	     	     
            if(parent != NULL && parent->getSCType()!=ptype)
            {
                ERROR(cout << "comp expected from file not same as program have to offer, state not loaded");
                skip_ahead=true;
            }
        }
	
        if(skip_ahead)
        {
            //try to read data, as not to corrupt for all following states...
            for(int i=0;i<count;i++)
            {
                //read event size:
                int disk_len;
                in->read((char*)&disk_len, sizeof(int));
		  
                if(disk_len==0)
                {
                    DERROR("load size 0");
                    in->exceptions(is);
                    return STATE_NOTLOADED;
                }
		  
		  
                unsigned int index;
                in->read((char*)&index, sizeof(int));
		  
                //discard data
                in->seekg(disk_len, ios::cur);
		  
            }
	     
            in->exceptions(is);
	     
            if(!dry_read)
            {
                return COMP_NOTFOUND;
            }
            return STATE_SKIP; //not an error
        }
	
	
	
        //continue normal read
	
        //get state hold for this component, or make new if it does not exist allready:
        CompStates* current=states->getCompStates(parent);
	
        if(current==NULL) //not found, make new state
        {
            //this happends when module is loading, otherwise addcomp will have made a compstate
	     
            current = new CompStates(parent);
            states->addCompStates(parent, current);
        }
        else
        {
            current->reset();
        }
        //states->setCurrentCompStates(current);
	
	
        //load event's
        for(int i=0;i<count;i++)
        {
            //SoundCompEvent* event = current_state->getState(i)->event;
	     
            //read event size:
            int disk_len;
            in->read((char*)&disk_len, sizeof(int));
	     
            if(disk_len==0)
            {
                DERROR("load size 0");
            }
            /*if(disk_len>1024*1024*)
              {
              DERROR("WARNING: BIG state(corrupt?)");
              }*/
            if(disk_len<0)
            {
                DERROR("ERROR: file corrupt, disk len < 0");
                disk_len=0;
            }
	     
            unsigned int index;
            in->read((char*)&index, sizeof(int));
	     
            SCState* s = current->getState(index);
	     
            DASSERT(s!=NULL);
	     
            SoundCompEvent* event = s->getEvent();
            DASSERT(event!=NULL);
	     
            //can't trust the size of event->data, load might want more/less
	     
	     
            if(disk_len!=0)
            {
                char* data= new char[disk_len];
		  
                in->read(data, disk_len);
		  
                event->copyData(data, disk_len);
                delete[] data;
            }
	     
        }
    }
    catch(ios_base::failure e)
    {
        DASSERT(!in->good());
        DERROR("(E) FILE IO:");
	
        if(in->bad())
        {
            cout << "badbit" << endl;
        }
        else if(in->fail())
        {
            cout << "failbit" << endl;
        }
        else if(in->eof())
        {
            cout << "eofbit" << endl;
        }
        else
        {
            cout << "REALLY WRONG" << endl;
        }
	
        in->exceptions(is);
        return FILE_IO_ERR;
    }
   
    in->exceptions(is);
    return OK;
}


/*OTHER*/

SoundComp* findComp(Container* cont, unsigned int seqno)
{
    comp_list* list = cont->getList();
   
    for(comp_list_it it = list->begin();it!=list->end();it++)
    {
        SoundComp* tmp = (SoundComp*)*it;
        if(tmp->getSeqNum()==seqno)return tmp;
    }
    return NULL;
}

