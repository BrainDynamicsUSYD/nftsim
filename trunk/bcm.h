#ifndef BCM_H
#define BCM_H

#include"cadp.h"

class BCM : public CaDP
{
  BCM();
  BCM(BCM&);

  double gain;
protected:
  virtual void init( Configf& configf );
public:
  BCM( int nodes, double deltat, int index, const vector<double>& glu,
          const Population& prepop, const Population& postpop );
  virtual ~BCM(void);
  virtual void step(void);
  virtual vector<Output*> output(void) const;
};

#endif
