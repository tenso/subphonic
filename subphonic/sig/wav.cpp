#include "wav.h"
#include <string.h>

#include "../util/debug.h"
#include "../util/util.h"

namespace spl{

Wav::err Wav::make(char* data, int nbytes, int freq, int channels, int format)
{
    if(isok)
    {
        DERROR("already loaded");
        return ALREADY_LOADED;
    }
   
    //make header
   
    head.riff[0] = 'R';
    head.riff[1] = 'I';
    head.riff[2] = 'F';
    head.riff[3] = 'F';
   
    head.flen = sizeof(Header)+nbytes-8;
   
    head.wave[0] = 'W';
    head.wave[1] = 'A';
    head.wave[2] = 'V';
    head.wave[3] = 'E';
   
    head.fmt[0] = 'f';
    head.fmt[1] = 'm';
    head.fmt[2] = 't';
    head.fmt[3] = ' ';
   
    head.fmtlen=16;
    head.fmttag=1;
    head.channels=channels;
    head.samplerate=freq;
   
    head.bytepersec=freq*channels*format/8;
    head.blockalign=channels*format/8;
    head.bitspersample=format;
   
    head.data[0] = 'd';
    head.data[1] = 'a';
    head.data[2] = 't';
    head.data[3] = 'a';
   
    head.nbytes=nbytes;
   
    //copy data
    bytes = new char[nbytes];
    memcpy(bytes, data, nbytes);
   
    isok=true;
    return OK;
}


Wav::err Wav::save(const string& filename, bool force)
{
    if(!isok)
    {
        DERROR("not loaded");
        return NOT_LOADED;
    }
   
    if(!force)
    {
        ifstream in(filename.c_str());
        if(in)
        {
            DERROR("file exist");
            return FILE_EXIST_ERR;
        }
	
    }
   
    ofstream out(filename.c_str());
    if(!out)
    {
        DERROR("could not create");
        return FILE_CREATE_ERR;
    }
   
    out.write((char*)(&head),sizeof(Header));
    if(!out.good())
    {
        DERROR("(E) IO error");
        return FILE_IO_ERR;
    }
   
   
    out.write((char*)(bytes),head.nbytes);
    if(!out.good())
    {
        DERROR("(E) IO error");
        return FILE_IO_ERR;
    }
   
    out.close();
   
    return OK;
}


Wav::err Wav::load(const string& filename)
{
    ifstream in(filename.c_str(), ios::in | ios::binary);
   
    if(in==NULL)
    {
        DERROR("Loadwav: no such file:" << filename); 

	
        return FILE_NOTFOUND_ERR;
    }
   
    in.read((char*)(&head),TO_FMTLEN);
    if(!in.good())
    {
        DERROR("(E) IO error: " << filename);
        return FILE_IO_ERR;
    }
   
    if(head.fmttag!=1)
    {
        DERROR("Loadwav: fmttag!=1, not PCM dicontinuing");
        return NOT_PCM;
    }
   
   
    if(head.fmtlen==16)in.read((char*)(&head)+TO_FMTLEN, FROM_FMTLEN); //as usuall
    else //read extra bytes, could just skip them
    {
        int extra = head.fmtlen-16;
        //cout << "extra bytes:" << extra << endl;
	
        char* tmp = new char[extra];
        in.read(tmp, extra);
        if(!in.good())
        {
            DERROR("(E) IO error");
            delete tmp;
            return FILE_IO_ERR;
        }
	
	
        in.read((char*)(&head)+TO_FMTLEN,FROM_FMTLEN);
        if(!in.good())
        {
            DERROR("(E) IO error");
            delete tmp;
            return FILE_IO_ERR;
        }
	
        delete tmp; //what to do with extra data?
	
    }
   
    //OK, this is a hack, BAD CODE
    //if another chunk comes after this,--> faulty
    if(strncasecmp(head.data, "LIST", 4)==0)
    {
        int bytes = head.nbytes;
        //char adtl[4];
        //in.read(adtl,4);
        //cout << adtl << endl;
        //cout << bytes << endl;
        in.seekg(bytes,ios_base::cur);
	
        in.read(head.data,4);
        in.read((char*)&head.nbytes,4);
	
        //cout << head.nbytes << endl;
    }
   
   
    if(strncasecmp(head.data, "DATA", 4)!=0)
    {
        ERROR("not 'data' head, corrupt read follows");
    }
   
   
    bytes = new char[head.nbytes];
   
    in.read(bytes,head.nbytes);
   
    if(!in.good())
    {
        DERROR("(E) IO error");
        delete bytes;
        bytes=NULL;
        return FILE_IO_ERR;
    }
   
   
    in.close();
   
    isok=true;
   
    return OK;
}

//FIXME: make return stream
void Wav::info()
{
    if(!isok)
    {
        cout << "wav not made" << endl;
        return;
    }
   
   
    cout << "Wave File:" << endl
         << "channels: " << head.channels << endl
         << "samplerate: " << head.samplerate << endl
         << "bits/smpl: " << head.bitspersample << endl
         << "block align: " << head.blockalign << endl
         << "fmtlen: " << head.fmtlen << endl
         << "fmttag: " << head.fmttag << endl
         << "bytes: " << head.nbytes << endl << endl;
}


void Wav::del()
{
    if(bytes==NULL)return;
    delete[] bytes;
    isok=false;
}

Sample* Wav::makeSample(int chan)
{
    if(!isok)
    {
        DERROR("not loaded");
        return NULL;
    }
   
   
    Sample* samp;
   
    if(chan==EXTRACT_ALL)
    {
        samp = new Sample(head.nbytes/2);
        memcpy(samp->data, bytes, head.nbytes);
    }
    else
    {
        DASSERTP(isStereo(),"(W): trying to extract single channel from mono!");
	
        samp = new Sample(head.nbytes/4);
        short* dp = (short*)bytes;
	
        if(chan==EXTRACT_LEFT)
        {
            for(int i=0;i<head.nbytes/4;i++)
            {
                samp->data[i] = dp[i<<1];
            }
        }
        if(chan==EXTRACT_RIGHT)
        {
            for(int i=0;i<head.nbytes/4;i++)
            {
                samp->data[i] = dp[ 1+ (i<<1)];
            }
        }
	
    }
   
    return samp;
}

int Wav::getChans() const
{
    return head.channels;
}

int Wav::getSampleRate() const
{
    return head.samplerate;
}

int Wav::getBitsPerSample() const
{
    return head.bitspersample;
}

}