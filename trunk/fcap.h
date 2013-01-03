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
  vector<double> oldnu;
  double alpha; // fractional integration order
  int order; // == ceil(alpha)

  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  fCaP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop );
  virtual ~fCaP(void);
  virtual void step(void);
  virtual const vector<double>& nu(void) const;
};

#endif
