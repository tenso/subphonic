#include "device.h"
#include "../util/debug.h"

using namespace std;

namespace spl
{

void SDLAudioDev::setFormat(int hz, Uint16 format , Uint8 chans, Uint16 smpls)
{
    wanted.freq=hz;
    wanted.format=format;
    wanted.channels=chans;
    wanted.samples = smpls;
}


SDL_AudioSpec SDLAudioDev::getObtainedFmt()
{
    if(!running)
    {
        D_MSG("device not running");
    }
    return obtained;
}


void SDLAudioDev::open(bool start_play)
{
    if(running)
    {
        D_MSG("device already running");
        return;
    }
   
    open_spec = wanted;
   
    work = SDL_OpenAudio(&wanted, &obtained);
   
    if(obtained.freq!=wanted.freq ||
    obtained.format!=wanted.format ||
    obtained.channels!=wanted.channels ||
    obtained.samples!=wanted.samples)
    {
        D_ERROR("unable to get wanted format, continue");
    }
   
    if(work==-1)
    {
        D_ERROR("failed to open audio");
        D_MSG(SDL_GetError());
    }
    else
    {
        setPlay(start_play);
        running=true;
    }
   
   
}

void SDLAudioDev::close()
{
    if(!running)
    {
        D_MSG("device not open");
        return;
    }
   
    setPlay(false);
    SDL_CloseAudio();
    running=false;
}

void SDLAudioDev::setPlay(bool v)
{
    if(v)SDL_PauseAudio(0);
    else SDL_PauseAudio(1);
}

bool SDLAudioDev::getPlay()
{
    SDL_audiostatus status = SDL_GetAudioStatus();
    return status==SDL_AUDIO_PLAYING;
}


SDLAudioDev::SDLAudioDev()
{
    wanted.callback = fill_audio;
    wanted.userdata = (void*)&data;
    setFormat(/*default*/);
   
    work=0;
    running=false;
}

SDLAudioDev::~SDLAudioDev()
{
    if(running)
    {
        D_ERROR("(W) audio device running");
        close();
    }
}

bool SDLAudioDev::working()
{
    return work==0;
}

bool SDLAudioDev::isOpen()
{
    return running;
}

void SDLAudioDev::playQueue(const char* data, int len)
{
    SDL_LockAudio();
    this->data.queue.push((const Uint8*)data, len);
    SDL_UnlockAudio();
}

int SDLAudioDev::inQueue()
{
    int ret=0;
    SDL_LockAudio();
    ret = data.queue.size();
    SDL_UnlockAudio();
    return ret;
}

void SDLAudioDev::playMix(const char* data, int len)
{
    SDL_LockAudio();
    this->data.mix.add((const Uint8*)data,len);
    SDL_UnlockAudio();
}

int SDLAudioDev::inMix()
{
    int ret=0;
    SDL_LockAudio();
    ret = data.mix.size();
    SDL_UnlockAudio();
   
    return ret;
}


void SDLAudioDev::clear()
{
    SDL_LockAudio();
    data.queue.clear();
    data.mix.clear();
    SDL_UnlockAudio();
}


/**********/
/*callback*/
/**********/

void fill_audio(void *udata, Uint8 *stream, int len)
{
    SoundDataQueue* qdata = &((SoundData*)udata)->queue;
   
    if(qdata->size()>0)
        mixQueue(stream, len, qdata);
   
    SoundDataMix* mdata = &((SoundData*)udata)->mix;
   
    if(mdata->size()>0)
        mixMix(stream, len, mdata);
   
}


void mixQueue(Uint8 *stream, int len, SoundDataQueue* data)
{
    int mixed=0;
    while(len>0 && data->size()>0)
    {
        QElement& elm = data->front();
	
        int elm_left=elm.len-elm.pos;
	
        //mix all that is possible from front then continue into next
        if( len > elm_left)
        {
	     
            SDL_MixAudio(stream+mixed,
            &elm.data[elm.pos], 
            elm_left, 
            SDL_MIX_MAXVOLUME);
	     
            //done with this one
            data->pop(); //alsa deletes data
	     
            //take rest from next element
            len-=elm_left;
            mixed+=elm_left;
        }
        //take everything from this element
        else 
        {
            SDL_MixAudio(stream+mixed,
            &elm.data[elm.pos],
            len, 
            SDL_MIX_MAXVOLUME);
	     
            elm.pos+=len;
	     
            len-=elm_left;
            //mixed+=elm_left;
        }
    }
}


void mixMix(Uint8 *stream, int len, SoundDataMix* data)
{
    list<QElement>::iterator it;
   
    for(it=data->data.begin();it!=data->data.end();)
    {
        QElement& elm = *it;
	
        int elm_left=elm.len-elm.pos;
	
        //mix all that is left
        if( len > elm_left)
        {
	     
            SDL_MixAudio(stream,
            &elm.data[elm.pos], 
            elm_left, 
            SDL_MIX_MAXVOLUME);
	     
            //done with this one
            delete elm.data;
            it = data->data.erase(it);
        }
        //take everything possible
        else 
        {
            SDL_MixAudio(stream,
            &elm.data[elm.pos],
            len, 
            SDL_MIX_MAXVOLUME);
	     
            elm.pos+=len;
	     
            it++;
        }
    }
}




/****************/
/*SoundDataQueue*/
/****************/

void SoundDataQueue::setMax(unsigned int max)
{
    this->max=max;
}

QElement& SoundDataQueue::front()
{
    return data.front();
}

void SoundDataQueue::pop()
{
    QElement q =data.front();
    //DONT DO THIS:
    delete q.data;
   
    data.pop();
}


void SoundDataQueue::push(const Uint8* bytes, int len)
{
    if(data.size()>=max)
    {
        D_ERROR("queue full, new element dropped");
        return;
    }
   
    QElement elm;
   
    //DONT DO THIS: instead have heap and just assign.
    elm.data=new Uint8[len];
    memcpy(elm.data, bytes, len);
   
    elm.len=len;
    elm.pos=0;
   
    data.push(elm);
}

int SoundDataQueue::size()
{
    return data.size();
}

void SoundDataQueue::clear()
{
    while(data.size()>0)
    {
        QElement& q =data.front();
        delete q.data;
	
        data.pop();
	
    }
   
   
}


/**************/
/*SoundDataMix*/
/**************/

void SoundDataMix::setMax(unsigned int max)
{
    this->max=max;
}

void SoundDataMix::add(const Uint8* bytes, int len)
{
    if(data.size()>=max)
    {
        D_ERROR("mix full, new element dropped");
        return;
    }
   
    QElement elm;
   
    elm.data=new Uint8[len];
    memcpy(elm.data, bytes, len);
   
    elm.len=len;
    elm.pos=0;
   
    data.push_back(elm);
}

int SoundDataMix::size()
{
    return data.size();
}

void SoundDataMix::clear()
{
    while(data.size()>0)
    {
        QElement& q = data.front();
        delete q.data;
	
        data.pop_front();
	
    }
}

}//end of namespace spl
