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
  double alpha;
  double t;
  vector< deque<double> > local; // local history of drive, used for averaging
  vector< deque<double> > drive; // == old dnudt in CaDP
  vector<double> oldnu; // to calculate old dnudt
  vector<double> init_nu; // initial condition(s)

  struct fCaDE : public CaDE
  {
    vector< deque<double> > xhistory;
    vector< deque<double> > xlocal;
    vector< deque<double> > yhistory;
    vector< deque<double> > ylocal;

    fCaDE( int nodes, double deltat )
        : CaDE(nodes,deltat), xhistory(nodes), xlocal(nodes),
        yhistory(nodes), ylocal(nodes) {}
    virtual ~fCaDE(void) {}
    virtual void pot( vector<double>& Ca, vector<double>& x );
    virtual void dep( vector<double>& Ca, vector<double>& y );
  };

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
