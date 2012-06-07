#ifndef STP_H
#define STP_H

#include"couple.h"

class STP : public virtual Couple
{
  STP();
  STP(STP&);

  vector<double> oldphi;
protected:
  double rate;
  double nu_0;
  double phi_r;

  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  STP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepop, const Population& postpop );
  virtual ~STP(void);
  virtual void step(void);
};

#endif
