#include "tracker_base.h" 

//FIXME: tracker: should do a backwards search for last control data i.e 0.1 .... marker .... .2 should always out .1
//also do this for arrowkey step

const string Tracker::empty_note = string("---");
const string Tracker::empty_ctrl = string("-----");

Tracker::Tracker(BitmapFont* font)
{
    setOnlyPreview(false);
   
    first=true;
    key_tick=0;
    key_d_tick=0;
   
    have_preview=false;
    have_preview_ctrl=false;
   
    octave=4;
    ch=-1; //use track num
   
    setAddStep(0);
    setDelStep(0);
   
    track_sel_wrap=false;
   
    outline_fill=true;     
    outline=true;
    outline_sz=10;
    c_outline.r=80;
    c_outline.g=80;
    c_outline.b=80;
   
    this->font = font;
    setStaySelected(true);
   
    ntracks=DEF_NTRACKS;
    utracks=DEF_UTRACKS;
   
    /*
      urows is number of rows viewd on screen
      _off is the view offset
      _sel is the selected on the screen
      so c_track,c_row = _off+_sel
    */
   
    c_sel=0;
    c_off=0;
   
    c_track=0;
   
    //WARNING: these MUST be in non-increasing order: segfault othwise
    nrows=DEF_NROWS;
    urows=DEF_UROWS;
    r_pre_look=DEF_PRELOOK; //this is index not number
   
    DASSERT(nrows>=urows);
    DASSERT(urows>r_pre_look);
   
    r_off=0;
    r_sel=0;
   
    cont = new Container();
    cont->setPos(0, 0);
   
    //BUILD SHEETS
    left_sheet = buildLeftSheet();
    cont->add(left_sheet);
   
    xpad=20;
    int xoff = left_sheet->getPos().w+xpad;
   
    view = new SheetViewLayout();
    view->setSpacing(8);
    view->setView(0, utracks);
   
    for(uint i=0;i<ntracks;i++)
    {
        SheetView* t = buildTrackSheet(i);
	
        tracks.push_back(t);
	
        /*cont->add(t,xoff,0);
          xoff += t->getPos().w+xpad;*/
	
        view->addSheet(t);
    }
   
   
    cont->add(view, xoff, 0); 
   
    setMarkersFollow(true);
   
    //SET DIMENSIONS of comp manually
    pos.w=cont->getPos().w;
    pos.h=cont->getPos().h;
   
    //BUILD MARKERS(play under sel!)
   
    play_marker = new SelMarker(pos.w+2, left_sheet->getCellH()+1);
    play_marker->setColor(125,200,5);
    play_marker->setThickness(2);
    cont->add(play_marker);
   
    row_marker = new SelMarker(pos.w+2, left_sheet->getCellH()+1);
    row_marker->setColor(0,0,0);
    row_marker->setThickness(3);
    cont->add(row_marker);
   
    /*col_marker = new SelMarker(tracks[0]->getPos().w+2, left_sheet->getCellH()+1);
      col_marker->setColor(0,240,0);
      col_marker->setThickness(3);
      cont->add(col_marker);*/
   
   
    color_noedit.r=155;
    color_noedit.g=255;
    color_noedit.b=5;
   
    color_edit.r=255;
    color_edit.g=5;
    color_edit.b=155;
   
    note_marker = new SelMarker(tracks[0]->getCellPos(0, NOTE_COL).w+2, left_sheet->getCellH()+1);
    note_marker->setColor(color_noedit);
    note_marker->setThickness(3);
   
    //cont->add(note_marker);
   
   
    ctrl_marker = new SelMarker(tracks[0]->getCellPos(0, CTRL_COL).w+2, left_sheet->getCellH()+1);
    ctrl_marker->setColor(color_noedit);
    ctrl_marker->setThickness(3);
   
    //cont->add(ctrl_marker);
   
    subcol=NOTE_COL;
   
    c_row=0;
    c_col=0;
   
    selPositionMarker();
   
    play=false;
   
   
    play_row=c_row;
    playPositionMarker();
   
    /*setNRows(12);
      setNRows(2);*/
    /* setNTracks(2);*/
   
    page_step=8;
   
    do_ctrl=false;
    do_ctrl_c=0;
   
    //build initial cache
    cache_data = vector<RowCache>(ntracks);
   
    for(uint track=0;track<ntracks;track++)
    {
        cache_data[track].last_read_count=0;
        cache_data[track].cache_count=1;
    }
   
    playCacheRowData();
   
    note_key_d_tick=KEY_DEL;
    note_key_tick=0;
   
    c_have_key=false;
    c_key_press=0;
}

Tracker::~Tracker()
{
    delete cont;
}

void Tracker::setPageStep(uint s)
{
    page_step=s;
}

uint Tracker::getPageStep()
{
    return page_step;
}


void Tracker::setNViewRows(uint v)
{
    if(v==urows)return;
   
    //DASSERT(urows<nrows);
    if(v>nrows)v=nrows;
    urows=v;
   
    if(urows>1)r_pre_look=urows/2-1;
    else r_pre_look=0;
   
    //size changed
    //must call setSelRow, it makes sure that offsets etc are OK
    setSelRow(c_row);
    setPlayRow(play_row);
   
    view->recalcSize(); //before 'cont'
    cont->recalcSize();
   
    pos.h = cont->getPos().h;
   
   
}

void Tracker::setNViewTracks(uint v)
{
    if(v==utracks)return;
   
    //DASSERT(urows<nrows);
    if(v>ntracks)v=ntracks;
    utracks=v;
   
    //r_pre_look=urows/2-1;
   
    //size changed
    setSelTrack(c_track);
    //updateView();
   
    view->recalcSize(); //before 'cont'
   
    //before cont recalc, this is in it
    row_marker->setDim(left_sheet->getPos().w+xpad+view->getPos().w+2, left_sheet->getCellH()+1);
    play_marker->setDim(left_sheet->getPos().w+xpad+view->getPos().w+2, left_sheet->getCellH()+1);
   
    cont->recalcSize();
   
    pos.w=cont->getPos().w;
   
    //row_marker->setDim(view->getPospos.w+2, left_sheet->getCellH()+1);
   
}


void Tracker::setSheetOpt(SheetView* sheet)
{
    sheet->setDelimMod(4);
    sheet->setSpacing(6,5);
    sheet->setView(0,urows);
    sheet->setShowGrid(false,false);
    sheet->setOutline(false);
}

//WARNING: 4 digits in left sheet HARDCODED
SheetView* Tracker::buildLeftSheet()
{
    SheetView* sheet = new SheetView(font, nrows);
    setSheetOpt(sheet);
   
   
    uint need_dig = 4; //was =1
    /*uint p=10;
      while(p < getNRows())
      {
      p*=10;
      need_dig++;
      }*/
   
   
    SheetView::id id = sheet->addCol(need_dig);
   
    for(uint i=0;i<sheet->getNRows();i++)
    {
        string str = getZeroPadded(i, need_dig);
	
        sheet->setEntry(i, id, str);
    }
   
    return sheet;
}

string Tracker::getZeroPadded(int num, int len)
{
    stringstream ss;
    //ss.clear();
   
    ss << num;
    string str = ss.str();
   
    uint nz = len-str.length();
   
    str.clear();
    for(uint j=0;j<nz;j++)str.append("0");
    str.append(ss.str());
   
    return str;
}


SheetView* Tracker::buildTrackSheet(uint def_chan)
{
    SheetView* sheet = new SheetView(font, nrows);
    setSheetOpt(sheet);
   
    //chan | note | ctrl
    //--     ---    -----
    //00     C#3    .0015
   
    sheet->addCol(NCHAR_CH);
    sheet->addCol(NCHAR_NOTE);
    sheet->addCol(NCHAR_CTRL);
   
   
    fillDefTrackData(sheet, 0, sheet->getNRows(), def_chan);
   
    return sheet;
}


void Tracker::fillDefTrackData(SheetView* sheet, uint start, uint num, uint def_chan)
{
    string ch = getZeroPadded(def_chan, 2);
   
    for(uint i=start;i<start+num;i++)
    {
        sheet->setEntry(i, CH_COL, ch);
        sheet->setEntry(i, NOTE_COL, empty_note);
        sheet->setEntry(i, CTRL_COL, empty_ctrl);
    }
}

bool Tracker::entryIsUsed(uint track, uint row)
{
    string ch = getZeroPadded(track, 2);
   
    string e_ch;
    string e_note;
    string e_ctrl;
   
    tracks[track]->getEntry(row, CH_COL, e_ch);
    tracks[track]->getEntry(row, NOTE_COL, e_note);
    tracks[track]->getEntry(row, CTRL_COL, e_ctrl);
   
    if(e_ch!=ch || e_note!=empty_note || e_ctrl!=empty_ctrl)return true;
    return false;
}


void Tracker::selPositionMarker()
{
    //position row marker
    int x = left_sheet->getCellPos(c_row, 0).x;
    int y = left_sheet->getCellPos(c_row, 0).y;
   
    row_marker->setPos(x-1, y);
   
    //position cell marker
    x = tracks[c_track]->getPos().x;
   
    uint x_note = tracks[c_track]->getCellPos(c_row, NOTE_COL).x;
    uint x_ctrl = tracks[c_track]->getCellPos(c_row, CTRL_COL).x;
   
    //col_marker->setPos(x-1, y);
   
    note_marker->setPos(x_note-1, y);
    ctrl_marker->setPos(x_ctrl-1, y);
}

void Tracker::playPositionMarker()
{
    //play arker visible?
    int x=0;
    int y=0;
   
    if(play_row>=r_off && play_row<r_off+urows)
    {
        //visible
	
        //position row marker
        x = left_sheet->getCellPos(play_row, 0).x;
        y = left_sheet->getCellPos(play_row, 0).y;
    }
    else
    {
        //not visible(hide behind sel marker)
        x = left_sheet->getCellPos(c_row, 0).x;
        y = left_sheet->getCellPos(c_row, 0).y;
    }
   
    play_marker->setPos(x-1, y);
}

void Tracker::setMarkersFollow(bool v)
{
    markers_follow=v;
}

bool Tracker::getMarkersFollow()
{
    return markers_follow;
}


void Tracker::setSelRow(uint n)
{
    r_sel=0;
    r_off=0;
    c_row=r_sel+r_off;
   
    stepSelRow(n); //also updates
}

void Tracker::setPlayRow(uint n)
{
    play_row=0;
   
    stepPlayRow(n); //also updates
}


void Tracker::stepSelRow(int n)
{
    while(n!=0)
    {
        if(n<0) //MOVE UP
        {
            if(r_sel>r_pre_look)
            {
                r_sel--;
            }
            else //if(r_sel<=r_pre_look)
            {
                int left = r_off;
		  
                if(left>0)
                {
                    if(r_off>0)r_off--;
                }
                else
                {
                    DASSERT(r_off==0);
		       
                    if(r_sel>0)r_sel--;
                }
            }
	     
            n++;
        }
        else  //MOVE DOWN
        {
            if(r_sel<r_pre_look)
            {
                r_sel++;
            }
            else //if(r_sel>=r_pre_look)
            {
                int left = nrows-(urows+r_off);
		  
                if(left>0)
                {
                    r_off++;
                    if(r_off>=nrows-urows)
                    {
                        r_off=nrows-urows;
                    }
                }
                else
                {
                    r_sel++;
		       
                    if(r_sel>=urows)
                    {
                        r_sel=urows-1;
                    }
                }
            }
	     
            n--;
        }
    }//end while
   
    c_row=r_sel+r_off;
   
    updateView();
   
    //cacheRowData();
}


void Tracker::stepPlayRow(int n)
{
    while(n!=0)
    {
        if(n<0) //MOVE UP
        {
            if(play_row>0)
            {
                play_row--;
            }
            else 
            {
                break;
            }
            n++;
        }
        else  //MOVE DOWN
        {
            if(play_row<nrows)
            {
                play_row++;
                if(play_row==nrows)play_row=nrows-1; //nrows cant be 0 here
            }
            else 
            {
                break;
            }
	     
            n--;
        }
    }//end while
   
    playPositionMarker();
   
    playCacheRowData();
}

void Tracker::setSelTrack(uint n)
{
    c_sel=0;
    c_off=0;
    c_track=c_sel+c_off;
   
    stepSelTrack(n); //also updates
}


void Tracker::stepSelTrack(int n)
{
    while(n!=0)
    {
        if(n<0)
        {
            if(c_sel>0)c_sel--;
            else
            {
                if(c_off>0)c_off--;
            }
            n++;
        }
        else
        {
            c_sel++;
            if(c_sel>=utracks)
            {
                c_sel=utracks-1;
		  
                c_off++;
                if(c_off >= ntracks-utracks)c_off=ntracks-utracks;
            }
            n--;
        }
    }
   
    c_track=c_off+c_sel;
   
    updateView();
}

Tracker::POS Tracker::selRowPos() const
{
    if(c_row==nrows-1)return END;
    if(c_row==0)return BEG;
    return NORM;
}

Tracker::POS Tracker::selTrackPos() const
{
    if(c_track==ntracks-1)return END;
    if(c_track==0)return BEG;
    return NORM;
}

uint Tracker::getSelRow() const
{
    return c_row;
}

uint Tracker::getSelTrack() const
{
    return c_track;
}

Tracker::POS Tracker::playRowPos() const
{
    if(play_row==nrows-1)return END;
    if(play_row==0)return BEG;
    return NORM;
}

uint Tracker::getPlayRow() const
{
    return play_row;
}



bool Tracker::giveInput(spl::Input& in)
{
    bool handled=false;
   
    if(!do_ctrl)
    {
        //no repeats:
	
        if(in.keySet(SDLK_SPACE, true))
        {
            if(play)
            {
            }
            else
            {
                setPlayRow(getSelRow());
            }
	     
            play=!play;
            handled=true;
        }
	
        if(in.keySet(SDLK_HOME, true))
        {
            setSelRow(0);
            if(markers_follow)setPlayRow(0);
            handled=true;
        }
        if(in.keySet(SDLK_END,true))
        {
            setSelRow(getNRows()-1);
            if(markers_follow)setPlayRow(getNRows()-1);
            handled=true;
        }
	
        if(in.keySet(SDLK_LCTRL,false)) //SINGLE TRACK
        {
            if(in.keySet(SDLK_DELETE,true))
            {
                clearTrackEntries(c_track);
                handled=true;
            }
            if(in.keySet(SDLK_LSHIFT,false))
            {
                if(in.keySet(SDLK_KP_PLUS,true))
                {
                    transposeTrackNotes(c_track, 12);
                    handled=true;
                }
                if(in.keySet(SDLK_KP_MINUS,true))
                {
                    transposeTrackNotes(c_track, -12);
                    handled=true;
                }
            }
            else
            {
                if(in.keySet(SDLK_KP_PLUS,true))
                {
                    transposeTrackNotes(c_track, 1);
                    handled=true;
                }
                if(in.keySet(SDLK_KP_MINUS,true))
                {
                    transposeTrackNotes(c_track, -1);
                    handled=true;
                }
            }
	     
        }
        else if(in.keySet(SDLK_RCTRL,false)) //ALL TRACKS
        {
            if(in.keySet(SDLK_DELETE,true))
            {
                clearAllEntries();
                handled=true;
            }
	     
            if(in.keySet(SDLK_LSHIFT,false))
            {
                if(in.keySet(SDLK_KP_PLUS,true))
                {
                    transposeAllNotes(12);
                    handled=true;
                }
                if(in.keySet(SDLK_KP_MINUS,true))
                {
                    transposeAllNotes( -12);
                    handled=true;
                }
            }
            else
            {
                if(in.keySet(SDLK_KP_PLUS,true))
                {
                    transposeAllNotes(1);
                    handled=true;
                }
                if(in.keySet(SDLK_KP_MINUS,true))
                {
                    transposeAllNotes(-1);
                    handled=true;
                }
            }
	     
        }
        else
        {
        }
	
    }
   
   
    //hold press
   
    /*static char keys[26] = {'z','x','c','v','b','n','m','a','s','d','f','g',
      'h','j','k','l','q','w','e','r','t','y','u','i','o','p'};
    */
    static char keys[] = {'z','x','c','v','b','n','m','a','s','d','f','g',
                          'h','j','k','l','q','w','e','r','t','y','u','i','o','p' /*25*/
                          ,'0', '1', '2', '3', '4', '5', '6', '7','8', '9', '.','-' /*26-37*/};
   
    static uint nkeys_alpha=26;
   
    //if(play_and_press)
    if(subcol==NOTE_COL)
    {
        bool update_press=false;
        int update_to=0;
	
        if(in.numKeyPress())
        {
            //cout << "press" << endl;
            handled=true;
            for(int i=0;i<nkeys_alpha;i++)
            {
                /*must also check keySet so nothing else has reseted it:e.g. save all state etc lctrl-'a'*/
                if(in.keyPress( (SDLKey)keys[i]) && in.keySet((SDLKey)keys[i])) 
                {
                    if(c_have_key)
                    {
                        if(c_key_press!=i)
                        {
                            //cout << "upd" << endl;
                            update_press=true;
                            have_preview=false;
                            update_to=i;
                            c_have_key=false;//so that repeat does not ruin it
                        }
                        else
                        {
                            //FIXME:happens!
                            //DERROR("never here");
                        }
                    }
                    else
                    {
                        //cout << "down: " << i << endl;
                        //new key altogether
                        note_key_d_tick=KEY_DEL;
                        note_key_tick=0;
                        update_press=true;
                        have_preview=false;
                        update_to=i;
                    }
                }
            }
        }
	
        if(c_have_key) //do key repeat
        {
            if(note_key_d_tick>0)note_key_d_tick--;
            if(note_key_d_tick==0)
            {
                if(note_key_tick%KEY_REP==0)
                {
                    //FIXME: this way update_press will enter a new note even if not needed...
                    //i,e if add step is 0
                    update_press=true;
                    update_to=c_key_press;
                }
                note_key_tick++;
            }
        }
	
	
        if(update_press)
        {
            //cout << "do update" << endl;
            //update c_key
            c_have_key=true;
            c_key_press=update_to;
	     
            if(!only_preview)//set entry
            {
                setNote(c_track, c_row, c_key_press+1+octave*12);
		  
                if(ch>=0)
                {
                    setCh(c_track, c_row, ch);
                }
                else
                {
                    setCh(c_track, c_row, c_track);
                }
            }
	     
            //always a preview as long as key is held
            //dont overwrite preview_key when stepping
	     
            if(!have_preview)
            {
                have_preview=true;
		  
                preview_key = c_key_press+1+octave*12;//getNote(c_track, c_row);
                preview_ch  = getCh(c_track, c_row);
                if(haveCtrl(c_track, c_row))
                {
                    preview_ctrl = getCtrlDouble(c_track, c_row);
                    have_preview_ctrl=true;
                }
                else 
                {
                    have_preview_ctrl=false;
                }
            }
	     
	     
            if(add_step!=0)
            {
                stepSelRow(add_step);
                if(markers_follow)
                {
                    stepPlayRow(add_step);
                }
                else playPositionMarker();//adding steps to selmarker might pot play marker off view
                handled=true;
            }
        }
	
        if(c_have_key) //key rel?
        {
            if(in.numKeyRel())
            {
                handled=true;
                //cout << "rel" << endl;
                for(int i=0;i<26;i++)
                {
                    if(in.keyRel( (SDLKey)keys[i]) ) //dont reset key, otherwise unable to hold for preview
                    {
                        //cout << i << " " << c_key_press << endl;
                        //only cut note if it is last press that is released
                        if(i==c_key_press)
                        {
                            //cout << "up: " << i << endl;
                            //off key
                            c_have_key=false;
                            have_preview=false;
                            //reset note key repeat
                            note_key_d_tick=KEY_DEL;
                            note_key_tick=0;
                            break;
                        }
                    }
                }
            }
        }
    }
    else 
    {
        c_have_key=false; //cant have key if not in note col
        have_preview=false;
    }/*NOTE_COL DONE*/
   
    //FIXME: key repeat for these
    if(subcol==CTRL_COL && ! do_ctrl)
    {
        bool do_ent_ctrl=false;
        double ent_val=0;
	
        static double small_step=0.01;
        static double step=0.05;
        static double big_step=0.1;
        for(int i=0;i<nkeys_alpha;i++)
        {
            /*must also check keySet so nothing else has reseted it:e.g. save all state etc lctrl-'a'*/
            if(in.keyPress( (SDLKey)keys[i]) && in.keySet((SDLKey)keys[i])) 
            {
                do_ent_ctrl=true;
                if(i<=9)ent_val=i*small_step;
                else if(i<=17)ent_val=(i-8)*step;
                else ent_val=0.5+(i-18)*big_step;
            }
        }
	
        /*if(in.keyPress(SDLK_q))
          {
          do_ent_ctrl=true;
          ent_val=0;
          }
          if(in.keyPress(SDLK_w))
          {
          do_ent_ctrl=true;
          ent_val=0.5;
          }
          if(in.keyPress(SDLK_e))
          {
          do_ent_ctrl=true;
          ent_val=1;
          }
        */
	
        if(do_ent_ctrl)
        {
            //caches row...
            setCtrl(c_track, c_row, ent_val);
            handled=true;
	     
            if(getAddStep()!=0)
            {
                stepSelRow(add_step);
		  
                if(markers_follow)
                {
                    stepPlayRow(add_step);
                }
                else playPositionMarker();//adding steps to selmarker might pot play marker off view
            }
        }
    }
   
   
    //ONLY [ENTER](some presses allowed, follows...) CTRL DATA WHEN NOT PLAYING, i.e stop play if return
    //if playing stop
    if(subcol==CTRL_COL)
    {
        /*TOGGLE ENTER MODE?*/
        if(in.keySet(SDLK_RETURN,true))
        {
            if(markers_follow) //FIXME:this is a little iffy
            {
                if(play)//play=false;
                {
                    play=false;
                }
            }
	     
            //FINISH ENTRY
            if(do_ctrl)
            {
                do_ctrl=false;
		  
                //dont let empty in(must be -----)
                string entry;
                tracks[c_track]->getEntry(c_row, CTRL_COL, entry);
                if(entry.size()==0)
                {
                    remCtrl(c_track,c_row);
                    //tracks[c_track]->setEntry(c_row, CTRL_COL, do_ctrl_old);
                }
		  
                if(play_row==c_row)playCacheRowData();
            }
            //START ENTRY
            else
            {
                do_ctrl=true;
		  
                //save old and set empty
                //do_ctrl_c=0;
                //tracks[c_track]->getEntry(c_row, CTRL_COL, do_ctrl_old);
                //tracks[c_track]->setEntry(c_row, CTRL_COL, string(""));
		  
                //start from old entry
                //must save old incase of escape
                tracks[c_track]->getEntry(c_row, CTRL_COL, do_ctrl_old);
		  
                if(haveCtrl(c_track, c_row))
                {
                    do_ctrl_c=do_ctrl_old.length();
                }
                else
                {
                    do_ctrl_c=0;
                    tracks[c_track]->setEntry(c_row, CTRL_COL, string(""));
                }
            }
            handled=true;
        }
	
        if(do_ctrl)
        {
            /*arrow key entry START*/
            bool do_ent_ctrl=false;
            double ent_val=0;
	     
            if(in.keySet(SDLK_LEFT, true))ent_val=-ENT_CTRL_SMALL;
            if(in.keySet(SDLK_RIGHT, true))ent_val=ENT_CTRL_SMALL;
            if(in.keySet(SDLK_UP, true))ent_val=ENT_CTRL_BIG;
            if(in.keySet(SDLK_DOWN, true))ent_val-=ENT_CTRL_BIG;
            if(ent_val!=0)do_ent_ctrl=true;
	     
            if(do_ent_ctrl)
            {
                double c_val = getCtrlDouble(c_track, c_row);
                c_val +=ent_val ;
                ostringstream ss;
                ss << c_val;
                tracks[c_track]->setEntry(c_row, CTRL_COL, ss.str());
                do_ctrl_c=ss.str().length()+1;
            }
            /*arrow key entry END*/
	     
            for(int i=26;i<38;i++)
            {
                //if( in.keySet( (SDLKey)(SDLK_0 + keys[i]-'0'), false) )
                if( in.keySet( (SDLKey)keys[i], true) )
                {
                    if(do_ctrl_c<NCHAR_CTRL)
                    {
                        string entry;
                        tracks[c_track]->getEntry(c_row, CTRL_COL, entry);
			    
                        entry.push_back(keys[i]);
                        tracks[c_track]->setEntry(c_row, CTRL_COL, entry);
                        do_ctrl_c++;
                    }
		       
                    handled=true;
                }
            }
	     
            if(in.keySet(SDLK_DELETE,true))
            {
                do_ctrl_c=0;
                tracks[c_track]->setEntry(c_row, CTRL_COL, string(""));
            }
	     
	     
            if(in.keySet(SDLK_BACKSPACE,true))
            {
                if(do_ctrl_c>0)do_ctrl_c--;
		  
                string entry;
                tracks[c_track]->getEntry(c_row, CTRL_COL, entry);
		  
                entry.erase(do_ctrl_c);
		  
                tracks[c_track]->setEntry(c_row, CTRL_COL, entry);
		  
                handled=true;
            }
            if(in.keySet(SDLK_ESCAPE,true))
            {
                //fIXME:DOES NOT WORK, CONSOLE take
                tracks[c_track]->setEntry(c_row, CTRL_COL, do_ctrl_old);
                do_ctrl=false;
		  
                handled=true;
            }
        }
	
    }else do_ctrl=false;
   
    //key repeat
   
    //FIXMENOW: HACK!
    //if(in.keySet(SDLK_LEFT,false))handled=true;
    //if(in.keySet(SDLK_RIGHT,false))handled=true;
   
    if(!do_ctrl)
    {
        if(key_tick%KEY_REP==0 && key_d_tick==0)
        {
            if(first)key_d_tick=KEY_DEL;
            first=false;
	     
            if(in.keySet(SDLK_UP, false))
            {
                if(selRowPos()==BEG)setSelRow(getNRows()-1);
                else stepSelRow(-1);
		  
                if(markers_follow)
                {
                    if(playRowPos()==BEG)setPlayRow(getNRows()-1);
                    else stepPlayRow(-1);
                }
		  
		  
                handled=true;
            }
            else if(in.keySet(SDLK_DOWN,false))
            {
                if(selRowPos()==END)setSelRow(0);
                else stepSelRow(1);
		  
                if(markers_follow)
                {
                    if(playRowPos()==END)setPlayRow(0);
                    else stepPlayRow(1);
                }
		  
		  
                handled=true;
            }
            else if(in.keySet(SDLK_LEFT, false))
            {
                if(subcol==CTRL_COL)
                {
                    subcol=NOTE_COL;
                }
                else
                {
                    if(selTrackPos()==BEG)
                    {
                        if(track_sel_wrap)
                        {
                            subcol=CTRL_COL;
                            setSelTrack(getNTracks()-1);
                        }
			    
                    }
                    else 
                    {
                        subcol=CTRL_COL;
                        stepSelTrack(-1);
                    }
                }
		  
                handled=true;
            }
            else if(in.keySet(SDLK_RIGHT, false))
            {
                if(subcol==NOTE_COL)
                {
                    subcol=CTRL_COL;
                }
                else
                {
                    if(selTrackPos()==END)
                    {
                        if(track_sel_wrap)
                        {
                            setSelTrack(0);
                            subcol=NOTE_COL;
                        }
                    }
                    else 
                    {
                        subcol=NOTE_COL;
                        stepSelTrack(1);
                    }
		       
                }
                handled=true;
            }
            else if(in.keySet(SDLK_PAGEUP, false))
            {
                if(selRowPos()==BEG);//setSelRow(getNRows()-1);
                else stepSelRow(-page_step);
		  
                if(markers_follow)
                {
                    if(playRowPos()==BEG);//setSelRow(getNRows()-1);
                    else stepPlayRow(-page_step);
                }
		  
		  
                handled=true;
            }
            else if(in.keySet(SDLK_PAGEDOWN,false))
            {
                if(selRowPos()==END);//setSelRow(0);
                else stepSelRow(page_step);
		  
                if(markers_follow)
                {
                    if(playRowPos()==END);//setSelRow(0);
                    else stepPlayRow(page_step);
                }
		  
                handled=true;
            }
            else if(in.keySet(SDLK_INSERT, false))
            {
                if(tracks[c_track]->moveEntries(c_row, 1)!=0)
                    fillDefTrackData(tracks[c_track], c_row, 1, c_track/*fixme also: clear to ch if it exist*/);
		  
                stepSelRow(1);
		  
                if(markers_follow)
                {
                    stepPlayRow(1);
                }
		  
		  
                handled=true;
            }
            else if(in.keySet(SDLK_BACKSPACE, false))
            {
                if(tracks[c_track]->moveEntries(c_row, -1)!=0)
                    fillDefTrackData(tracks[c_track], nrows-1, 1, c_track/*fixme also: clear to ch if it exist*/);
		  
                stepSelRow(-1);
		  
                if(markers_follow)
                {
                    stepPlayRow(-1);
                }
		  
                handled=true;
            }
            else if(in.keySet(SDLK_DELETE, false))
            {
                //fillDefTrackData(tracks[c_track], c_row, 1, c_track/*fixme*/);
                /*if(subcol==NOTE_COL)tracks[c_track]->setEntry(c_row, NOTE_COL, empty_note);
                  if(subcol==CTRL_COL)tracks[c_track]->setEntry(c_row, CTRL_COL, empty_ctrl);
                  cacheRowData();*/
		  
		  
                if(subcol==NOTE_COL)remNote(c_track, c_row);
                if(subcol==CTRL_COL)remCtrl(c_track, c_row);
		  
                if(del_step!=0)
                {
                    stepSelRow(del_step);
		       
                    if(markers_follow)
                    {
                        stepPlayRow(del_step);
                    }
		       
                }
		  
                handled=true;
            }
            else 
            {
                first=true;
            }
	     
        }
	
        if(key_d_tick > 0)key_d_tick--;
	
        key_tick++;
	
        if(!in.keySet(SDLK_UP, false) && !in.keySet(SDLK_DOWN,false) &&
        !in.keySet(SDLK_LEFT, false) && !in.keySet(SDLK_RIGHT,false) &&
        !in.keySet(SDLK_PAGEUP, false) && !in.keySet(SDLK_PAGEDOWN, false)
        && !in.keySet(SDLK_INSERT, false) && !in.keySet(SDLK_BACKSPACE, false)
        && !in.keySet(SDLK_DELETE,false))
        {
            key_tick=0;
            key_d_tick=0;
            first=true;
        }
    }
   
    return handled;
}

bool Tracker::getOnlyPreview()
{
    return only_preview;
}

void Tracker::setOnlyPreview(bool v)
{
    only_preview=v;
}


uint Tracker::getPreviewCh() const
{
    return preview_ch;
}

bool Tracker::havePreview() const
{
    return have_preview;
}

bool Tracker::havePreviewCtrl() const
{
    return have_preview_ctrl;
}

int Tracker::getPreviewNote() const
{
    return preview_key;
}

double Tracker::getPreviewCtrl() const
{
    return preview_ctrl;
}


void Tracker::updateView()
{
    //update rows visible
   
    left_sheet->setView(r_off, urows);
   
    for(uint i=0;i<tracks.size();i++)
    {
        tracks[i]->setView(r_off, urows);
    }
   
    //update tracks visible
    view->setView(c_off, utracks);
   
    //after setView!
    selPositionMarker();
    playPositionMarker();
}


void Tracker::draw(Screen* s)
{
    if(outline)
    {
        if(outline_fill)
        {
            s->box(pos.x-outline_sz, pos.y-outline_sz,
		    pos.x+pos.w+outline_sz, pos.y+pos.h+outline_sz,
		    s->makeColor(c_outline));
        }
        else
        {
            s->wirebox(pos.x-outline_sz, pos.y-outline_sz, pos.x+pos.w+outline_sz, pos.y+pos.h+outline_sz,
			s->makeColor(c_outline), outline_sz);
        }
    }
   
    cont->draw(s);
   
    if(subcol==NOTE_COL)
    {
        note_marker->draw(s);
    }
    else if(subcol==CTRL_COL)
    {
        if(do_ctrl)ctrl_marker->setColor(color_edit);
        else  ctrl_marker->setColor(color_noedit);
	
        ctrl_marker->draw(s);
	
        if(do_ctrl)
        {
            int x = tracks[c_track]->getCellPos(c_row, CTRL_COL).x + do_ctrl_c*tracks[c_track]->getCharW();
            int y = tracks[c_track]->getCellPos(c_row, CTRL_COL).y + tracks[c_track]->getCellPos(c_row, CTRL_COL).h;
	     
            int w = tracks[c_track]->getCharH();
	     
            uint xsp=tracks[c_track]->getSpacingX();
	     
            s->box(x+xsp, y-5, x+xsp+w,y-2, s->makeColor(155,255,5));
        }
    }
}

void Tracker::setX(int x)
{
    pos.x=x;
    cont->setPos(pos.x,pos.y);
   
    selPositionMarker();
    playPositionMarker();
}

void Tracker::setY(int y)
{
    pos.y=y;
    cont->setPos(pos.x,pos.y);
   
    selPositionMarker();
    playPositionMarker();
}

//WARNING: 4 digits in left sheet HARDCODED
void Tracker::setNRows(uint n)
{
    //if(n==nrows)return;
   
    int fill=n-nrows;
    int old = nrows;
   
    nrows=n;
   
    //urows=DEF_UROWS;
    if(urows>nrows)
    {
        urows=nrows;
        r_pre_look=DEF_UROWS; //=off
    }
    else
    {
        if(urows>1)r_pre_look=urows/2-1;
        else r_pre_look=0;
    }
   
    //if(nrows>urows)
    //r_pre_look=DEF_PRELOOK;
    //else r_pre_look=DEF_UROWS; //=off
   
   
    for(uint i=0;i<ntracks;i++)
    { 
        tracks[i]->setNRows(nrows); //consider view thrashed after this
	
        if(fill>0)
        {
            fillDefTrackData(tracks[i], old, fill, i);
        }
    }
   
    //remake left_sheet, no user data anyway
   
   
    //int dx_off = left_sheet->getPos().w+20;
   
    SheetView* o_left = left_sheet;
    left_sheet = buildLeftSheet();
    left_sheet->setPos(o_left->getPos());
   
    cont->replace(o_left, left_sheet);
    delete o_left;
   
    /*dx_off = left_sheet->getPos().w+20 - dx_off;
     * 
     comp_list* cl = cont->getList();
     for(comp_list_it it = cl->begin();it!=cl->end();it++)
     {
     if((*it) !=left_sheet)
     (*it)->relMove(dx_off, 0);
     }
    */
   
    //must reset view here, before any recal size is made so right size is used
    if(getSelRow()>=nrows)
    {
        setSelRow(nrows-1);
    }
    else 
    {
        //must still setSelRow, it makes sure that offsets etc are OK
        setSelRow(c_row);
    }
   
    if(getPlayRow()>=nrows)
    {
        setPlayRow(nrows-1);
    }
    else 
    {
        //must still setSelRow, it makes sure that offsets etc are OK
        setPlayRow(play_row);
    }
   
   
    view->recalcSize(); //before 'cont'
   
    cont->recalcSize();
   
    /*cout << view->getPos().h << endl;
      cout << " " << row_marker->getPos().y << endl;
      cout << "  " << cont->getPos().h << endl;*/
   
    pos.h = cont->getPos().h;
   
    //cout << urows << endl << endl;
   
    //updateView();
}

uint Tracker::getNRows() const
{
    return nrows;
}


void Tracker::setNTracks(uint n)
{
    if(n==ntracks)return;
   
    if(n>ntracks)
    {
        uint fill = n-ntracks;
        for(uint i=0;i<fill;i++)
        {
            SheetView* t = buildTrackSheet(ntracks+i);
	     
            tracks.push_back(t);
	     
            view->addSheet(t);
        }
    }
    else
    {
        int erase = ntracks-n;
        for(int i=0;i<erase;i++)
        {
            DASSERT(tracks.size()!=0);
	     
            SheetView* sheet = *(tracks.end()-1);
            view->remSheet(view->numSheets()-1);
            tracks.erase(tracks.end()-1);
            delete sheet;
        }
	
    }
   
    ntracks=n;
   
    //utracks=DEF_UTRACKS;
    if(utracks>ntracks)utracks=ntracks;
   
    //this is needed: if not size might be wrong if num tracks was less than utracks...
   
    if(getSelTrack()>=ntracks)setSelTrack(ntracks-1);
    else setSelTrack(c_track);
   
    int w = view->getPos().w + left_sheet->getPos().w + xpad;
   
    row_marker->setDim(w+2, left_sheet->getCellH()+1);
    play_marker->setDim(w+2, left_sheet->getCellH()+1);
   
    cont->recalcSize();
   
    pos.w=cont->getPos().w;
   
    //updateView();
   
    //num tracks changed; chache changed
    cache_data = vector<RowCache>(ntracks);
   
    for(uint track=0;track<ntracks;track++)
    {
        cache_data[track].last_read_count=0;
        cache_data[track].cache_count=1;
    }
   
    playCacheRowData();
}

uint Tracker::getNTracks() const
{
    return ntracks;
}

void Tracker::setOctave(int o)
{
    octave=o;
}

int Tracker::getOctave() const
{
    return octave;
}

void Tracker::setDefChannel(int c)
{
    ch=c;
}

int Tracker::getDefChannel() const
{
    return ch;
}

bool Tracker::getPlay() const
{
    return play;
}

void Tracker::setPlay(bool p)
{
    play=p;
}

const bool* Tracker::getPlayPtr() const
{
    return &play;
}


uint Tracker::getCh(uint track, uint row) const
{
    DASSERT(track<getNTracks());
    DASSERT(row<getNRows());
   
    if(track>=getNTracks())return 0;
    if(row>=getNRows())return 0;
   
    string str;
    tracks[track]->getEntry(row, CH_COL, str);
   
    return parseCh(str);
}

bool Tracker::haveNote(uint track, uint row) const
{
    string str;
    tracks[track]->getEntry(row, NOTE_COL, str);
   
    if(str == empty_note)return false;
    return true;
}

bool Tracker::haveCtrl(uint track, uint row) const
{
    string str;
    tracks[track]->getEntry(row, CTRL_COL, str);
   
    if(str == empty_ctrl)return false;
    return true;
}


int Tracker::getNote(uint track, uint row) const
{
    DASSERT(track<getNTracks());
    DASSERT(row<getNRows());
    if(track>=getNTracks())return 0;
    if(row>=getNRows())return 0;
   
    bool have = haveNote(track, row);
    DASSERT(have);
    if(!have)return 0;
   
    string str;
    tracks[track]->getEntry(row, NOTE_COL, str);
   
    return parseNote(str);
}

double Tracker::getCtrlDouble(uint track, uint row) const
{
    DASSERT(track<getNTracks());
    DASSERT(row<getNRows());
    if(track>=getNTracks())return 0;
    if(row>=getNRows())return 0;
   
    bool have = haveCtrl(track, row);
    DASSERT(have);
    if(!have)return 0;
   
    string str;
    tracks[track]->getEntry(row, CTRL_COL, str);
   
    return parseCtrl(str);
}

string Tracker::getCtrl(uint track, uint row) const
{
    DASSERT(track<getNTracks());
    DASSERT(row<getNRows());
    if(track>=getNTracks())return string("NULL");
    if(row>=getNRows())return string("NULL");
   
    bool have = haveCtrl(track, row);
    DASSERT(have);
    if(!have)return string("invld");
   
    string str;
    tracks[track]->getEntry(row, CTRL_COL, str);
   
    return str;
}

void Tracker::setCh(uint track, uint row, uint ch)
{
    tracks[track]->setEntry(row, CH_COL, getZeroPadded(ch, NCHAR_CH));
   
    if(row == play_row)playCacheRowData();
}

void Tracker::setNote(uint track, uint row, int key)
{
    string n = noteStr(key);
    tracks[track]->setEntry(row, NOTE_COL, n);
   
    if(row == play_row)playCacheRowData();
}

void Tracker::setCtrl(uint track, uint row, const string& ctrl)
{
    tracks[track]->setEntry(row, CTRL_COL, ctrl);
   
    if(row == play_row)playCacheRowData();
}

void Tracker::setCtrl(uint track, uint row, double ctrl)
{
    ostringstream ss;
    ss << ctrl;
    tracks[track]->setEntry(row, CTRL_COL, ss.str());
   
    if(row == play_row)playCacheRowData();
}


void Tracker::remNote(uint track, uint row)
{
    DASSERT(track<getNTracks());
    DASSERT(row<getNRows());
   
    tracks[track]->setEntry(row, NOTE_COL, empty_note);
    if(row==play_row)playCacheRowData();
}

void Tracker::remCtrl(uint track, uint row)
{
    DASSERT(track<getNTracks());
    DASSERT(row<getNRows());
   
    tracks[track]->setEntry(row, CTRL_COL, empty_ctrl);
   
    if(row==play_row)playCacheRowData();
}


uint Tracker::parseCh(const string& str) const
{
    //crating a stringstream is rather expensive
    /*static*/ istringstream ss;
    //ss.clear();
    ss.str(str);
   
    uint ch;
    ss >> ch;
   
    return ch;
}

double Tracker::parseCtrl(const string& str) const
{
    /*static*/ istringstream ss;
    //ss.clear();
    ss.str(str);
   
    double c;
    ss >> c;
   
    return c;
}

int Tracker::parseNote(const string& str) const
{
    return strNote(str);
}

void Tracker::transposeTrackNotes(uint track, int tran)
{
    for(uint i=0;i<nrows;i++)
    {
        transposeNote(track, i, tran);
    }
   
}

void Tracker::transposeAllNotes(int tran)
{
    for(uint i=0;i<ntracks;i++)
    {
        transposeTrackNotes(i, tran); //some unnecessary caching going on...
    }
}


void Tracker::transposeNote(uint track, uint row, int tran)
{
    if(!haveNote(track,row))return;
   
    int k = getNote(track, row);
    k+=tran;
   
    //FIXME: check max octave
   
    setNote(track, row, k);
   
    if(row == play_row)playCacheRowData();
}


void Tracker::setDelStep(int step)
{
    del_step=step;
}

int Tracker::getDelStep()
{
    return del_step;
}

void Tracker::setAddStep(int step)
{
    add_step=step;
}

int Tracker::getAddStep()
{
    return add_step;
}


void Tracker::setTrackSelWrap(bool on)
{
    track_sel_wrap=on;
}

bool Tracker::getTrackSelWrap()
{
    return track_sel_wrap;
}


uint Tracker::getNumEntries()
{
    uint n=0;
   
    for(uint t=0;t<ntracks;t++)
    {
        for(uint r=0;r<nrows;r++)
        {
            if(entryIsUsed(t, r))n++;
        }
    }
    return n;
}

void Tracker::clearAllEntries()
{
    for(uint i=0;i<tracks.size();i++)
    {
        fillDefTrackData(tracks[i], 0, nrows, i);
    }
   
    playCacheRowData();
}

void Tracker::clearTrackEntries(uint track)
{
    DASSERT(track<ntracks);
    if(track>=ntracks)return;
   
    fillDefTrackData(tracks[track], 0, nrows, track);
   
    playCacheRowData();
}

void Tracker::getEntries(vector<StateEntry>& ret)
{
    for(uint t=0;t<ntracks;t++)
    {
        for(uint r=0;r<nrows;r++)
        {
            if(entryIsUsed(t, r))
            {
                StateEntry e;
                e.track=t;
                e.row=r;
                e.ch = getCh(t,r);
		  
                if(haveNote(t,r))
                {
                    e.note = getNote(t,r);
                    e.have_note=true;
                }
                else  e.have_note=false;
		  
                if(haveCtrl(t,r))
                {
                    string ctrl = getCtrl(t,r);
                    DASSERT(ctrl.size()<=NCHAR_CTRL);
                    memcpy(e.ctrl, ctrl.c_str(), (ctrl.size()+1)*sizeof(char)); //+1 : '\0'
		       
                    e.have_ctrl=true;
                }
                else e.have_ctrl=false;
		  
                ret.push_back(e);
            }
        }
    }
}

void Tracker::setEntries(const vector<StateEntry>& data)
{
    for(uint i=0;i<data.size();i++)
    {
        StateEntry e = data[i];
	
        if(e.track>=ntracks || e.row>=nrows)
        {
            DERROR("tracker");
            cout << "(W) data does not correspond to tracker, skip. track: " 
                 << e.track << " row: " << e.row << endl;
            continue;
        }
	
        setCh(  e.track, e.row, e.ch);
        if(e.have_note)setNote(e.track, e.row, e.note);
	
        if(e.have_ctrl)
        {
            string ctrl;
            ctrl.assign(e.ctrl);
            setCtrl(e.track, e.row, ctrl);
        }
    }
   
    playCacheRowData();
}


void Tracker::StateEntry::zero()
{
    track=0;
    row=0;
    ch=0;
    note=0;
    have_note=false;
   
    memset(ctrl,'\0', 6);
    have_ctrl=false;
}

/*******/
/*CACHE*/
/*******/

void Tracker::playCacheRowData()
{
    //FIXME: have error here but why? play_row get>=nrows
    DASSERTP(play_row<nrows, "play_row again!");
   
    for(uint track=0;track<ntracks;track++)
    {
        //CH
        cache_data[track].ch = getCh(track, play_row);
	
        //NOTE
        cache_data[track].have_note = haveNote(track, play_row);
	
        if(cache_data[track].have_note)
            cache_data[track].note = getNote(track, play_row);
        else
            cache_data[track].note = 0;
	
        //CTRL
        cache_data[track].have_ctrl = haveCtrl(track, play_row);
	
        if(cache_data[track].have_ctrl)
        {
            cache_data[track].ctrl = getCtrlDouble(track, play_row);
            cache_data[track].str_ctrl = getCtrl(track, play_row);
        }
        else
        {
            cache_data[track].ctrl = 0;
            cache_data[track].str_ctrl = string("");
        }
	
        //COUNTER
	
        //only increment if needed this way "overflow" not possible
        if(cache_data[track].last_read_count==cache_data[track].cache_count)
            cache_data[track].cache_count++;
    }
}


bool Tracker::playHaveUpdate(uint track)
{
    return (cache_data[track].cache_count != cache_data[track].last_read_count);
}

void Tracker::playFlagReadUpdate(uint track)
{
    cache_data[track].last_read_count = cache_data[track].cache_count;
}


bool Tracker::playHaveNote(uint track) const
{
    return cache_data[track].have_note;
}

bool Tracker::playHaveCtrl(uint track) const
{
    return cache_data[track].have_ctrl;
}


uint Tracker::playGetCh(uint track) const
{
    return cache_data[track].ch;
}

int Tracker::playGetNote(uint track) const
{
    return cache_data[track].note;
}


double Tracker::playGetCtrlDouble(uint track) const
{
    return cache_data[track].ctrl;
}

string Tracker::playGetCtrl(uint track) const
{
    return cache_data[track].str_ctrl;
}



/***********/
/*SelMarker*/
/***********/

Tracker::SelMarker::SelMarker(uint w, uint h)
{
    pos.x=0;
    pos.y=0;
    pos.w=w;
    pos.h=h;
   
    setColor(255,255,255);
   
    thick=2;
   
    fill=false;
}

void Tracker::SelMarker::setFill(bool v)
{
    fill=v;
}


void Tracker::SelMarker::setDim(uint w, uint h)
{
    pos.w=w;
    pos.h=h;
}

void Tracker::SelMarker::setColor(const SDL_Color& c)
{
    color=c;
}

void Tracker::SelMarker::setColor(Uint8 r, Uint8 g, Uint8 b)
{
    color.r=r;
    color.g=g;
    color.b=b;
}


void Tracker::SelMarker::setThickness(uint v)
{
    thick=v;
}


void Tracker::SelMarker::draw(Screen* s)
{
    if(fill)
    {
        s->box(pos.x, pos.y, pos.x+pos.w, pos.y+pos.h, s->makeColor(color));
    }
    else
    {
        s->wirebox(pos.x, pos.y, pos.x+pos.w, pos.y+pos.h, s->makeColor(color), thick);
    }
}

