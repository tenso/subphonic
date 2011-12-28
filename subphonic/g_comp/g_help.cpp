#include "g_help.h"
#include "../main/config.h"

void convertCoords(const vector<SelectPlane::Coord>& in, vector<SigCoord>& out)
{
    vector<SelectPlane::Coord>::const_iterator it=in.begin();
   
    while(it!=in.end())
    {
        SigCoord sc;
        sc.x = it->x;
        sc.y = it->y;
        out.push_back(sc);
        it++;
    }
}


SoundComp* SCFactory(SCType id, int x, int y, BitmapFont* f, GroundMixer& ground)
{
    SoundComp* sc=NULL;
    GExecCmd* cmd=NULL;
   
    switch(id)
    {
#ifdef USE_ALT0
        case G_ALTZEROCTRL:
            sc = new GAltZeroCtrl(f, ground);
            break;
#endif
        case G_EDELAY:
            sc = new GEDelay(f, ground);
            break;
        case G_BITCRUSH:
            sc = new GBitCrush(f, ground);
            break;
        case G_BUFFER:
            sc = new GBuffer(f, ground);
            break;
        case G_ONOFF:
            sc = new GOnOff(f, ground);
            break;
        case G_JOYSTICK:
            sc = new GJoystick(f, ground);
            break;
        case G_CHANGEPULSE:
            sc = new GChangePulse(f, ground);
            break;
        case G_INTEGRATE:
            sc = new GIntegrate(f, ground);
            break;
        case G_LESSTHAN:
            sc = new GLessThan(f, ground);
            break;
        case G_LESSTHANEQUAL:
            sc = new GLessThanEqual(f, ground);
            break;
        case G_EXECCMD_PROCESS:
            cmd = new GExecCmd(f, ground);
            cmd->setCmdStr("process");
            sc=cmd;
            break;
        case G_EXECCMD_DUMPFORCE:
            cmd = new GExecCmd(f, ground);
            cmd->setCmdStr("dumpforce");
            sc=cmd;
            break;
        case G_SIGN:
            sc = new GSign(f, ground);
            break;
        case G_LINEAR:
            sc = new GLinear(f, ground);
            break;
        case G_SAMPLERATE:
            sc = new GSampleRate(f, ground);
            break;
        case G_MOD:
            sc = new GMod(f, ground);
            break;
        case G_SIN:
            sc = new GSin(f, ground);
            break;
        case G_FLOOR:
            sc = new GFloor(f, ground);
            break;
        case G_CEIL:
            sc = new GCeil(f, ground);
            break;
        case G_TRUNC:
            sc = new GTrunc(f, ground);
            break;
        case G_MIN:
            sc = new GMin(f, ground);
            break;
        case G_MAX:
            sc = new GMax(f, ground);
            break;
        case G_PLUS:
            sc = new GPlus(f, ground);
            break;
        case G_MINUS:
            sc = new GMinus(f, ground);
            break;
        case G_LOG:
            sc = new GLog(f, ground);
            break;
        case G_PRINT:
            sc = new GPrint(f, ground);
            break;
        case G_GATETOPULSE:
            sc = new GGateToPulse(f, ground);
            break;
        case G_KEYEXTR:
            sc = new GKeyExtr(f, ground);
            break;
        case G_SORT:
            sc = new GSort(f, ground);
            break;
        case G_DIVIDE:
            sc = new GDivide(f, ground);
            break;
        case G_MULT:
            sc = new GMult(f, ground);
            break;
        case G_DRAIN:
            sc = new GDrain(f, ground);
            break;
        case G_EXP:
            sc = new GExp(f, ground);
            break;
        case G_NOTEGAIN:
            sc = new GNoteGain(f, ground);
            break;
        case G_KEYBOARD:
            sc = new GKeyboard(f, ground);
            break;
        case G_PLAY:
            sc = new GPlay(f, ground);
            break;
        case G_STATETICK:
            sc = new GStateTick(f, ground);
            break;
        case G_MIDIKEY:
            sc = new GMIDIkey(f, ground);
            break;
        case G_MIDICTRL:
            sc = new GMIDIctrl(f, ground);
            break;
        case G_MIDIPGM:
            sc = new GMIDIpgm(f, ground);
            break;
        case G_BRIDGE:
            sc = new GBridge(f, ground);
            break;
        case G_DC:
            sc = new GDc(f, ground);
            break;
        case G_SLEW:
            sc = new GSlew(f, ground);
            break;
        case G_WAV:
            sc = new GWav(f, ground);
            break;
        case G_SAMPLE_AND_HOLD:
            sc = new GSampleAndHold(f, ground);
            break;
        case G_OSC:
            sc = new GOsc(f, ground);
            break;
        case G_MIX:
            sc = new GMix(f, ground);
            break;
        case G_GAIN:
            sc = new GGain(f, ground);
            break;
        case G_FILTER:
            sc = new GFilter(f, ground);
            break;
        case G_DELAY:
            sc = new GDelay(f, ground );
            break;
        case G_ABS:
            sc = new GAbs(f, ground );
            break;
        case G_CROSSFADE:
            sc = new GCrossfade(f, ground );
            break;
        case G_CLIP:
            sc = new GClip(f, ground );
            break;
        case G_AHDSR:
            sc = new GAhdsr(f, ground );
            break;
        case G_DOPPLER:
            sc = new GDoppler(f, ground );
            break;
        case G_ENV:
            sc = new GEnv(f, ground);
            break;
        case G_SCOPE:
            sc = new GScope(f, ground);
            break;
        case G_NOTE:
            sc = new GNote(f, ground); 
            break;
        case G_STEPMAT:
            sc = new GStepMat(f, ground); 
            break;
        case G_PUNCHPAD:
            sc = new GPunchPad(f, ground);
            break;
        case G_TRACKER:
            sc = new GTracker(f, ground); 
            break;
        case G_METRO:
            sc = new GMetro(f, ground);
            break;
        case G_VECTOR:
            sc = new GVector(f, ground);
            break;
        case G_DRAWENV:
            sc = new GDrawEnv(f, ground);
            break;
        case G_SELECTIN:
            sc = new GSelectIn(f, ground);
            break;
        case G_LOGISTICMAP:
            sc = new GLogisticMap(f, ground);
            break;
        case G_THRESHOLD:
            sc = new GThreshold(f, ground);
            break;
        case G_FFT_FILT:
            sc = new GFFTFilt(f, ground);
            break;
	
	
            /*      case G_WS:
                    sc = new GWaveShaper(f, ground );
                    break;*/
        case G_SPEC:
            sc = new GSpec(f, ground);
            break;
	
            /*OBSOLETE*/
        case G_ADSR:
            sc = new GAdsr(f, ground);
            break;
        case G_OCTSCALE:
            sc = new GOctScale(f, ground);
            break;
	
        default:
            stringstream s;
            s << "no such SCType: " << id << endl;
            DERROR(s.str());
            return NULL;
            break;
    }
   
    sc->setPos(x, y);
    sc->setSCType(id);
    return sc;
   
}
