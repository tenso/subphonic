#include "file_io.h"

#include "../util/util.h"
#include "../util/debug.h"

using namespace std;

FileDir::FileDir()
{
}

FileDir::~FileDir()
{
}


bool FileDir::read(const string& path, bool read_hidden,
const string& only_with_postfix, bool postfix_case,
bool read_dirs)
{
    files.clear();
   
    this->path=path;
   
    DIR* dir = opendir(path.c_str());
   
    if(dir==NULL)
    {
        // /usr/include/asm-generic/errno-base.h
        // /usr/include/asm-x86_64/errno.h
	
        ERROR("no such dir: " << path << " ;or error: " << errno);
        return false;
    }
   
    //read dir
    dirent* ent;      
    while((ent = readdir(dir))!=NULL)
    {
        if(!read_hidden && ent->d_name[0] == '.')continue;
        if(!read_dirs    && ent->d_type    == DT_DIR) continue;
	
        string s(ent->d_name);
		
        if(ent->d_type !=DT_DIR && only_with_postfix.length()!=0)
        {
            if(postfix_case)
            {
                s = spl::toLower(s);
            }
	     
            size_t at = s.find(only_with_postfix);
	     
            if(s.length() < only_with_postfix.length())continue;
	     
            if(at != (s.length()-only_with_postfix.length()))continue;
        }
	
        //messes up portability, suppsedly

        if(ent->d_type==DT_DIR)types.push_back(T_DIR);
        else if(ent->d_type==DT_UNKNOWN)types.push_back(T_UNKNOWN);
        else types.push_back(T_FILE);
	
        files.push_back(ent->d_name);
     
    }
    //done
   
   
    int err = closedir(dir);
      
    if(err==-1)
    {
        ERROR("error: " << errno);
        return false;
    }
   
    return true;
}


void FileDir::getFiles(vector<string>& ret)
{
    ret=files;
}

void FileDir::getTypes(vector<TYPE>& ret)
{
    ret=types;
}


string FileDir::getPath()
{
    return path;
}
