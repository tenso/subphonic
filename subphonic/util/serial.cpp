#include "serial.h"

#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
using namespace std;

Serial::Serial()
{
    _devFd = -1;
}

Serial::~Serial()
{
    if(_devFd >= 0)
    {
        cout << "~Serial:: device still open" << endl;
        close();
    }
}

bool Serial::open(string dev, speed_t baud)
{
    _devFd = ::open(dev.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(_devFd < 0)
    {
        cout << "Serial::open failed" << endl;
        return false;
    }

    int ret = 0;
    
    struct termios termios;
    memset(&termios, 0, sizeof(termios));
    
    termios.c_cflag = CREAD | CLOCAL | CS8;
    
    ret = cfsetspeed(&termios,  baud);
    if (ret != 0)
    {
        cout << "error setting serial attr" << endl;
        close();
    }

    tcflush(_devFd, TCIOFLUSH);
    tcsetattr(_devFd, 0, &termios);

    return true;
}

bool Serial::close()
{
    int ret = ::close(_devFd);
    _devFd = NULL;

    if(ret < 0)
    {
        cout << "Serial::close failed" << endl;
        return false;
    }
    return true;
}
//returns true if data available

//true on sucess
int Serial::read(string* ret)
{
    *ret = string();
    char buff[255];
    int totDone=0;
    while(true)
    {
        int nRead = ::read(_devFd, buff, 254);
        if(nRead == -1)
        {
            cout << "Serial:: error read" << endl;
            return -1;
        }
        totDone+=nRead;

        if(nRead)
        {
            ret->append(buff, nRead);
        }
        if(nRead < 254)break;
    }
    return totDone;
}

int Serial::read(char* ret, int dataLen)
{
    int nRead = ::read(_devFd, ret, dataLen);
    if(nRead == -1)
    {
        cout << "Serial:: error read" << endl;
        return -1;
    }
    return nRead;
}

bool Serial::write(string data)
{
    int toDo = data.size(); //trailing \0
    int done = 0;

    int sret;
    fd_set wfds;
    struct timeval timeval;

    while (done < toDo)
    {
        timeval.tv_usec = 1000;
        timeval.tv_sec = 0;
        FD_ZERO(&wfds);
        FD_SET(_devFd, &wfds);
        sret = select(_devFd+1, NULL, &wfds, NULL, &timeval);
        
        if(sret == -1)
        {
            cout << "Serial:: select() error" << endl;
        }
        if(sret)
        {
            int ret = ::write(_devFd, data.c_str(), toDo-done);
            if (ret == -1)
            {
                //11: try again 
                cout << "Serial:: error write:" << errno << endl;
                return false;
            }
            done+=ret;
        }
    }
    return true;
}

bool Serial::write(const char* data, int dataLen)
{
    int toDo = dataLen;
    int done = 0;

    int sret;
    fd_set wfds;
    struct timeval timeval;

    while (done < toDo)
    {
        timeval.tv_usec = 1000;
        timeval.tv_sec = 0;
        FD_ZERO(&wfds);
        FD_SET(_devFd, &wfds);
        sret = select(_devFd+1, NULL, &wfds, NULL, &timeval);
        
        if(sret == -1)
        {
            cout << "Serial:: select() error" << endl;
        }
        if(sret)
        {
            int ret = ::write(_devFd, data, toDo-done);
            if (ret == -1)
            {
                //11: try again 
                cout << "Serial:: error write:" << errno << endl;
                return false;
            }
            done+=ret;
        }
    }
    return true;
}

int Serial::pollWrite()
{
    fd_set wfds;
    struct timeval timeval;
    timeval.tv_usec = 0;
    timeval.tv_sec = 0;
    FD_ZERO(&wfds);
    FD_SET(_devFd, &wfds);
    return select(_devFd+1, NULL, &wfds, NULL, &timeval);
}

void Serial::drain()
{
    if(_devFd >= 0)
        tcdrain(_devFd);
}
