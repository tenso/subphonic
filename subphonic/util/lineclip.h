#ifndef SPL_LINECLIP_H
#define SPL_LINECLIP_H

#include "../math/vec4.h"

namespace spl{

//line=[x0,y0,x1,y1]
//bound=[ xmin,  ymin,  xmax,  ymax]
//returns true if line is inside or clipped, i.e false if completely outside
bool clipLine(Vec4& line, const Vec4& bound);

typedef unsigned int code;
enum{TOP=0x1, BOTTOM=0x2, RIGHT=0x4, LEFT=0x8};
code pointCode(float x, float y, const Vec4& bound);

}

#endif
