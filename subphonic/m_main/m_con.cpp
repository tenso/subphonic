#include "m_con.h" 

Connection::Connection()
{
    default_color=true;
    out=NULL;
    in=NULL;
    give=false;
}

Connection::~Connection()
{
    for(con_list_it it = lines.begin();it!=lines.end();it++)
    {
        delete *it;
    }
    lines.clear();
}

void Connection::reset()
{
    in=NULL;
    give=false;
    out=NULL;
}


bool Connection::removeLine(LineData* line)
{
    con_list_it t = find(lines.begin(),lines.end(), line);
   
    if(t==lines.end())
    {
        DERROR("(W) line not found");
        return false;
    } 
   
    LineData* ld = *t;
    lines.erase(t);
   
    DASSERT(ld!=NULL);
    DASSERT(ld->in!=NULL && ld->out!=NULL);
   
    //tell the soundcomps to remove this input/output pair
    ld->in->remInput();
    ld->out->remOutput();
   
    //MUST remove link to line in it/og from
    ld->in->remLine();
    ld->out->remLine();
   
    delete ld;
   
    return true;
}

LineData* Connection::saveLine()
{
    DASSERT(in!=NULL && out!=NULL);
    LineData* data = new LineData(outpos, inpos, out, in);
   
    if(default_color)
    {
        const ProgramState& progstate = ProgramState::instance();
        color = progstate.getLineColor();
    }
   
    data->color[0]=color.r;
    data->color[1]=color.g;
    data->color[2]=color.b;
   
    lines.push_back(data);
    return data;
}

//FIXME: not good!
void Connection::drawPatchCord(int x0, int y0, int x1, int y1, double tension, Screen* s, 
const SDL_Color& s_color, const SDL_Color& e_color, unsigned int line_w)
{
    int xs=0;
    int ys=0;
    int xe=0;
    int ye=0;
   
    if(y0<y1)
    {
        xs=x1;
        ys=y1;
        xe=x0;
        ye=y0;
    }
    else
    {
        xs=x0;
        ys=y0;
        xe=x1;
        ye=y1;
    }
   
    double dx= xe-xs;
    int dir = (dx >= 0 ) ? 1 : -1;
    int xo=xs;
    int yo=ys;
    if(dx==0)
    {
        xs-=dir;
        dx=1;
    }
   
    int done=0;
    int want=abs((int)dx);
    int x=xs;
    int step=1;
   
    //Uint32 col = s->makeColor(e_color);
   
    while(done<want)
    {
        double k0 = (xe-x)/dx;
        k0*=k0;
        double k1 = (x-xs)/dx;
        k1*=k1;
        int y = (int)( ys*((tension+1)/tension-k0/tension)+(ye*tension-(tension+1)*ys)*k1/tension);
	
        //s->line(xo,yo,x,y,col, line_w);
        s->roundedLine(xo,yo,x,y, s_color, e_color, line_w);
        xo=x;
        yo=y;
	
        x+=dir*step;
        done+=step;
    }
    //s->line(xo,yo,xo+dir,ye,col, line_w);
    s->roundedLine(xo,yo,xo+dir,ye, s_color, e_color, line_w);
}


void Connection::draw(Screen* s, bool fancy, unsigned int line_w)
{
    const ProgramState& progstate = ProgramState::instance();
   
    for(con_list_it it = lines.begin();it!=lines.end();it++)
    {
        LineData* ld = (LineData*)*it;
	
        //get color
        SDL_Color s_color;
        SDL_Color e_color;
	
        Uint32 color;
        color=s->makeColor(ld->color[0],ld->color[1],ld->color[2]);
	
        e_color.r = ld->color[0];
        e_color.g = ld->color[1];
        e_color.b = ld->color[2];
	
        float c_scale=0.5;
        s_color.r = (Uint8)(ld->color[0]*c_scale);
        s_color.g = (Uint8)(ld->color[1]*c_scale);
        s_color.b = (Uint8)(ld->color[2]*c_scale);
	
        /*	Uint32 color;
            if(ld->have_color)color=ld->color_v;
            else 
            {
            ld->color_v=s->makeColor(ld->color[0],ld->color[1],ld->color[2]);
            ld->have_color=true;
            * 
            color=ld->color_v;
            }*/
	
        if(fancy)
        {
            double want=progstate.draw_line_tension;
            double tmp_settle=want;
	     
            double nrev=20.0;
	     
            if(progstate.draw_line_settle && ld->tmp_settle<nrev)
            {
                ld->tmp_settle+=0.3;
		  
                double frac=0.25*want*(nrev-ld->tmp_settle)/nrev;

                ld->tmp_count+=0.4;
		  
                tmp_settle += frac*cos(M_2PI*ld->tmp_count/(nrev)+M_PI_2);
            }
            else ld->tmp_settle=nrev;
	     
            drawPatchCord(ld->line[0].x,ld->line[0].y,ld->line[1].x,ld->line[1].y, 
            tmp_settle, s, s_color,e_color, line_w);
        }
        //else s->line(ld->line[0].x,ld->line[0].y,ld->line[1].x,ld->line[1].y, color, line_w);
        else s->roundedLine(ld->line[0].x,ld->line[0].y,ld->line[1].x,ld->line[1].y, s_color, e_color, line_w);
    }
}

void Connection::startCon(OutputGiver* o)
{
    out = o;
   
    //rem old, if it exist
    //out->remLine();
    if(out->line!=NULL)
        removeLine(out->line);
   
   
    outpos = out->getPos();
    outpos.x+=outpos.w/2;
    outpos.y+=outpos.h/2;
   
    give = true;
}

void Connection::finishCon(InputTaker* i)
{
    in=i;
   
    //must remove line already going into this input, IF it exist
    //in->remLine();
    if(in->line!=NULL)
        removeLine(in->line);
   
    //make the actual Value** connection
    in->setInput(out);
   
    inpos =  in->getPos();
    inpos.x+=inpos.w/2;
    inpos.y+=inpos.h/2;
   
    LineData* line = saveLine();
   
    //use this new line(when comps move they update line)
    in->setLine(line);
    out->setLine(line);
   
    reset();
}

void Connection::defaultAddColor()
{
    default_color=true;
}

void Connection::addColor(const SDL_Color& c)
{
    default_color=false;
   
    color =c;
}
