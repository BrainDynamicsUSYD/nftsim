#ifndef FCAP_H
#define FCAP_H

#include<deque>
using std::deque;
#include"cadp.h"

class fCaP : public CaDP
{
  fCaP();
  fCaP(fCaP&);
protected:
  vector< deque<double> > drive; // == old dnudt in CaDP
  vector<double> oldnu; // to calculate old dnudt
  double alpha; // fractional integration order
  double init_nu;

  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  fCaP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop );
  virtual ~fCaP(void);
  virtual void step(void);
};

#endif
