#ifndef CMDI_H
#define CMDI_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <string.h>
#include <algorithm>

#include "debug.h"
#include "types.h"

//FIXME: change void* => to const string
typedef void (*cmd_fun)(void* arg);

class Cmd
{
  public:
    std::string name;
    cmd_fun fn;
   
    class Eq : public std::unary_function<class Cmd, bool>
    {
      public:
        explicit Eq(const std::string& s) : name(s)
        {
        }
	
        result_type operator()(const argument_type& a)
        {
            return a.name==name;
        }
	
	
      private:
        std::string name;
    };
};

class CmdInterface
{
  public:
    //typedef void (*fn_type)(void* arg);
   
    CmdInterface();
    ~CmdInterface();
   
    //returns 1 if ok(found), 0 otherwise
    int parseLine(const std::string& line) const;
   
    //returns index of command, -1 if failed
    //possible to use add("", NULL) as delimiter
    int add(const std::string& name, cmd_fun fn );
    bool rem(const std::string& name);
   
    //is command with 'name' added?
    bool have(uint index) const;
    bool have(const std::string& name) const;
   
    //returns empty string if not added
    cmd_fun getFun(uint index) const;
    std::string getName(uint index) const;
   
    uint num() const;
   
  private:
    typedef std::vector<Cmd>::iterator cmd_it;
    typedef std::vector<Cmd>::const_iterator c_cmd_it;
   
    std::vector<Cmd> cmds;
};

#endif
