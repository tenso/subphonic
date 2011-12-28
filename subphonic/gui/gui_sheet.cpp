#include "gui_sheet.h" 

namespace spl {

SheetView::SheetView(BitmapFont* font, uint nrows)
{
    this->font=font;
   
    this->nrows=nrows;
   
    outline=false;
   
    xinc=2;
    yinc=2;
   
    delim_mod=4;
   
    setView(0,nrows);
   
   
   
    pos.w=0;
    pos.h = nrows * (font->charH() + 2*yinc);
   
    c_bg.r=128;
    c_bg.g=128;
    c_bg.b=128;
   
    c_lines.r=255;
    c_lines.g=255;
    c_lines.b=255;
   
    c_delim.r=140;
    c_delim.g=140;
    c_delim.b=140;
   
    c_sel.r=0;
    c_sel.g=255;
    c_sel.b=0;
   
    setShowGrid(false,false);
}

SheetView::~SheetView()
{
}


void SheetView::clearCols()
{
    cols.clear();
   
    pos.w=0;
    //pos.h=0;
   
    setView(0,nrows);
}


void SheetView::draw(Screen* screen)
{
    if(cols.size()==0)return;
    if(nrows==0)return;
   
    int x = pos.x;
    int y = pos.y;
   
    //first draw bg
    screen->box(x, y, x+pos.w, y+pos.h, screen->makeColor(c_bg));
   
    int xend= pos.x+pos.w;
    int cell_h = getCellH();
   
    if(delim_mod>0)
    {
        //FIXME: check this...
        //draw delimiters
        uint ndelim=1+(urows-1)/delim_mod;
	
        int off = start%delim_mod;
	
        //y -= cell_h;
        y -= cell_h*off;
	
        for(uint i=0 ; i<ndelim ;)
        {
            if(y>=pos.y)
            {
                screen->box(x, y, xend, y+cell_h, screen->makeColor(c_delim));
                i++;
            }
	     	     
            y += cell_h*delim_mod;	     
        }
    }
   
    y = pos.y;
   
    //draw all entries
    x += xinc;
   
    for(l_it it = cols.begin() ; it!=cols.end() ; it++)
    {
        y = pos.y + yinc;
	
        for(uint i=start;i<start+num;i++)
        {
            string e;
            if (!it->getEntry(i, e))
            {
                DERROR("entry not found");
                continue;
            }
            //FIXME DEBUG (segfault here?):
            if (e.length() == string::npos)
            {
                DERROR("string length ok?");
                continue;
            }
            const char* cstr = e.c_str();
            if (cstr[e.length()] != '\0')
            {
                DERROR("string not null terminated");
                continue;
            }

            font->printStr(x, y, cstr, e.length());
	     
            y += cell_h;
        }
	
        x += 2*xinc + it->getNChar()*font->charW();
    }
   
   
    //draw outline
   
    if(outline)
    {
        x = pos.x;
        y = pos.y;
	
        int h= pos.h;
        int w= pos.w;
	
        screen->wirebox(x, y, x+w, y+h, screen->makeColor(c_lines));
    }
   
    //vertical
    if(showgrid_v)
    {
        x = pos.x;
        y = pos.y;
        for(l_it it = cols.begin(); it!=cols.end()-1 ; it++)
        {
            x += 2*xinc + it->getNChar()*font->charW();
            screen->line(x, y, x, pos.y+pos.h, screen->makeColor(c_lines));
        }
    }
   
    //horizontal
    if(showgrid_h)
    {
        x = pos.x;
        y = pos.y;
	
        for(uint i=0 ; i < urows-1 ; i++)
        {
            y += cell_h;
	     
            screen->line(x, y, xend, y, screen->makeColor(c_lines));
        }
    }
}

SheetView::id SheetView::addCol(uint nchar_v)
{
    Column c(nchar_v, nrows);
    cols.push_back(c);
   
    pos.w+=font->charW()*nchar_v+2*xinc;
   
    return cols.size()-1;
}

uint SheetView::getNCols()
{
    return cols.size();
}


void SheetView::setBGColor(const SDL_Color& bg)
{
    c_bg = bg;
}

void SheetView::setLineColor(const SDL_Color& lines)
{
    c_lines = lines;
}

void SheetView::setSelColor(const SDL_Color& selected)
{
    c_sel = selected;
}

void SheetView::setDelimColor(const SDL_Color& delim)
{
    c_delim = delim;
}

void SheetView::setDelimMod(int m)
{
    delim_mod=m;
}

int SheetView::getDelimMod() const
{
    return delim_mod;
}

void SheetView::setShowGrid(bool vert, bool horiz)
{
    showgrid_v=vert;
    showgrid_h=horiz;
}

/*bool SheetView::getShowGrid() const
  {
  return showgrid;
  }*/


void SheetView::setOutline(bool v)
{
    outline=v;
}

bool SheetView::getOutline() const
{
    return outline;
}

bool SheetView::setView(uint start, int num)
{
    if(start>=nrows-1)start=nrows-1;
    if(num<0)num=nrows-start;
    if( start+num > nrows )num=nrows-start;
   
    this->start=start;
    this->num=num;
   
    urows = num;
    pos.h=urows*getCellH();
   
    return true;
}

void SheetView::getView(uint* start, int* num) const
{
    *start = this->start;
    *num = this->num;
}

void SheetView::setNRows(uint n)
{
    if(n==nrows)return;
    nrows=n;
   
    for(l_it it = cols.begin();it!=cols.end();it++)
    {
        it->setNRows(n);
    }
      
    setView(start, num); //updates h
}

uint SheetView::getNRows() const
{
    return nrows;
}

SDL_Rect SheetView::getCellPos(uint r, uint c) const
{
    DASSERT(c<cols.size());
    if(c>=cols.size())c=cols.size()-1;
   
    DASSERT(r<nrows);
    if(r>=nrows)r=nrows-1;
   
    int y = pos.y + (r-start)*getCellH();
    int x = pos.x;
   
   
    //must loop over all columns adding width: different w
    for(lc_it it = cols.begin() ; it!=cols.begin()+c ; it++)
    {
        x += 2*xinc + it->getNChar()*font->charW();
    }
   
   
    SDL_Rect ret;
   
    ret.x=x;
    ret.y=y;
    ret.w = getCellW(c);
    ret.h = getCellH();
   
    return ret;
   
}

uint SheetView::getCellW(uint col) const
{
    DASSERT(col<cols.size());
    if(col>=cols.size())col=cols.size()-1;
   
    return 2*xinc + cols[col].getNChar()*font->charW();
}

uint SheetView::getCellH() const
{
    return font->charH()+2*yinc;
}

uint SheetView::getCharW() const
{
    return font->charW();
}

uint SheetView::getCharH() const
{
    return font->charH();
}



bool SheetView::setEntry(uint r, uint c, const string& s)
{
    if(c>=cols.size())return false;
   
    l_it it = cols.begin()+c;
    DASSERT(it<cols.end());
   
    if(r>=it->getNRows())return false;
   
    return it->setEntry(r, s);
}

bool SheetView::getEntry(uint r, uint c, string& s) const
{
    DASSERT(c<cols.size());
    if(c>=cols.size())return false;
   
    lc_it it = cols.begin()+c;
    DASSERT(it<cols.end());
   
    if(r>=it->getNRows())return false;
   
    return it->getEntry(r, s);
}

void SheetView::setSpacing(uint x, uint y)
{
    xinc=x;
    yinc=y;
   
    pos.h = nrows * (font->charH() + 2*yinc);
   
    pos.w=0;
    for(l_it it = cols.begin();it!=cols.end();it++)
    {
        pos.w += font->charW()*it->getNChar()+2*xinc;
	
    }
   
}

uint SheetView::getSpacingX() const
{
    return xinc;
}


uint SheetView::getSpacingY() const
{
    return yinc;
}


int SheetView::moveEntries(uint from_row, int dir)
{
    int moved=0;
   
    for(l_it it = cols.begin();it!=cols.end();it++)
    {
        moved = it->moveEntries(from_row, dir);
    }
   
    return moved;
}



/********/
/*Column*/
/********/

SheetView::Column::Column(uint nchar_v, uint rows)
    : entries(rows)
{
    w=nchar_v;
}


void SheetView::Column::setNRows(uint rows)
{
    if(entries.size()==rows)return;
   
    entries.resize(rows);
    DASSERT(entries.size()==rows);
}

uint SheetView::Column::getNRows() const
{
    return entries.size();
}


void SheetView::Column::setNChar(uint nchar_v)
{
    if(w==nchar_v)return;
   
    w=nchar_v;
   
    for(v_it it=entries.begin();it!=entries.end();it++)
    {
        it->resize(nchar_v);
    }
   
}

uint SheetView::Column::getNChar() const
{
    return w;
}


//will reset ret
bool SheetView::Column::getEntry(uint n, string& ret) const
{
    DASSERT(n<entries.size());
    if(n>=entries.size())
    {
        return false;
    }
   
    ret = entries[n];
    return true;
}


//returns false if out of boounds
bool SheetView::Column::setEntry(uint n, const string& s)
{
    DASSERT(n<entries.size());
    if(n>=entries.size())
    {
        return false;
    }
   
    entries[n]=s;
   
    if(s.length()>w)
        entries[n] = entries[n].substr(0, w);
   
    return true;
}

int SheetView::Column::moveEntries(uint from, int dir)
{
    int moved=0;
   
    //DASSERT(from<entries.size());
    if(from>=entries.size())return moved;
   
    while(dir!=0)
    {
        if(dir<0)
        {
            if(from==0)break;
	     
            moveEntriesUp(from--);
            dir++;
	     
            moved--;
        }
        else
        {
            if(from==entries.size()-1)break;
	     
            moveEntriesDown(from++);
            dir--;
	     
            moved++;
        }
    }
   
    return moved;
}

bool SheetView::Column::moveEntriesUp(uint from)
{
    if(from==0)return false;
   
    uint sz = entries.size();
    for(uint i=from; i < sz ;i++)
    {
        entries[i-1] = entries[i];
    }
    entries[sz-1].clear();
   
    return true;
}

bool SheetView::Column::moveEntriesDown(uint from)
{
    if(from >= entries.size())return false;
      
    uint max = entries.size()-1;
      
    for(uint i=0;i<max-from;i++)
    {
        entries[max-i] = entries[max-i-1];
    }
    entries[from].clear();
   
    return true;
}


/*****************/
/*SheetViewLayout*/
/*****************/

SheetViewLayout::SheetViewLayout()
{
    start=0;
    num=0;
    xspace=0;
    usheets=0;
}


void SheetViewLayout::setSpacing(uint xspace)
{
    this->xspace=xspace;
}

void SheetViewLayout::clearSheets()
{
    sheets.clear();
}

SheetViewLayout::id SheetViewLayout::addSheet(SheetView* sheet)
{
    //first set sheets new pos
    int x = pos.x;
    int y = pos.y;
   
    if(sheets.size()>0)
    {
        int after = sheets[sheets.size()-1]->getPos().x + sheets[sheets.size()-1]->getPos().w;
        x += after + xspace;
    }
    sheet->setPos(x,y);
   
    sheets.push_back(sheet);
   
    recalcSize();
   
    return sheets.size()-1;
}

SheetView* SheetViewLayout::remSheet(id i)
{
    DASSERT(i < numSheets());
    if(i >= numSheets())return NULL;
     
    SheetView* sheet = sheets[i];
    sheets.erase(sheets.begin()+i);
   
    recalcSize();
   
    return sheet;
}

void SheetViewLayout::recalcSize()
{
    if(sheets.size()==0)return;
    if(start>=sheets.size())return;
   
    int w=-xspace; //xspace not included for last sheet
    int h=0;
   
   
    uint todo = (start+num <= sheets.size() ) ? start+num : sheets.size();
   
    for(uint i=start ; i < todo ;i++)
    {
        w+=sheets[i]->getPos().w+xspace;
	
        if(sheets[i]->getPos().h > h)
        {
            h=sheets[i]->getPos().h;
        }
	
    }
   
    pos.w=w;
    pos.h=h;
}
 
void SheetViewLayout::recalcPos()
{
    //reset sheet positions
    int x = pos.x;
    int y = pos.y;
   
    for(uint i=0;i<start;i++)
    {
        x -= sheets[i]->getPos().w + xspace;
    }
   
   
    for(uint i=0;i<sheets.size();i++)
    {
        sheets[i]->setPos(x,y);
	
        x += sheets[i]->getPos().w + xspace;
    }
}


SheetView* SheetViewLayout::getSheet(id i)
{
    DASSERT( i < sheets.size());
    if(i>=sheets.size())return NULL;
   
    return sheets[i];
}

bool SheetViewLayout::setView(uint start, int num)
{
    uint n = numSheets();
    if(start>=n-1)start=n-1;
    if(num<0)num=n-start;
   
    this->start=start;
    this->num=num;
   
    usheets = start+num;
   
    recalcPos();
    recalcSize();
   
    return true;
}

uint SheetViewLayout::numSheets() const
{
    return sheets.size();
}


void SheetViewLayout::draw(Screen* s)
{
    if(sheets.size()==0)return;
    if(start>=sheets.size())return;
   
    uint todo = (start+num <= sheets.size() ) ? start+num : sheets.size();

    for(uint i=start ; i < todo ;i++)
    {
        sheets[i]->draw(s);
    }
}

void SheetViewLayout::setPos(int x, int y)
{
    pos.x=x;
    pos.y=y;
   
    recalcPos();
   
}

}