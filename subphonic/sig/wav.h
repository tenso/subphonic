#ifndef WAV_H
#define WAV_H

#include <iostream>
#include <fstream>

#include "sample.h"

namespace spl {
/*NOW THIS HERE IS HARDLY GOOD CODE,  only loads most basic wav*/

#define TO_FMTLEN 36
#define FROM_FMTLEN 8

using namespace std;

class Wav
{
    //startheader
    struct Header
    {
        //riff head
        char riff[4]; //spell: R,I,F,F
        int flen; //following size
        char wave[4]; //spell: W,A,V,E
	
        //fmt head
        char fmt[4]; //spell: f,m,t,' '
        int fmtlen;  //16+extra                   //20bytes here
        short fmttag;//1:uncompressed PCM
        short channels; //1,2 mono,stereo
        int samplerate; //frequency: smpls/sec
        int bytepersec; //smprate*balign 
        short blockalign; //channels*byte/sample 
        short bitspersample; //8,16              //36bytes here, TO_FMTLEN
        //extra here
	
        //data head
        char data[4]; //spell: d,a,t,a
        int nbytes;
	
        //for i,n,f,o: same + 4bytes a,d,t,l + data(nybtes)
    };
    //endheader - 44byte total
   
  public:
   
    enum err {FILE_EXIST_ERR, FILE_CREATE_ERR, FILE_IO_ERR, FILE_NOTFOUND_ERR, 
              NOT_LOADED, ALREADY_LOADED, NOT_PCM, OK };
   
   
    Wav()
    {
        isok=false;
        bytes=NULL;
    }
   
    err load(const string& filename);
   
    //make empty, make sure not to have loaded or anything
    err make(char* data, int nbytes, int freq, int channels, int format);
   
    //if force overwrite existing file 
    err save(const string& filename, bool force=true);
   
    //FIXME: make return stream
    void info();
   
    void del();
   
    static const int EXTRACT_ALL=-1;
    static const int EXTRACT_LEFT=0;
    static const int EXTRACT_RIGHT=1;
   
    //if stereo sound and want one channel only use EXTRACT_LEFT/RIGHT
    Sample* makeSample(int chan=EXTRACT_ALL);
   
    uint getNumBytes() const
    {
        return head.nbytes;
    }
   
    const char* getBytes() const
    {
        return bytes;
    }
   
   
    int getChans() const;
    int getSampleRate() const;
    int getBitsPerSample() const;
   
    bool isStereo() const
    {
        return (head.channels==2);
    }
   
      
    bool ok() const
    {
        return isok;
    }
   
  private:
    bool isok;
    char* bytes;
    Header head;

};

}

#endif
