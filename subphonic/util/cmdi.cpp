#include "cmdi.h"
using namespace std;

CmdInterface::CmdInterface()
{

}

CmdInterface::~CmdInterface()
{
   
}

int CmdInterface::parseLine(const string& line) const
{
    string cmd("");
    string arg("");
    uint cmdlen=0;
    bool have_arg=false;
   
    size_t arg_pos = line.find_first_of(' ');
    if(arg_pos==string::npos)
    {
        have_arg=false;
        cmdlen=line.length();
        cmd=line;
    }
    else
    {
        have_arg=true;
        cmdlen = arg_pos;
        cmd = line.substr(0,arg_pos);
        if(line.size() != arg_pos+1)
        {
            arg = line.substr(arg_pos+1);
        }
        else
        {
            have_arg=false;
        }
    }
   
    if(cmdlen == 0)return 0;
   
    c_cmd_it it = find_if(cmds.begin(),cmds.end(), Cmd::Eq(cmd));
   
    if(it==cmds.end())return 0;
      
    if(have_arg)
    {
        char* c_arg = new char[arg.length()+1];
        memcpy(c_arg, arg.c_str(), arg.length()+1);
	
        it->fn(c_arg);
	
        delete[] c_arg;
    }
    else it->fn(NULL);
	     
    return 1;
   
}

//returns index of command, -1 if failed
int CmdInterface::add(const std::string& name, cmd_fun fn )
{
    //let "", NULL through
    if(name.size()!=0 && have(name))
    {
        DERROR("name exist");
        return -1;
    }
   
    if(name.size()==0 && fn!=NULL)
    {
        DERROR("no name");
        return -1; 
    }
   
   
    Cmd cmd;
    cmd.name=name;
    cmd.fn=fn;
   
    cmds.push_back(cmd);
   
    return cmds.size()-1;
}

bool CmdInterface::rem(const std::string& name)
{
    cmd_it it = find_if(cmds.begin(),cmds.end(), Cmd::Eq(name));
   
    if(it==cmds.end())
    {
        DERROR("not found");
        return false;
    }
   
   
    cmds.erase(it);
   
    return true;
}


bool CmdInterface::have(uint index) const
{
    return cmds.size() > index;
}


bool CmdInterface::have(const string& name) const
{
    c_cmd_it it = find_if(cmds.begin(),cmds.end(), Cmd::Eq(name));
   
    return it != cmds.end();
}


cmd_fun CmdInterface::getFun(uint index) const
{
    if(!have(index))return NULL;
   
    return cmds[index].fn;
}


std::string CmdInterface::getName(uint index) const
{
    if(!have(index))return string("");
   
    return cmds[index].name;
}
   
uint CmdInterface::num() const
{
    return cmds.size();
}
 

