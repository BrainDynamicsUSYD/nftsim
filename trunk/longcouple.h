#ifndef LONG_COUPLE_H
#define LONG_COUPLE_H

#include"couple.h"

class LongCouple : public Couple
{
  LongCouple();
  LongCouple(LongCouple&);
protected:
  virtual void init( Configf& configf ); 

  vector< vector<double> > n2d; // nu tensor, nu[i][j] = from i to j
  int pos;
public: 
  LongCouple( int nodes, double deltat, int index,
      const Propag& prepropag, const Population& postpop );
  virtual ~LongCouple(void);
  virtual void step(void);
  virtual void output( Output& output ) const;
};

#endif
