#ifndef DENDRITEDYNAMIC_H
#define DENDRITEDYNAMIC_H

#include "dendrite.h"

class DendriteDynamic : public Dendrite // inherits the virtual functions as virtual
{
  DendriteDynamic(void); // default constructor
  DendriteDynamic(Dendritedynamic& ); // no copy constructor

  double alpha1, alpha2;
  double beta1, beta2;
  double t1, t2, t3, t4;
  double time;

protected:
  virtual void init( Configf& configf );
public: 
  DendriteDynamic( int nodes, double deltat, int index,
      const Propag& prepropag, const Couple& precouple );
  virtual ~DendriteDynamic(void);
  virtual void step(void);
};

#endif
