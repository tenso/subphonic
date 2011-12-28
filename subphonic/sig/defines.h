#ifndef DEFINES_H
# define DEFINES_H

namespace spl{
//"primitive" defines: i.e no extra include
//NO: typedef list<smp_t> smp_list (et al)

//typedef double smp_t;
typedef double smp_t;
const static smp_t SMP_MAX=1.0f;

const static unsigned int DEF_SRATE = 44100;
  
//everything under this is considered a zero for the gates; default
#define GATE_ZERO_LVL 1e-10
}

#endif
