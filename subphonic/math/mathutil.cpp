#include "mathutil.h"

namespace spl{

bool isfinite(double num)
{
    return !_isnan(num);
}

double log2(double num)
{
    return log(num)/log(2.0);
}

double round(double val)
{
    return floor(val+0.5);
}

}//end spl namespace
