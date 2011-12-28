#ifndef RES_HOLD_H
#define RES_HOLD_H

#include <map>
#include <string>
#include <list>

#include "../util/debug.h"

using namespace std;

//singleton

//dont delete anything added to this(let it go)
//OK to add same pointer twice(under diff names)
template<class T>
class ResHold
{
  public:
    static ResHold& instance();
   
    T* operator[](const std::string k);
      
    //if strict it will return NULL and output error on not found, otherwise any pointer(from added) will be returned, NULL if no added
    T* get(const std::string name, bool strict=false);

    //return true with success
    bool add(const std::string name, T* s);
      
    void clear(); //deletes all content; so dont use pointer after that!
   
  protected:
   
    typedef std::map<std::string, T*> Data;
   
    static ResHold me;
   
    ResHold();
    ~ResHold();
   
    ResHold& operator=(const ResHold&);
    ResHold(const ResHold&);
      
    Data data;
   
    T* empty;
};

#include "res_hold_T.h"

#endif
