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
  class FractionalIntegral
  {
    double alpha; // integral order
    double deltat;
    double init;
    double m; // value
    deque<double> history;
  public:
    void newHistory( double newest_history );
    void step(void);
    operator double() const { return m; }
    FractionalIntegral( double alpha, double deltat, double init )
        : alpha(alpha), deltat(deltat), init(init) {}
  };
  vector<FractionalIntegral*> newnu;
  vector<FractionalIntegral*> newnu2;
  double zeta;
  virtual void init( Configf& configf );
  virtual void step(void);
  fCaP( int nodes, double deltat, int index,
          const Propag& prepropag, const Population& postpop );
  virtual ~fCaP(void);
};

#endif
