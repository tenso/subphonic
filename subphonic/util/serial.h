#ifndef SERIAL_H
#define SERIAL_H

#include <string>

class Serial
{
  public:
    Serial();
    ~Serial();
    
    //true on sucess
    bool open(std::string dev, /*speed_t*/int baud);
    bool close();
    //returns true if data available
    //bool pollRead(); //non blocking anyways
    
    //returns > 0 if write is ok
    //-1 on error
    int pollWrite();

    
    //return number read
    int read(std::string* ret);
    int read(char* ret, int dataLen);
    
    bool write(std::string data);
    bool write(const char* data, int dataLen);

    //drains all unwritten data
    void drain();

  private:
    int _devFd;
};

#endif
