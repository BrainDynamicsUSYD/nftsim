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
  struct fCaDE : public CaDE
  {
    double alpha, beta;
    double tau_x, tau_y;
    vector< deque<double> > xhistory1st;
    vector< deque<double> > xhistory2nd;
    vector< deque<double> > yhistory1st;
    vector< deque<double> > yhistory2nd;
    virtual void init( Configf& configf );
    fCaDE( int nodes, double deltat ) : CaDE(nodes,deltat),
      xhistory1st(nodes), xhistory2nd(nodes),
      yhistory1st(nodes), yhistory2nd(nodes) {}
    virtual ~fCaDE(void) {}
    void pot(void);
    void dep(void);
  };
  /*double alpha;
  vector< deque<double> > history1st; // == eta*( old dnudt in CaDP )
  vector< deque<double> > history2nd; // == D^-alpha(history1st) -eta*nu
  vector<double> oldnu; // to calculate old dnudt
  double init_nu; // initial condition*/

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
