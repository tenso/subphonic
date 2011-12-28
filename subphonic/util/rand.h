#ifndef SPL_RAND_H
# define SPL_RAND_H

namespace spl{

class Rand
{
  public:
   
    static void seed(unsigned int s);
    static double next();
    static double next(double min, double max);
    static int nextInt(int low, int high);
   
private:
    Rand();
    unsigned int use_seed;
    static Rand r;
};

}

#endif
