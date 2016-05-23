#ifndef COUPLEATAN_H
#define COUPLEATAN_H

#include"couple.h"

using std::vector;

class CoupleAtan : public Couple
{
  CoupleAtan();
  CoupleAtan(CoupleAtan&);
    
protected:
  double nu0, nu_max;
  double delt, t1, t2;
  double time_t;
  double ramp, ramp_min, ramp_max;
  double time;
  vector<double> deltanu;

  
public:
  void init( Configf& configf );
  void step(void);
  void find(void);
  CoupleAtan( int nodes, double deltat, int index,
          const Propag& prepropag, const Population& postpop, double tempf );
  virtual ~CoupleAtan(void);
};

#endif
