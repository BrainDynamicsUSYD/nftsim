#ifndef COUPLERAMP_H
#define COUPLERAMP_H

#include"couple.h"

class CoupleRamp : public Couple
{
  CoupleRamp();
  CoupleRamp(CoupleRamp&);
protected:
  double nu1,nu2,t1,t2;
  double time,deltanu;
public:
  void init( Configf& configf );
  void step(void);
  CoupleRamp( int nodes, double deltat, int index,
          const Propag& prepropag, const Population& postpop );
  virtual ~CoupleRamp(void);
};

#endif
