#ifndef BCM_H
#define BCM_H

#include<vector>
using std::vector;
#include"cadp.h"

class BCM : public CaDP
{
  BCM();
  BCM(BCM&);

  vector< vector<double> > history; // keyring of postpop firing
  int key; // key to keyring
  Dumpf dumpf; // output th
protected:
  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public:
  BCM( int nodes, double deltat, int index, const vector<double>& glu,
          const Population& prepop, const Population& postpop );
  virtual ~BCM(void);
  virtual void step(void);
};

#endif
