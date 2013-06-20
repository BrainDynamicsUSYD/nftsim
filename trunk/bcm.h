#ifndef BCM_H
#define BCM_H

#include"cadp.h"

class BCM : public CaDP
{
  BCM(BCM&);
  BCM();
protected:
  //double t_BCM;
  struct BCMDE : public CaDE
  {
    double t_BCM;
    double gnmda_0;
    BCMDE( int nodes, double deltat ) : CaDE(nodes,deltat) {
      extend(1);
    }
    virtual ~BCMDE(void) {}
    virtual void init( Configf& configf );
    virtual void rhs( const vector<double>& y, vector<double>& dydt );
  };
public:
  BCM( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop );
  virtual ~BCM(void);
  virtual void output( Output& output ) const;
};

#endif
