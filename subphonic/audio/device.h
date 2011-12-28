#ifndef SPL_DEVICE_H
# define SPL_DEVICE_H

# include <queue>
# include <list>
# include <SDL.h>

namespace spl
{

/*SDL DEFINES:
  
  typedef struct{
  int freq;
  Uint16 format;
  Uint8 channels;
  Uint8 silence;
  Uint16 samples;
  Uint32 size;
  void (*callback)(void *userdata, Uint8 *stream, int len);
  void *userdata;
  } SDL_AudioSpec;
  
*/

/*SDL_FORMATS
  AUDIO_U8 	Unsigned 8-bit samples.
  AUDIO_S8 	Signed 8-bit samples.
  AUDIO_U16 or AUDIO_U16LSB 	not supported by all hardware (unsigned 16-bit little-endian)
  AUDIO_S16 or AUDIO_S16LSB 	not supported by all hardware (signed 16-bit little-endian)
  AUDIO_U16MSB 	not supported by all hardware (unsigned 16-bit big-endian)
  AUDIO_S16MSB 	not supported by all hardware (signed 16-bit big-endian)
  AUDIO_U16SYS 	Either AUDIO_U16LSB or AUDIO_U16MSB depending on hardware CPU endianness
  AUDIO_S16SYS 	Either AUDIO_S16LSB or AUDIO_S16MSB depending on hardware CPU endianness
*/


/*name missleading, not a queue element*/

class QElement
{
  public:
    QElement()
    {
        data=NULL;
        len=0;
        pos=0;
    }
   
    Uint8* data;
    int len;
    int pos;
};


class SoundDataQueue
{
  public:
   
    SoundDataQueue()
    {
        max=32;
    }
   
    void setMax(unsigned int max);
   
    QElement& front();
   
    void pop();
    void push(const Uint8* bytes, int len);
   
    int size();
    void clear();
   
  private:
    std::queue<QElement> data;
    unsigned int max;
};

class SoundDataMix
{
  public:
    SoundDataMix()
    {
        max=32;
    }
   
    void setMax(unsigned int max);
    void add(const Uint8* bytes, int len);
   
    int size();
    void clear();
   
    std::list<QElement> data;
   
  private:
    unsigned int max;
   
};

struct SoundData
{
    SoundDataQueue queue;
    SoundDataMix   mix;
};

//WANRING: do not call SDL_LockAudio()
//play() enques sample for mixing
class SDLAudioDev /*: public AudioDev*/
{
  public:
    SDLAudioDev();
    ~SDLAudioDev();
   
    //defaults are as default args in setFormat() etc
   
    //hz: 44100, 22050, etc
    //format: AUDIO_U8 AUDIO_S8 AUDIO_U16, AUDIO_S16, etc (see SDL man)
    //chans: 1,2,4,6
    //if set after open(): a close() + setFromat() + open() required
    void setFormat(int hz=44100, Uint16 format=AUDIO_S16 , Uint8 chans=1, Uint16 smpls=4096);
   
    SDL_AudioSpec getObtainedFmt();
   
    void open(bool start_play=true);
    void close();
    bool isOpen();
   
    void setPlay(bool v);
    bool getPlay();
   
    bool working();
   
    //copys data to device(ok to free after return)
    //if stereo just play data as left|right chunks (i.e interleaved)...
   
    //OK to use both queue and mix at once
    void playQueue(const char* data, int len);
    int inQueue();
   
    void playMix(const char* data, int len);
    int inMix();
   
    //empty all playCalls()
    void clear();
   
  private:
   
    SoundData data; //shared and protected by SDL_LockAudio()
   
    SDL_AudioSpec wanted, obtained, open_spec;
    bool running;
    int work;
};


//callback for SDL
void fill_audio(void *udata, Uint8 *stream, int len); //shares SDLAudioDev::data

void mixQueue(Uint8 *stream, int len, SoundDataQueue* data);
void mixMix(Uint8 *stream, int len, SoundDataMix* data);

typedef SDLAudioDev AudioDev;

}
#endif
