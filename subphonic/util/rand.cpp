#include "rand.h"

# include <stdlib.h>
# include <time.h>

#include <iostream>

namespace spl{

Rand Rand::r;

 void Rand::seed(unsigned int s)
 {
     r.use_seed=s;
     srand(s);
     next();
 }
 
 double Rand::next()
 {
     return rand()/(double)(RAND_MAX);
 }

 double Rand::next(double min, double max)
 {
     return min+(max-min)*rand()/(double)(RAND_MAX);
 }

 int Rand::nextInt(int low, int high)
 {
     return (int)(low+(double)(high+1-low)*rand()/(double)(RAND_MAX+1.0));
 }
 
 Rand::Rand()
 {
     seed(time(NULL));
 }

}
