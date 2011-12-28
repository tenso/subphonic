#include "screen.h"
#include "../util/debug.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace spl {

/******/
/*BASE*/
/******/

Screen::Screen(int w, int h)
{
    this->w=w;
    this->h=h;
}

Screen::~Screen()
{
}

void Screen::wirebox(int x0, int y0, int x1, int y1, Uint32 color, int size)
{
    size = 1;

    line(x0, y0, x1, y0,    color, size);
   
    line(x0, y1-size, x1, y1-size, color, size);
   
    line(x1-size, y0, x1-size, y1-size, color, size);
    line(x0, y0, x0, y1-size, color, size);
}


void Screen::box(int x0, int y0, int x1, int y1, Uint32 color)
{
    if(x0>x1)
    {
        int t=x0;
        x0=x1;
        x1=t;
    }
    if(y0>y1)
    {
        int t=y0;
        y0=y1;
        y1=t;
    }
   
    for(int i=y0;i<y1;i++)
    {
        for(int j=x0;j<x1;j++)
        {
            plot(j,i,color);
        }
    }
}


void Screen::line(int x0, int y0, int x1, int y1, Uint32 color, int size)
{
    int dx = (x1>=x0) ? x1-x0 : x0-x1;
    int dy = (y1>=y0) ? y1-y0 : y0-y1;
    int xstep = (dx>0) ? ((x0<x1) ? 1 : -1) : 0;
    int ystep = (dy>0) ? ((y0<y1) ? 1 : -1) : 0;
    int x=x0;
    int y=y0;
    int temp=0;
   
    if(dx>=dy)
    {
        while(x!=x1)
        {
            for(int i=0;i<size;i++)plot(x,y+i,color);
            temp+=dy;
            if(temp>=dx)
            {
                y+=ystep;
                temp-=dx;
            }
            x+=xstep;
        }
    }
    else
    {
        while(y!=y1)
        {
            for(int i=0;i<size;i++)plot(x+i,y,color);
            temp+=dx;
            if(temp>=dy)
            {
                x+=xstep;
                temp-=dy;
            }
            y+=ystep;
        }
    }
}



void Screen::roundedLine(int x0, int y0, int x1, int y1,
const SDL_Color& s_color, const SDL_Color& e_color, int size)
{
    /*if(size<=1)*/return line(x0,y0,x1,y1,makeColor(e_color), size);
   
    int dx = (x1>=x0) ? x1-x0 : x0-x1;
    int dy = (y1>=y0) ? y1-y0 : y0-y1;
    int xstep = (dx>0) ? ((x0<x1) ? 1 : -1) : 0;
    int ystep = (dy>0) ? ((y0<y1) ? 1 : -1) : 0;
    int x=x0;
    int y=y0;
    int temp=0;
   
    //size >=2 here
   
    int dcolor = size>>1; //symetric around ~half
    SDL_Color color;
    int dr,dg,db;
   
    dr=(e_color.r-s_color.r)/dcolor;
    dg=(e_color.g-s_color.g)/dcolor;
    db=(e_color.b-s_color.b)/dcolor;
   
    if(dx>=dy)
    {
        while(x!=x1)
        {
            for(int i=0;i<size;i++)
            {
                if(i<dcolor)
                {
                    color.r = s_color.r+dr*i;
                    color.g = s_color.g+dg*i;
                    color.b = s_color.b+db*i;
		       
                }
                else if(i>dcolor)
                {
                    color.r = e_color.r-dr*(i-dcolor);
                    color.g = e_color.g-dg*(i-dcolor);
                    color.b = e_color.b-db*(i-dcolor);
                }
                else
                {
                    color.r = e_color.r;
                    color.g = e_color.g;
                    color.b = e_color.b;
                }
		  
                plot(x,y+i, makeColor(color));
            }
	     
            temp+=dy;
            if(temp>=dx)
            {
                y+=ystep;
                temp-=dx;
            }
            x+=xstep;
        }
    }
    else
    {
        while(y!=y1)
        {
            for(int i=0;i<size;i++)
            {
                if(i<dcolor)
                {
                    color.r = s_color.r+dr*i;
                    color.g = s_color.g+dg*i;
                    color.b = s_color.b+db*i;
		       
                }
                else if(i>dcolor)
                {
                    color.r = e_color.r-dr*(i-dcolor);
                    color.g = e_color.g-dg*(i-dcolor);
                    color.b = e_color.b-db*(i-dcolor);
                }
                else
                {
                    color.r = e_color.r;
                    color.g = e_color.g;
                    color.b = e_color.b;
                }
                plot(x+i,y, makeColor(color));
            }
	     
            temp+=dx;
            if(temp>=dy)
            {
                x+=xstep;
                temp-=dy;
            }
            y+=ystep;
        }
    }
}

void Screen::fill(Uint32 color)
{
    box(0,0,w,h,color);
}


/*******/
/*SDLSW*/
/*******/

SDL_SW_Screen::SDL_SW_Screen(int w, int h, int bpp, int flag) : Screen(w,h)
{
    if(flag==0)flags = SDL_SWSURFACE;
    else flags=flag;
   
    screen = SDL_SetVideoMode(w, h, bpp, flags);
    if(screen == NULL)
    {
        cout << "sdl set video mode failed" << endl;
        return;
    }
    
    pixels = (Uint8*)screen->pixels;
   
    this->bpp = screen->format->BytesPerPixel;
   
    pitch = screen->pitch;
   
    //SDL_Rect crect = {0,0,w,h};
         
    this->w=w;
    this->h=h;
   
    SDL_SetClipRect(screen, NULL/*&crect*/);
}

SDL_SW_Screen::~SDL_SW_Screen()
{
    cout <<"~SDL_SW_Screen()"<<endl;
    SDL_FreeSurface(screen);
    cout <<"~SDL_SW_Screen() done"<<endl;
}


void SDL_SW_Screen::blit(SDL_Surface* s, SDL_Rect* pos, SDL_Rect* off)
{
    //off not changed by SDL, pos is must copy it
    SDL_Rect p;
    p.x=0;
    p.y=0;
    if(pos!=NULL)p=*pos;
   
    SDL_BlitSurface(s, off/*determins src rect*/, screen, &p/*only x,y used*/);
}


void SDL_SW_Screen::toggleFullScreen()
{
    SDL_WM_ToggleFullScreen(screen);
}


void SDL_SW_Screen::plot(int x, int y, Uint32 color)
{
    //remove me!!!!!!!!!!!!!!!!
    if(x>w-1 || y>h-1 || y<0 || x<0)
    {
        //cout << "plot: " << x << "," << y << endl;
        return;
    }
   
    Uint8* addr = pixels+x*bpp+y*pitch;
    switch(screen->format->BytesPerPixel)
    {
        case 1:
            *addr = (Uint8)color;
            break;
        case 2:
            *(Uint16*)addr = (Uint16)color;
            break;
        case 4:
            *(Uint32*)addr = (Uint32)color;
            break;
    }
}

void SDL_SW_Screen::show(int x, int y, int w, int h)
{
    SDL_UpdateRect(screen, x, y, w, h);
}


Uint32 SDL_SW_Screen::makeColor(Uint8 r, Uint8 g, Uint8 b)
{
    return (Uint32)SDL_MapRGB(screen->format, r, g, b);
}

bool SDL_SW_Screen::ok()
{
    return (screen!=NULL);
}

void SDL_SW_Screen::box(int x0, int y0, int x1, int y1, Uint32 color)
{
    SDL_Rect dst;
   
    int x,y,w,h;
    x = (x0<=x1) ? x0 : x1;
    y = (y0<=y1) ? y0 : y1;
    w=abs(x0-x1);
    h=abs(y0-y1);
   
    dst.x=x;
    dst.y=y;
    dst.w=w;
    dst.h=h;
   
    SDL_FillRect(screen, &dst, color);
}


/*******/
/*SDLGL*/
/*******/

SDL_GL_Screen::SDL_GL_Screen(int w, int h, int bpp, int flag) : Screen(w,h)
{
    flags = flag;

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    flags |= SDL_OPENGL;// | SDL_RESIZABLE;
    screen = SDL_SetVideoMode(w, h, bpp, flags);
    if(screen == NULL)
    {
        cout << "sdl set video mode failed" << endl;
        return;
    }
   
    this->bpp = screen->format->BytesPerPixel;
   
    pitch = screen->pitch;
    
    this->w=w;
    this->h=h;
   
    glViewport(0,0,w,h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, 1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.5, 0.5,0.5, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    blitLayer = -90;

    //original is now better
/*    if (GL_ARB_texture_rectangle)
      {
      cout << "have GL_ARB_texture_rectangle" << endl;
      }
      else
      {
      cout << "does not have GL_ARB_texture_rectangle" << endl;
      }*/

}

SDL_GL_Screen::~SDL_GL_Screen()
{
    cout <<"~SDL_GL_Screen()"<<endl;
    SDL_FreeSurface(screen);
    cout <<"~SDL_GL_Screen() done"<<endl;
}


void SDL_GL_Screen::blit(SDL_Surface* s, SDL_Rect* pos, SDL_Rect* off)
{
    SDL_Rect p, o;
    p.x=0;
    p.y=0;
    if(pos!=NULL)p=*pos;
    
    map<SDL_Surface*, RenderData>::iterator it;

    if( s->format->BytesPerPixel == 3)
    {
        it = renderData.find(s);

        if ( it  == renderData.end())
        {
            RenderData data;
            /*data.texState.envMode(GL_REPLACE);
            data.texState.autoScale(false);
            data.texState.magFilter(GL_NEAREST);
            data.texState.minFilter(GL_NEAREST);
            data.texState.assignIMG(s);*/
            data.surf = s;
            renderData[s] = data;
            it = renderData.find(s);
        }
        D_ASSERT(it!=renderData.end());
    }
    else
    {
        it = renderDataAlpha.find(s);

        if ( it  == renderDataAlpha.end())
        {
            RenderData data;
            /*data.texState.envMode(GL_REPLACE);
            data.texState.autoScale(false);
            data.texState.magFilter(GL_NEAREST);
            data.texState.minFilter(GL_NEAREST);
            data.texState.assignIMG(s);*/
            data.surf = s;
            renderDataAlpha[s] = data;
            it = renderDataAlpha.find(s);
        }
        D_ASSERT(it!=renderDataAlpha.end());
    }
    o.x = 0;
    o.y = 0;
    o.w = s->w;
    o.h = s->h;

    if (off != NULL)
    {
        o = *off;
    }
   
    //tri0
    it->second.texCoords.push_back(o.x);
    it->second.texCoords.push_back(o.y+o.h);
    it->second.verts.push_back(p.x);
    it->second.verts.push_back(p.y + o.h);
    it->second.verts.push_back(blitLayer);

    it->second.texCoords.push_back(o.x+o.w);
    it->second.texCoords.push_back(o.y+o.h);
    it->second.verts.push_back(p.x + o.w);
    it->second.verts.push_back(p.y + o.h);
    it->second.verts.push_back(blitLayer);

    it->second.texCoords.push_back(o.x);
    it->second.texCoords.push_back(o.y);
    it->second.verts.push_back(p.x);
    it->second.verts.push_back(p.y);
    it->second.verts.push_back(blitLayer);

    //tri1
    it->second.texCoords.push_back(o.x + o.w);
    it->second.texCoords.push_back(o.y + o.h);
    it->second.verts.push_back(p.x + o.w);
    it->second.verts.push_back(p.y + o.h);
    it->second.verts.push_back(blitLayer);
   
    it->second.texCoords.push_back(o.x + o.w);
    it->second.texCoords.push_back(o.y);
    it->second.verts.push_back(p.x + o.w);
    it->second.verts.push_back(p.y);
    it->second.verts.push_back(blitLayer);
    
    it->second.texCoords.push_back(o.x);
    it->second.texCoords.push_back(o.y);
    it->second.verts.push_back(p.x);
    it->second.verts.push_back(p.y);
    it->second.verts.push_back(blitLayer);

    blitLayer += 0.01;
}


void SDL_GL_Screen::toggleFullScreen()
{
    cout << "fullscreen" << endl;
    flags^=SDL_FULLSCREEN;
    
    SDL_WM_ToggleFullScreen(screen);

    /*if( (SDL_SetVideoMode( w, h, bpp, flags ))==0)
      {
      D_ERROR("SDL_SetVideoMode failed");
      }*/
}


void SDL_GL_Screen::plot(int x, int y, Uint32 color)
{
    return;

    //FIXME:speed+implement alpha
    float r = (color&screen->format->Rmask)>>screen->format->Rshift;
    float g = (color&screen->format->Gmask)>>screen->format->Gshift;
    float b = (color&screen->format->Bmask)>>screen->format->Bshift;
    float a = 255;//(color&screen->format->Amask)>>screen->format->Ashift;

    glBegin(GL_POINTS);
    
    glColor4f(r/255.0,g/255.0,b/255.0, a/255.0);
    glVertex2i(x, y);    
    
    glEnd();
}

void SDL_GL_Screen::show(int x, int y, int w, int h)
{

    //draw here

    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_TEXTURE);
    
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    

    map<SDL_Surface*, RenderData>::iterator it;
    for(it = renderData.begin();it != renderData.end();it++)
    {
        if (it->second.verts.size() < 3)continue;

        SDL_Surface* s = it->second.surf;
        
        glPushMatrix();
        glScalef( 1.0/(double)s->w, 1.0/(double)s->h, 1);
        
        //it->second.texState.apply();
        
        glColor4f(0,0,0,0);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, &it->second.verts[0]);
        
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_INT, 0, &it->second.texCoords[0]);
        
        glDrawArrays(GL_TRIANGLES, 0, it->second.verts.size()/3);
        
        //it->second.texState.unApply();
        
        glPopMatrix();

        //clear for next run
        it->second.verts.clear();
        it->second.texCoords.clear();
    }
    
    glEnable(GL_BLEND);
    
    for(it = renderDataAlpha.begin();it != renderDataAlpha.end();it++)
    {
        if (it->second.verts.size() < 3)continue;

        SDL_Surface* s = it->second.surf;
        
        glPushMatrix();
        glScalef( 1.0/(double)s->w, 1.0/(double)s->h, 1);
        
        //it->second.texState.apply();
        
        glColor4f(0,0,0,0);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, &it->second.verts[0]);
        
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_INT, 0, &it->second.texCoords[0]);
        
        glDrawArrays(GL_TRIANGLES, 0, it->second.verts.size()/3);
        
        //it->second.texState.unApply();
        
        glPopMatrix();

        //clear for next run
        it->second.verts.clear();
        it->second.texCoords.clear();
    }

    glDisable(GL_BLEND);


    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

    glPopClientAttrib();

    //frame ready, show it
    SDL_GL_SwapBuffers();
    
    //start next frame
    blitLayer = -90;        
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


Uint32 SDL_GL_Screen::makeColor(Uint8 r, Uint8 g, Uint8 b)
{
    return (Uint32)SDL_MapRGB(screen->format, r, g, b);
}

bool SDL_GL_Screen::ok()
{
    return (screen!=NULL);
}

void SDL_GL_Screen::box(int x0, int y0, int x1, int y1, Uint32 color)
{
    int x,y,w,h;
    x = (x0<=x1) ? x0 : x1;
    y = (y0<=y1) ? y0 : y1;
    w=abs(x0-x1);
    h=abs(y0-y1);
    
    //FIXME:speed
    float r = (color&screen->format->Rmask)>>screen->format->Rshift;
    float g = (color&screen->format->Gmask)>>screen->format->Gshift;
    float b = (color&screen->format->Bmask)>>screen->format->Bshift;
    float a = 255;//(color&screen->format->Amask)>>screen->format->Ashift;

    //glEnable(GL_BLEND);
    glBegin(GL_QUADS);
    
    glColor4f(r/255.0,g/255.0, b/255.0, a/255.0);
    glVertex3f(x, y+h, blitLayer);
    glVertex3f(x+w, y+h, blitLayer);    
    glVertex3f(x+w, y, blitLayer);    
    glVertex3f(x, y, blitLayer);    
    
    glEnd();
    //glDisable(GL_BLEND);
    blitLayer += 0.01;
}

void SDL_GL_Screen::line(int x0, int y0, int x1, int y1, Uint32 color, int size)
{
    //FIXME:speed
    float r = (color&screen->format->Rmask)>>screen->format->Rshift;
    float g = (color&screen->format->Gmask)>>screen->format->Gshift;
    float b = (color&screen->format->Bmask)>>screen->format->Bshift;
    float a = 255;//(color&screen->format->Amask)>>screen->format->Ashift;

    glBegin(GL_LINES);
    
    glColor4f(r/255.0,g/255.0, b/255.0, a/255.0);
    glVertex3f(x0, y0, blitLayer);
    glVertex3f(x1, y1, blitLayer);    
    
    glEnd();

    blitLayer += 0.01;
}

}