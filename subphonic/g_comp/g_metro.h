#ifndef G_METRO_H
#define G_METRO_H

#include "g_common.h"
#include "../sig/signal_osc.h"


/*WARNING:
 * if two synced metros are mixed -> OVERFLOW!
 * 100bpm is not half of 200 because of roundoff!
 */

class GMetro : public SoundComp
{
  public :
    GMetro(BitmapFont* f, GroundMixer& g);
    ~GMetro();
   
    virtual string name() const
    {
        return string("GMetro");
    }
   
    Value** getOutput(unsigned int id);
    void addInput(Value** in, uint id);
    void remInput(Value** out, uint id);
   
    int syncToIndex(unsigned int index);
      
    void getState(SoundCompEvent* e);
    void setState(SoundCompEvent* e);
   
    class State
    {
      public:
        State(smp_t n, uint suppressed)
        {
            num=n;
            this->suppressed=suppressed;
        }
		
        smp_t num;
        uint suppressed;
    };
   
   
  private:
   
    class C_UpdateSup : public Action
    {
      public:
        C_UpdateSup(GMetro* src);
        void action(Component* c);
	
      private:
        GMetro* src;
    };
   
    SignalGenerator* sig;
    smp_t num;
    NumberSelect<smp_t>* ns;
    ConvertHz2Per* v;
    Const* a, *shp, *ck;
    SmpPoint* c;
    Gain* m;
   
    NumberSelect<uint>* n_sup;
    uint suppressed;
   
};

#endif
