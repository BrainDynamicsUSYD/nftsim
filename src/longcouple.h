/** @file longcouple.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_LONGCOUPLE_H
#define NEUROFIELD_SRC_LONGCOUPLE_H

#include"couple.h"

class LongCouple : public Couple {
  LongCouple();
  LongCouple(LongCouple&);
 protected:
  void init( Configf& configf ) override;

  vector< vector<double> > n2d; // nu tensor, nu[i][j] = from i to j
  int pos;
 public:
  LongCouple( int nodes, double deltat, int index,
              const Propag& prepropag, const Population& postpop );
  ~LongCouple(void) override;
  void step(void) override;
  void output( Output& output ) const override;
};

#endif
