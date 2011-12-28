#include "util.h"

namespace spl
{

std::string toLower(const std::string& s)
{
    std::string ret=s;
   
    for(unsigned int i=0;i<ret.length();i++)ret[i]=tolower(ret[i]);
   
    return ret;
}

std::string toUpper(const std::string& s)
{
    std::string ret=s;
   
    for(unsigned int i=0;i<ret.length();i++)ret[i]=toupper(ret[i]);
   
    return ret;
}

int strncasecmp(const char* s1, const char* s2, int n)
{
    std::string ss1(s1);
    std::string ss2(s2);
    return strncmp(toLower(ss1).c_str(), toLower(ss2).c_str(), n);
}

}
