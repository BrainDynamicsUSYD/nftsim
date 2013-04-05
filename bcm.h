#ifndef BCM_H
#define BCM_H

#include"cadp.h"

class BCM : public CaDP
{
  BCM(BCM&);
  BCM();
protected:
  struct BCMDE : public CaDE
  {
    BCMDE( int nodes, double deltat ) : CaDE(nodes,deltat) {
      extend(4);
    }
    virtual ~BCMDE(void) {}
    virtual void pot(void);
    virtual void dep(void);
    virtual void rhs( const vector<double>& y, vector<double>& dydt );
  };
  virtual void init( Configf& configf );
public:
  BCM( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop );
  virtual ~BCM(void);
  virtual void step(void);
  virtual void output( Output& output ) const;
};

#endif
