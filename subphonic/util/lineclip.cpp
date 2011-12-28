#include "lineclip.h"

namespace spl{

bool clipLine(Vec4& line, const Vec4& bound)
{
    bool run=true;
    bool draw=false;
    code c0,c1,out;
   
    c0 = pointCode(line[0], line[1], bound);
    c1 = pointCode(line[2], line[3], bound);
   
    float xslop = (line[2]-line[0])/(line[3]-line[1]);
    float yslop = (line[3]-line[1])/(line[2]-line[0]);
   
    while(run)
    {
		
        if(!(c0|c1))
        {
            draw=true;
            run=false;
            continue;
        }
        if(c0&c1)
        {
            run=false;
            continue;
        }
	
        out = c0 ? c0 : c1;
	
        float newx,newy;
	
        if(out&TOP)
        {
            newx = line[0]+xslop*(bound[3]-line[1]);
            newy=bound[3];
        }
        else if(out&BOTTOM)
        {
            newx = line[0]+xslop*(bound[1]-line[1]);
            newy=bound[1];
        }
        else if(out&RIGHT)
        {
            newx = bound[2];
            newy= line[1]+yslop*(bound[2]-line[0]);
        }
        else
        {
            newx = bound[0];
            newy= line[1]+yslop*(bound[0]-line[0]);
        }
	
        if(out==c0)
        {
            line[0]=newx;
            line[1]=newy;
            c0=pointCode(line[0],line[1],bound);
        }
        else
        {
            line[2]=newx;
            line[3]=newy;
            c1=pointCode(line[2],line[3],bound);
        }
    }
   
    return draw;	  
}

code pointCode(float x, float y, const Vec4& bound)
{
    code c=0;
    if(y>bound[3])c|=TOP;
    else if(y<bound[1])c|=BOTTOM;
    if(x>bound[2])c|=RIGHT;
    else if(x<bound[0])c|=LEFT;
   
    return c;
}
}
