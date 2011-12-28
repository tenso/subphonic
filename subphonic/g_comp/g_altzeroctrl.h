#ifndef G_ALTZEROCTRL_H
#define G_ALTZEROCTRL_H

#include "g_common.h"
#include "../util/serial.h"
#include "alt0cmd.h"

#include <list>

//FIXME: parse these from alt0
static const uint NUM_GEN = 3;
static const uint SAMPLE_FREQ = 12000;

class AltZeroCtrl;

class GAltZeroCtrl : public SoundComp
{
  public :
    GAltZeroCtrl(BitmapFont* f, GroundMixer& g);
    virtual ~GAltZeroCtrl();
   
    virtual string name() const
    {
        return string("GAltZeroCtrl");
    }
   
    //Value** getOutput(unsigned int id);
    void addInput(Value** in, uint id);
    void remInput(Value** out, uint id);
    
  private:
    
    Serial* serial;
    AltZeroCtrl* sig;
};

//assume gen:4 lfo:8 (some data wasted...)

#define OSC_HZ 0
#define ENV_ON 4
#define ENV_A 8
#define ENV_D 12
#define ENV_S 16
#define ENV_R 20
#define OSC_WAVE 24
#define OSC_AMP 28
#define LFO_AMP_0 32
#define LFO_HZ_0 36
#define LFO_WAVE_0 40
#define LFO_AMP_1 44
#define LFO_HZ_1 48
#define LFO_WAVE_1 52
#define OSC_WS 56
#define OSC_GLIDE 60

class AltZeroCtrl : public Value
{
    static const uint NUM_INPUT=80;
    static const uint POLL_DELAY = 10000;
    static const uint MAX_CMDS_OUT = 20;
    //static const uint WRITE_DELAY= 1500; //FIXME: this must depend on sample rate

  public:
    AltZeroCtrl();
    
    smp_t nextValue();

    void setInput(Value** r, uint index);
    void setSerial(Serial* serial);

  private:

    class Generator
    {
      public:
        Generator();

        uint hz;
        bool on;
        uint a;
        uint d;
        uint s;
        uint r;
        
        uint amp;
        uint wave;

        int lastTrigTime;
        int lastTrigIndex;
        
        int dt;
        int t;
        uint ws;
        
        uint lfoHz[2];
        uint lfoAmp[2];
        uint lfoWave[2];

        uint glide;
    };

    class AltZeroCmd
    {
      public:
        char data[CMD_BYTES];
        int atIndex;
        int atTime;

        int trigAtTime;
    };
    bool pushCmd(CmdTypeId type, CmdGroupId group, char param, char id, unsigned short data);    

    
    Generator gen[NUM_GEN];
    Value** inputs[NUM_INPUT];
    Serial* serial;
    list<AltZeroCmd> cmdsOut;

    char altZeroCmd[CMD_BYTES];
    char altZeroResp[CMD_BYTES];
    uint altZeroRespCount;
    uint writeDelay;
    
    static ProgramState& progstate;
    int startSec;
};

#endif
