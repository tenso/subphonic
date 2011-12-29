#ifndef SPL_DEVICE_H
# define SPL_DEVICE_H

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

#define SOUND_MAX_DATA_LEN 8192
#define SOUND_MAX_QUEUE_LEN 32

class QElement
{
  public:
    QElement()
    {
        len=0;
        pos=0;
    }
   
    Uint8 data[SOUND_MAX_DATA_LEN];
    int len;
    int pos;
};

class SoundDataRing
{
public:
    SoundDataRing();
    QElement& front();
    void pop();
    void push(const Uint8* bytes, int len);
    int size();
    void clear();

protected:
    QElement data[SOUND_MAX_QUEUE_LEN];
    int queueFront;
    int back;
    int queueSize;
};

struct SoundData
{
    SoundDataRing queue;
    SoundDataRing mix;
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

void sdlMix(Uint8 *stream, int len, SoundDataRing* data, bool queueMode);

typedef SDLAudioDev AudioDev;

}
#endif
