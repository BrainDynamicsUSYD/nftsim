#ifndef COUPLE_DIFF_ARCTAN_H
#define COUPLE_DIFF_ARCTAN_H

#include"couple.h"
using std::vector;

class Couple_diff_arctan : public Couple {
 Couple_diff_arctan();
 Couple_diff_arctan(Couple_diff_arctan&);
 protected:
  double nu_min, nu_max;
  double delt, t_half_down, t_half_up;
  double time_tot;
  double ramp, ramp_min, ramp_max;
  double time;
  double time_int;
  vector<double> deltanu;

 public:
   void init( Configf& configf );
   void step(void);
   void find(void);
  Couple_diff_arctan( int nodes, double deltat, int index,
          const Propag& prepropag, const Population& postpop, double tempf );
  virtual ~Couple_diff_arctan(void);
};

#endif

