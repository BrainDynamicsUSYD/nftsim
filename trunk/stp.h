#ifndef STP_H
#define STP_H

#include"couple.h"

class STP : public Couple
{
  STP();
  STP(STP&);
protected:
  vector<double> Ca;
  vector<double> glu;
  vector<double> oldlngluonphi; // for calculating time derivative

  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  STP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepop, const Population& postpop );
  virtual ~STP(void);
  virtual void step(void);
  virtual vector<Output*> output(void) const;
};

#endif
