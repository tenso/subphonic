#ifndef UTIL_FILE_IO
# define UTIL_FILE_IO

#include <errno.h>

#include <sys/types.h>
#include <dirent.h>

#include <string>
#include <vector>

#include "debug.h"

class FileDir
{
  public:
    enum TYPE {T_FILE, T_DIR, T_UNKNOWN };
   
    FileDir();
    ~FileDir();
   
    //run once then use getFiles as many times as needed(unless re-read)
    //check if return = true, before getFiles(), otherwise dir did not exist
    //posfix should be complete with dot i.e: ".wav"
    bool read(const std::string& path, bool read_hidden=true,
    const std::string& only_with_postfix="", bool postfix_case=false
    , bool read_dirs=true);
      

    //FIXME: name: getEntries
    //unsorted
    void getFiles(std::vector<std::string>& ret);
    std::string getPath();
   
    void getTypes(std::vector<TYPE>& ret);
    
  private:
    std::vector<std::string> files;
    std::string path;
    std::vector<TYPE> types;
};


#endif
