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
    this->data.mix.push((const Uint8*)data,len);
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
    SoundDataRing* qdata = &((SoundData*)udata)->queue;
    if(qdata->size()>0)
        sdlMix(stream, len, qdata, true);
   
    SoundDataRing* mdata = &((SoundData*)udata)->mix;
    if(mdata->size()>0)
        sdlMix(stream, len, mdata, false);
}


void sdlMix(Uint8 *stream, int len, SoundDataRing* data, bool queueMode)
{
    int mixed=0;
    while(len>0 && data->size()>0)
    {
        QElement& elm = data->front();
	
        int elm_left=elm.len-elm.pos;
	
        //mix all that is possible from front then continue into next
        if( len > elm_left)
        {
            SDL_MixAudio(stream+mixed, &elm.data[elm.pos], elm_left, SDL_MIX_MAXVOLUME);
	     
            //done with this one
            data->pop(); 
	     
            //take rest from next element
            len-=elm_left;
            if (queueMode)mixed+=elm_left;
        }
        //take everything from this element
        else 
        {
            SDL_MixAudio(stream+mixed, &elm.data[elm.pos], len, SDL_MIX_MAXVOLUME);
            elm.pos+=len;
            len-=elm_left;
        }
    }
}


/***************/
/*SoundDataRing*/
/***************/

SoundDataRing::SoundDataRing()
{
    clear();
}

QElement& SoundDataRing::front()
{
    if (size() == 0)
    {
        D_ERROR("ERROR: no elements!");
    }
    
    return data[back];
}

void SoundDataRing::pop()
{
    if(size() == 0)
    {
        D_ERROR("queue empty");
        return;
    }
    back++;
    back %= SOUND_MAX_QUEUE_LEN;
    queueSize--;
}

void SoundDataRing::push(const Uint8* bytes, int len)
{
    if(size() >= SOUND_MAX_QUEUE_LEN)
    {
        D_ERROR("queue full, new element dropped");
        return;
    }
       
    memcpy(data[queueFront].data, bytes, len);
    data[queueFront].len = len;
    data[queueFront].pos = 0;

    queueFront++;
    queueSize++;
    queueFront %= SOUND_MAX_QUEUE_LEN;
}

int SoundDataRing::size()
{
    return queueSize;
}

void SoundDataRing::clear()
{
    queueFront = 0;
    back = 0;
    queueSize = 0;
}

}//end of namespace spl
