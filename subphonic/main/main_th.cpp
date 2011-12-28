#include "main_th.h" 

//FIXME:
ProgramState& MasterResourses::progstate = ProgramState::instance();
SurfaceHoldAutoBuild& MasterResourses::pix = SurfaceHoldAutoBuild::instance(); //OK
ResHold<BitmapFont>& MasterResourses::fnt = ResHold<BitmapFont>::instance();

/*****************/
/*MASTERRESOURSES*/
/*****************/

MasterResourses::MasterResourses()
{
    maincomp=NULL;
   
    screen=NULL;
   
    //f=NULL;
    //f1=NULL;
    //fb=NULL;
   
    con=NULL;
   
    dodraw=true;
   
    do_shutdown=false;
    change_master=false;
    do_sync=false;
    do_state_sync=false;
    do_toggle_fullscreen = false;

    master_th_id=0;
    next_master_th=0;
    c_fps=0;
   
    d_fps=0;
   
    change_d_fps=false;
    d_run_fps=0; 
}

MasterResourses::~MasterResourses()
{
}


void MasterResourses::draw()
{
    if(dodraw)
    {
        maincomp->draw();
	
        //if(progstate.showDumpSec())  {}
	
        /*screen->box(1,screen->H()-19-8, 130, screen->H()-1, screen->makeColor(0,0,0));
          screen->wirebox(0,screen->H()-20-8, 131, screen->H(), screen->makeColor(155,0,0));*/
	
        if(progstate.runningDump())
        {
            int done = progstate.dumpLenDone()/(progstate.getSampleRate()*progstate.getNChan());
            int left = (progstate.dumpLenDone()+progstate.dumpLenLeft())/(progstate.getSampleRate()*progstate.getNChan());
            fnt["hud"]->printF(2,screen->H()-26,"dump %d/%d", done, left);
        }
        else
        {
            fnt["hud"]->printF(2,screen->H()-26,"dump      : -/-");
        }
	
        if(progstate.getOn())fnt["hud"]->printF(2,screen->H()-18,"process   : on\nworkspace : %d", master_th_id);
        else fnt["hud"]->printF(2,screen->H()-18,"process   : off\nworkspace : %d", master_th_id);
	
	
        int xo=screen->W()-160;
        int yo=screen->H()-40;
	
        yo-=24+40;
	
        int yofps=yo;
	
        if(progstate.getShowStats())
        {
	     
            /*screen->box(xo+1-2,yo-10+1-2, screen->W()-4, screen->H()-1, screen->makeColor(0,0,0));
              screen->wirebox(xo-2,yo-10-2, screen->W()+1-4, screen->H(), screen->makeColor(155,0,0));*/
	     
            fnt["stat"]->printF(xo,yo-8 ,"time(us) [all th]");
            fnt["stat"]->printF(xo,yo+0 ,"ev:      %.0f", stats_all.ev);
            fnt["stat"]->printF(xo,yo+8 ,"draw(th):%.0f", stats_draw.draw);
            fnt["stat"]->printF(xo,yo+16,"gen:     %.0f", stats_all.gen);
            fnt["stat"]->printF(xo,yo+24,"sleep:   %.0f", stats_all.sleep);
            fnt["stat"]->printF(xo,yo+32,"play:    %.0f", stats_all.play);
            fnt["stat"]->printF(xo,yo+40,"total:   %.0f", stats_all.total);
            if(c_fps>0)
                fnt["stat"]->printF(xo,yo+48,"load:    %d%%", (int)(100*stats_all.load*progstate.getFps()/c_fps));
            else 
                fnt["stat"]->printF(xo,yo+48,"load:    0%%");
	     
            yo+=16;
            fnt["stat"]->printF(xo,yo+56,"time(us) [th]");
            for(uint i=0;i<stats.size();i++)
            {
                fnt["stat"]->printF(xo,yo+64+i*8, "gen %d: %.0f", i, stats[i].gen);
                fnt["stat"]->printF(xo+15*8,yo+64+i*8, "%d%%", (int)(100*stats[i].gen/stats_all.gen));
            }
	     
            //Uint32 t = SDL_GetTicks();
            //uint tl = (uint)(progstate.autotime - (t-progstate.autotick)/1000.0);
            //fnt["stat"]->printF(xo,yo-16,"autosave in:    %d", tl);
	     
            fnt["stat"]->printF(xo,yofps+56,"runfps:   %.0f", c_fps);
            fnt["stat"]->printF(xo,yofps+56+8,"drawfps:   %.0f", d_fps);
        }
        else
        {
            //FIXME: move to bottom of scree
            if(progstate.getShowFps())
            {
                fnt["stat"]->printF(xo,screen->H()-16,"runfps:   %.0f", c_fps);
                fnt["stat"]->printF(xo,screen->H()-8,"drawfps:   %.0f", d_fps);
            }
        }
	
	
        con->draw();
	
        screen->show();
    }
}


void MasterResourses::sdlInput()
{
    in.resetEvent();
   
    SDL_Event event;
    /*
      in.setUnicode(true);
      in.setUseInputStr(true);
    */
    //poll
    while(SDL_PollEvent(&event))
    {
        //SDL_ResizeEvent
        if(event.type==SDL_QUIT)
        {
            do_shutdown=true;
            //progstate.setRun(false);
        }
        else in.setEvent(event);
	
        /*string n;
          if(in.inputStr(n))
          {
          mystr.append(n);
          cout << mystr << endl;
          }*/
    }
}


void MasterResourses::handleInput()
{
    //   static string mystr;
   
    
    /*
      if(in.keySet(SDLK_BACKSPACE, true))
      {
      if(mystr.length()>=2)
      {
      mystr.erase( mystr.end()-2, mystr.end());
      cout << mystr << endl;
      }
      }*/
   
    //INPUT overwrite maincomp begin
   
    //keyboard
   
    /*if( in.keySet(SDLK_HOME, false))
      {
      maincomp->midi->sendEventDirect(maincomp->midi->eventMakeNoteOn(0,60,128),0);
      cout << "on" << endl;
      }
      if( in.keySet(SDLK_END, false))
      {
      maincomp->midi->sendEventDirect(maincomp->midi->eventMakeNoteOn(0,60,0),0);
      cout << "off" << endl;
      }*/
   
   
    if( in.keySet(SDLK_RCTRL, false))
    { 
        if( in.keySet(SDLK_LSHIFT, false))
        {
            if(in.keySet(SDLK_PAGEUP,true))
            {
                maincomp->bench->moveAllStateIndex(1);
            }
            if(in.keySet(SDLK_PAGEDOWN,true))
            {
                maincomp->bench->moveAllStateIndex(-1);
            }
        }
        else
        {
            if(in.keySet(SDLK_PAGEUP,true))
            {
                maincomp->bench->moveCurrentStateIndex(1);
            }
            if(in.keySet(SDLK_PAGEDOWN,true))
            {
                maincomp->bench->moveCurrentStateIndex(-1);
            }
        }
    }//end rctrl
    /*else
      {
      if(in.keySet(SDLK_PAGEUP, true))
      {
      maincomp->bench->seekMarkerListIndex(1, progstate.getStateTickAll(), true);
      }
      if(in.keySet(SDLK_PAGEDOWN, true))
      {
      maincomp->bench->seekMarkerListIndex(-1, progstate.getStateTickAll(), true);
      }
      }*/
   
    if(in.keySet(SDLK_LCTRL, false))
    {
        if(in.keySet(SDLK_LSHIFT, false))
        {
            if(in.keySet(SDLK_f, true))maincomp->bench->delAllForCurrentStateIndex();
        }
    }
   
    if(in.keySet(SDLK_LCTRL, false))
    {
        if(in.keySet(SDLK_s,true))maincomp->bench->saveCurrentStateIndex(); 
        if(in.keySet(SDLK_a,true)) 
        { 
            con->show("");
            con->show("all states saved");
            maincomp->bench->saveAllStateIndex();
        }
	
        if(in.keySet(SDLK_d, true))maincomp->bench->delCurrentStateIndex();
        if(in.keySet(SDLK_e, true))maincomp->bench->delAllStateIndex();

	
        if(in.keySet(SDLK_r, true))
        {
            maincomp->remMouseOver(in.xMouse(), in.yMouse());
        }
	
        if(in.keySet(SDLK_x, true))
        {
            if(maincomp->bench->inSelList()>0)
            {
                maincomp->bench->currentCopySelListToMarkerIndex();
            }
        }
	
        if(in.keySet(SDLK_z, true))
        {
            if(maincomp->bench->inSelList()>0)
            {
                maincomp->bench->allCopySelListToMarkerIndex();
            }
        }
	
        if(in.keySet(SDLK_g, true))
        {
            if(maincomp->bench->inSelList()>0)
            {
                maincomp->bench->currentDelSelList();
            }
        }
	
        if(in.keySet(SDLK_t, true)) 
        {
            if(maincomp->bench->inSelList()>0)
            {
                maincomp->bench->allDelSelList();
            }
        }
	
	
        if(in.keySet(SDLK_c, true))
        {
	     
            StateCopyHold::ERR err = maincomp->stateCopySelected(false);
            if(err==StateCopyHold::NO_SRC)con->show("no selected comp to copy from");
	     
        }
        if(in.keySet(SDLK_p, true))
        {
            StateCopyHold::ERR err = maincomp->stateCopySelected(true);
            if(err==StateCopyHold::NO_SRC)con->show("no selected comp to copy from");
        }
        if(in.keySet(SDLK_v, true))
        {
            StateCopyHold::ERR err = maincomp->statePasteSelected();
            if(err==StateCopyHold::NO_DEST)con->show("no selected comp to paste to");
            if(err==StateCopyHold::NOTSAME_COMP_TYPE)con->show("from and to comps not the same");
            if(err==StateCopyHold::NO_COPY)con->show("unable to paste: no copy");
        }
	
        //DEBUG CMDS
        if(in.keySet(SDLK_i, true))
        {
            cout << "name: " << maincomp->getSelectedComp()->name() << endl;
            cout << "sc-type: " << maincomp->getSelectedComp()->getSCType() << endl;
            cout << "seq num: " << maincomp->getSelectedComp()->getSeqNum() << endl;
            cout << "pos: " << maincomp->getSelectedComp()->getPos().x+maincomp->getCompOff().x << 
                "," << maincomp->getSelectedComp()->getPos().y+maincomp->getCompOff().y << endl;
        }
    }//end lctrl
   
   
    if(in.mButton(1, false) || in.keySet(SDLK_LALT, false))
    {
        maincomp->addCompOff((int)(in.xMouseMove()*progstate.scroll_speed),
        (int)(in.yMouseMove()*progstate.scroll_speed));
	
        //center workspace
        /*if(in.keySet(SDLK_c))
          {
          maincomp->setCompOff(0,0);
          }*/
	
    }
   
    int sc_off=(int)((in.mWheelUp()-in.mWheelDown())*progstate.scroll_speed*20); 
    if(in.keySet(SDLK_LSHIFT))
    {
        maincomp->addCompOff(sc_off, 0);
    }
    else
    {
        maincomp->addCompOff(0, -sc_off);
    }
   
   
   
   
    if(in.keySet(SDLK_LSHIFT))
    {
        //FIXME: make scroll speed
        int off=(int)(-40*60.0/(double)progstate.getFps());
	
        if(in.keySet(SDLK_w))
        {
            maincomp->addCompOff(0, off); 
        }
        if(in.keySet(SDLK_s))
        {
            maincomp->addCompOff(0, -off);
        }
        if(in.keySet(SDLK_a))
        {
            maincomp->addCompOff(off, 0);
        }
        if(in.keySet(SDLK_d))
        {
            maincomp->addCompOff(-off, 0);
        }
    }
   
    if(in.keySet(SDLK_F1, true))
    {
        /*do_sync=true;
          do_state_sync=true;*/
        con->execCmd("sync");
    }
    if(in.keySet(SDLK_F2, true))
    {
        //do_sync=true;
        con->execCmd("sync_comps");
    }
    if(in.keySet(SDLK_F3, true))
    {
        //FIXME: TH SAFETY
        progstate.setOn(!progstate.getOn());
        //con->show("play:%d",progstate.getOn());
    }
    if(in.keySet(SDLK_F4, true))
    {
        con->execCmd("playondev");
    }
    if(in.keySet(SDLK_F5, true))
    {
        con->execCmd("line_fg");
    }
    if(in.keySet(SDLK_F6, true))
    {
        //con->show("*last add replicate");
        SCType t = maincomp->getLastMenuAdd();
        if(t!=G_NONE)
        {
            SDL_Rect r = maincomp->getCompOff();
	     
            //check err
            m_err err = maincomp->addComp(t, in.xMouse()+r.x, in.yMouse()+r.y);
            DASSERT(err==OK);
	     
            //FIXMENOW: sort this mess out(g_wav/m_main/m_main_help
            if(t==G_WAV)
            { 
                GWav* g = (GWav*)maincomp->getSelectedComp();
                DASSERT(g!=NULL);
		  
                g->loadPath(progstate.getSamplePath());
            }
        }
    }
    if(in.keySet(SDLK_LSUPER, false))
    {
        //con->show("*move all");
        int x = (int)(in.xMouseMove()*progstate.scroll_speed);
        int y = (int)(in.yMouseMove()*progstate.scroll_speed);
        maincomp->relMoveAllComps(x, y);
    }
   
    if(in.keySet(SDLK_F7, true))
    {
        con->execCmd("save");
    }
    if(in.keySet(SDLK_F8, true))
    {
        con->execCmd("rload");
    }
      
   
   
    if(in.keySet(SDLK_F9, true))
    {
        con->show("*toggle draw");
        dodraw=!dodraw;
        screen->fill(screen->makeColor(0,0,0));
        SDL_Rect pos;
        pos.x=screen->W()/2-160;
        pos.y=screen->H()/2-100;
        screen->blit(pix["no_draw"], &pos);
        screen->show();
    }
   
    if(in.keySet(SDLK_F10, true))
    {
        con->execCmd("fullscreen");
        //screen->toggleFullScreen();
    }
   
   
    if(in.keySet(SDLK_F11, true))
    {
        ostringstream ss;
        ss << "grab_in " << !progstate.getGrabInput();
        con->execCmd(ss.str());
    }
   
    if(in.keySet(SDLK_F12, true))
    {
        con->show("*shutdown");
        do_shutdown=true;
    }
   
   
   
    if(in.keySet(SDLK_LALT, false))
    {
        for(uint i=0;i<9;i++)
        {
            if(in.keySet((SDLKey)(SDLK_1+i), true))
            {
                //con->show("want workspace:" + toStr(i));
                change_master=true;
                next_master_th=i;
                in.keySet(SDLK_LALT, true);
            }
        }
    }
   
    if(in.keySet(SDLK_ESCAPE, true))
    {
        //con takes over events when down so in.keySet() etc might miss some event's; say a keyRelease
        //so must reset in when entering consoles event handling
        if(dodraw)
        {
            in.reset();
            con->togglescroll();
        }
    }
   
    //INPUT maincomp begin
   
    bool handled = maincomp->handleInput(in);
   
    //maincomp overwrite end
    //components override input for these:
    if(!handled)
    {
        //WARNING: this makes it possible to select keyboard play note select other comp
        //and keyboard keeps on
        if(maincomp->keyboardSelected() || maincomp->keyboardStay())maincomp->giveKeyboard(&in); 
        maincomp->giveJoystick(&in);
	
        if(in.keySet(SDLK_RSUPER, true))
        {
            uint index = maincomp->bench->getCurrentMarkerIndex();
            if(index > 0)
            {
                index-=1;
                //only apply states if shift not pressed
                maincomp->bench->setCurrentMarkerIndex(index, !in.keySet(SDLK_LSHIFT)) ;
            }
        }
        if(in.keySet(SDLK_MENU, true))
        {
            uint index = maincomp->bench->getCurrentMarkerIndex();
	     
            if(index < maincomp->bench->getMaxStates()-1)
            {
                index += 1;
		  
                //only apply states if shift not pressed
                maincomp->bench->setCurrentMarkerIndex(index, !in.keySet(SDLK_LSHIFT)) ;
            }
	     
	     
        }
	
        if(in.keySet(SDLK_HOME, true))
        {
            maincomp->bench->seekMarkerListIndex(1, progstate.getStateTickAll(), true);
        }
        if(in.keySet(SDLK_END, true))
        {
            maincomp->bench->seekMarkerListIndex(-1, progstate.getStateTickAll(), true);
        }
    }
   
    in.resetKeyPressRel(); //some comps use these
}

int MasterResourses::initInput()
{
    //open all joysticks
    int njoy =  in.numJoysticks();

    if(njoy>2)ERROR("num joy > 2 unimplemented!");
   
    for(uint i=0;i<njoy;i++)
    {
        //FIXME: mouse opened as joy
        in.openJoy(i);
    }

    in.setUnicode(1);
    in.setUseInputStr(1);

    return 0;
}

int MasterResourses::freeInput()
{
    int njoy =  in.numJoysticks();
    if(njoy>2)ERROR("num joy > 2 unimplemented!");
   
    for(uint i=0;i<njoy;i++)
    {
        in.closeJoy(i);
    }
   
    return 0;
}


int MasterResourses::initVideo(uint screen_w, uint screen_h, bool useGL, bool fullscreen)
{
    DASSERT(screen==NULL);
   
    //SCREEN
    const SDL_VideoInfo* info = SDL_GetVideoInfo();
    VERBOSE1(cout << "using " << (int)info->vfmt->BitsPerPixel << " bpp" <<endl;);
    int extFlags = 0;
    if (fullscreen)extFlags |= SDL_FULLSCREEN;

    if (useGL)
    {
        cout << "render: opengl" << endl;
        screen = new SDL_GL_Screen(screen_w, screen_h, (int)info->vfmt->BitsPerPixel, extFlags /*SDL_DOUBLEBUF*/);
    }
    else
    {
        cout << "render: software" << endl;
        screen = new SDL_SW_Screen(screen_w, screen_h, (int)info->vfmt->BitsPerPixel, 
                                   SDL_HWSURFACE | SDL_HWACCEL| SDL_DOUBLEBUF | SDL_ANYFORMAT | extFlags);
    }
   
    if(screen != NULL && !screen->ok())
    {
        cout << " init video failed (1)" << endl;
        return -1;
    }

    //SW_SURFACE is better for pixel manip?
   
    if (screen == NULL)
    {
        cout << " init video failed (2)" << endl;
        return -1;
    }
    return 0;
}

int MasterResourses::freeVideo()
{
    if(screen==NULL)return 1;
   
    //delete screen; //DONT! freed by SDL_Quit()
    screen=NULL;
   
    return 0;
}


int MasterResourses::loadFonts()
{
    //FIXME: read from cfg
    ResHold<BitmapFont>& fnt = ResHold<BitmapFont>::instance();
   
    int ret=0;
    //FONT
    string font_bmp = progstate.getDataPrefix() + "fonts/font8x8.bmp";
    string font_fmp = progstate.getDataPrefix() + "fonts/font8x8.fmp";
   
    BitmapFont* load;
   
    load = new BitmapFont(screen);
    load->loadFont(font_bmp.c_str(),font_fmp.c_str(),BitmapFont::ASCII);
    if(load->error())
    {
        cout << load->getError() << endl;
        ret = -1;
    }
    else
    {
        load->setMask(0xff,0xff,0xff);
        //load->setColor(35,255,10);
        load->setColor(155,255,5);
    }
    //fnt.add("green", load);
    fnt.add("label", load);
   
   
    load = new BitmapFont(screen);
    load->loadFont(font_bmp.c_str(),font_fmp.c_str(),BitmapFont::ASCII);
    if(load->error())
    {
        cout << load->getError() << endl;
        ret = -1;
    }
    else
    {
        load->setMask(0xff,0xff,0xff);
        load->setColor(255,0,0);
    }
    fnt.add("red", load);
    fnt.add("hud2", load);
   
   
    load = new BitmapFont(screen);
    load->loadFont(font_bmp.c_str(),font_fmp.c_str(),BitmapFont::ASCII);
    if(load->error())
    {
        cout << load->getError() << endl;
        ret = -1;
    }
    else
    {
        load->setMask(0xff,0xff,0xff);
        load->setColor(180,235,235);
	
    }
    //fnt.add("title", load);
    //fnt.add("menu", load);
   
    fnt.add("con_in", load);
   
   
    load = new BitmapFont(screen);
    load->loadFont(font_bmp.c_str(),font_fmp.c_str(),BitmapFont::ASCII);
    if(load->error())
    {
        cout << load->getError() << endl;
        ret = -1;
    }
    else
    {
        load->setMask(0xff,0xff,0xff);
        load->setColor(0,255,255);
    }
    fnt.add("cyan", load);
   
   
    load = new BitmapFont(screen);
    load->loadFont(font_bmp.c_str(),font_fmp.c_str(),BitmapFont::ASCII);
    if(load->error())
    {
        cout << load->getError() << endl;
        ret = -1;
    }
    else
    {
        load->setMask(0xff,0xff,0xff);
        load->setColor(110,110,110);
	
    }
    //fnt.add("lpurp", load);
    fnt.add("con_out", load);
   
    load = new BitmapFont(screen);
    load->loadFont(font_bmp.c_str(),font_fmp.c_str(),BitmapFont::ASCII);
    if(load->error())
    {
        cout << load->getError() << endl;
        ret = -1;
    }
    else
    {
        load->setMask(0xff,0xff,0xff);
        load->setColor(200,250,250);
    }
    //fnt.add("stat", load);
    fnt.add("track", load);
   
   
    load = new BitmapFont(screen);
    load->loadFont(font_bmp.c_str(),font_fmp.c_str(),BitmapFont::ASCII);
    if(load->error())
    {
        cout << load->getError() << endl;
        ret = -1;
    }
    else
    {
        load->setMask(0xff,0xff,0xff);
        load->setColor(0,0,0);
    }
    fnt.add("black", load);

   
    load = new BitmapFont(screen);
    load->loadFont(font_bmp.c_str(),font_fmp.c_str(),BitmapFont::ASCII);
    if(load->error())
    {
        cout << load->getError() << endl;
        ret = -1;
    }
    else
    {
        load->setMask(0xff,0xff,0xff);
        //load->setColor(170,225,245);
        load->setColor(150,250,250);
    }
   
    //fnt.add("white", load);
    fnt.add("std", load);
    fnt.add("hud", load);
    fnt.add("stat", load);
   
    /**/
   
      
    return ret;
}

int MasterResourses::freeFonts()
{
    ResHold<BitmapFont>& fnt = ResHold<BitmapFont>::instance();
    fnt.clear();
   
    //freed by fnt: ResHold
    /*delete f;
      delete f1;
      delete fb;*/
   

   
    //f=NULL;
    //f1=NULL;
    //fb=NULL;
   
    return 0;
}

/*******************/
/*SHAREDMESSAGEPASS*/
/*******************/

SharedMessagePass::SharedMessagePass()
{
    //master_th_id=0; 
    change_master=false;
    next_master_th_id=0;
    sync_to=0;
    shutdown=false;
    sync_state=false;
    sync_state_to=0;
   
    freeze_threads=false;
    num_frozen=0;
   
}

SharedMessagePass::~SharedMessagePass()
{
}

/*
  void SharedMessagePass::build(uint num_th)
  {
  this->num_th=num_th;
  }
  * 
  void SharedMessagePass::free()
  {
  }
*/

/*************/
/*SHAREDAUDIO*/
/*************/

ProgramState& SharedAudio::progstate = ProgramState::instance();

SharedAudio::SharedAudio() : uploaded(0)
{
    num_uploaded=0;
    first_in_batch=true;
    mix_smp=NULL;
    max_sync=0;
}

SharedAudio::~SharedAudio()
{
    //freeAudioDev();
    //free();
}

int SharedAudio::initAudioDev()
{
    ProgramState& progstate = ProgramState::instance();
   
    int ret=0;
    //AUDIO
    adev.setFormat(progstate.getSampleRate(), progstate.getDevFmt(), progstate.getNChan(), progstate.getDevNSmp());
    adev.open();
   
    //check format gotten
    SDL_AudioSpec as = adev.getObtainedFmt();
   
    if(as.freq != progstate.getSampleRate())
    {
        cout << "sample rate got: " << as.freq << endl;
        progstate.setSampleRate(as.freq);
        ret = 1;
    }
    if(as.channels != progstate.getNChan())
    {
        cout << "channels got: " << as.channels << endl;
        progstate.setNChan(as.channels);
        ret = 1;
    }
    if(as.samples != progstate.getDevNSmp())
    {
        cout << "samples, wanted: " << progstate.getDevNSmp() << " got: " << as.samples << endl; 
        progstate.setDevNSmp(as.samples);
        ret = 1;
    }
    if(as.format != AUDIO_S16)
    {
        cout << "fmt got: " << as.format << endl;
        ret = 1;
        //fixme
    }
   
    //DONE
   
    if(!adev.working())
    {
        DERROR("audio failed");
        return -1;
    }
   
    return ret;   
}

void SharedAudio::freeAudioDev()
{
    if(adev.isOpen())
    {
        adev.clear();
        adev.close();
    }
   
}



void SharedAudio::build(uint num_th, uint genbuff, uint genlen)
{
    //DASSERT(!isbuilt);
    uploaded.resize(num_th);
   
    num_uploaded=0;
    first_in_batch=true;
    max_sync=0; //FIXME: maybe keep this set?(for performance)
   
    for(uint i=0;i< uploaded.size();i++)
    {
        WorkData* data = new WorkData();
        data->build(genbuff, genlen);
        uploaded[i]=data;
    }
   
   
    mix_smp=new Sample(genlen);
   
   
}

void SharedAudio::free()
{
    for(uint i=0;i< uploaded.size();i++)
    {
        //uploaded[i]->free();
        delete uploaded[i];
        uploaded[i]=NULL;
    }
   
    delete mix_smp;
    mix_smp=NULL;
}



bool SharedAudio::shouldUpload(uint th_id, int sync_count)
{
    if(sync_count<max_sync)return false;
    return !uploaded[th_id]->have_uploaded;
}


void SharedAudio::upload(Main* mc, uint th_id, int sync_count)
{
    DASSERT(th_id<uploaded.size());
    DASSERT(!uploaded[th_id]->have_uploaded);
   
    if(first_in_batch)
    {
        max_sync=sync_count;
        first_in_batch=false;
    }
    else
    {
        if(sync_count>max_sync)
        {
            max_sync=sync_count;
	     
            //this means all with less sync than this have to be discarded(they will upload again)
            for(uint i=0;i<uploaded.size();i++)
            {
                if(uploaded[i]->have_uploaded && (uploaded[i]->sync_count < max_sync))
                {
                    uploaded[i]->have_uploaded=false;
                    num_uploaded--;
                }
            }
        }
        else if(sync_count<max_sync)
        {
            ERROR("not here");
            //can not upload old sync
            return;
        }
    }
   
    while( !uploaded[th_id]->queueFull() )
    {
        DASSERT(uploaded[th_id]->queue_max_size==mc->smpQueueSize());
        DASSERT(mc->smpQueueIn()!=0);
        uploaded[th_id]->push(mc->smpQueuePop());
    }
   
    uploaded[th_id]->from_th=th_id;
    uploaded[th_id]->sync_count=sync_count;
    uploaded[th_id]->have_uploaded=true;
   
    num_uploaded++;
}

bool SharedAudio::shouldMix(uint th_id)
{
    return num_uploaded==uploaded.size();
}


void SharedAudio::mix(bool play_adev)
{
    DASSERT(num_uploaded==uploaded.size());
   
    //debug:
    int sy = uploaded[0]->sync_count;
    for(uint i=0;i<uploaded.size();i++)
    {
        DASSERT(uploaded[i]->sync_count == sy);
    }
    //end debug
   
   
    /*if(adev.inQueue() == 0)
      {
      DERROR("warning: queue empty...sysytem to slow?");
      }*/
   
    //must do it this way: oterwise adev.inQueue() might change(plays) during for-loop
    //WARNING: keep todo a int: inQueue might be greater than SampleGenBuffer() if change occured:
    int todo=progstate.getSampleGenBuffer();
    if(play_adev)
        todo = progstate.getSampleGenBuffer()-adev.inQueue(); /*FIXME: decouple?*/
   
    if(todo<=0)return;
   
    for(int i=0;i<todo;i++)
    {
        const Sample* smp = mixNext(); //when play: gen buffers should be full and they are == adev::queue
	
        if(play_adev)adev.playQueue((const char*)smp->data, smp->bytes);
	
        //FIXME: not thread safe:
        if(progstate.runningDump())
        {
            if(!progstate.appendToDump(smp))
            {
                DERROR("dump failed"); 
            }
            if(progstate.dumpReady())
            {
                if(!progstate.dumpToFile())
                {
                    DERROR("dump file failed");
                }
            }
        }
    }
   
    //reset structure for next upload
    first_in_batch=true;
    num_uploaded=0;
    for(uint i=0;i<uploaded.size();i++)
    {
        uploaded[i]->have_uploaded=false;
    }
   
   
}

const Sample* SharedAudio::mixNext()
{
    static bool warned=false;
   
    mix_smp->zero();

    for(uint i=0;i<uploaded.size();i++)
    {
        DASSERT(uploaded[i]->have_uploaded);
	
        const Sample* next = uploaded[i]->pop();
        if(next==NULL)
        {
            ERROR("mix NULL");
            continue;
        }

        if(i == (uploaded.size()-1)) //only check clip for last mix(speed)
        {
            int clip=0;
            mix_smp->mix(next, &clip, SSHORT_MAX);
            if(clip>0)
            {
                if(!warned)
                {
                    DERROR("final mix overflow, "  << clip << " samples (one warning per prog-restart)");
                    warned=true;
                }
            }
        }
        else mix_smp->mix(next);
	
    }
   
    return mix_smp;
}

void SharedAudio::resetFor(uint th_id)
{
    uploaded[th_id]->resetQueue();
   
    if(uploaded[th_id]->have_uploaded)
    {
        DASSERT(num_uploaded>=1);
	
        uploaded[th_id]->have_uploaded=false;
        num_uploaded--;
	
        first_in_batch=true;
	
        for(uint i=0;i<uploaded.size();i++)
        {
            if(uploaded[i]->have_uploaded)
            {
                if(first_in_batch)
                {
                    max_sync=uploaded[i]->sync_count;
                    first_in_batch=false;
                }
                else
                {
                    if(uploaded[i]->sync_count>max_sync)
                    {
                        max_sync=uploaded[i]->sync_count;
                    }
                }
            }
        }
        DASSERT(num_uploaded<uploaded.size());
    }
}



SharedAudio::WorkData::WorkData()  
{
    queue_max_size=0;
    sync_count=0;
    from_th=0;
    smp_data=NULL;
    have_uploaded=false;
    q_read=0;
   
    d_num_pushed=0;
    d_num_popped=0;
}

SharedAudio::WorkData::~WorkData()
{
    free();
}

void SharedAudio::WorkData::build(uint genbuff, uint genlen)
{
    DASSERT(smp_data==NULL);
   
    q_read=0;
    sync_count=0;
    from_th=0;
    have_uploaded=false;
    resetQueue();
    d_num_pushed=0;
    d_num_popped=0;
   
   
    queue_max_size=genbuff;
   
    smp_data = new Sample*[genbuff];
   
    for(uint i=0;i<queue_max_size;i++)
    {
        smp_data[i] = new Sample(genlen);
    }
   
}

void SharedAudio::WorkData::free()
{
    if(smp_data==NULL)
    {
        ERROR("already freed");
        return;
    }
   
    for(uint i=0;i<queue_max_size;i++)delete smp_data[i];
    delete[] smp_data;
    smp_data=NULL;
}


void SharedAudio::WorkData::push(const Sample* smp)
{
    DASSERT(smp_queue.size() < queue_max_size);
   
    smp_data[q_read]->copy(smp);
   
    smp_queue.push(smp_data[q_read]);
   
    q_read++;
    q_read%=queue_max_size;
   
    //debug:
    d_num_pushed++;
}

const Sample* SharedAudio::WorkData::pop()
{
    DASSERT(smp_queue.size()>0);
    if(smp_queue.size()==0)
    {
        ERROR("smp queue empty!");
        cout << "pushed: " << d_num_pushed << " popped: " << d_num_popped << endl;
        return NULL;
    }
   
   
    Sample* ret = smp_queue.front();
    smp_queue.pop();
   
    d_num_popped++;
   
    return ret;
}

bool SharedAudio::WorkData::queueFull()
{
    return (smp_queue.size() == queue_max_size);
}

void SharedAudio::WorkData::resetQueue()
{
    q_read=0;
    smp_queue=queue<Sample*>();
}
