/** @file longcouple.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_LONGCOUPLING_H
#define NEUROFIELD_SRC_LONGCOUPLING_H

#include"coupling.h"

class LongCoupling : public Coupling {
  LongCoupling();
  LongCoupling(LongCoupling&);
 protected:
  void init( Configf& configf ) override;

  vector< vector<double> > n2d; ///< nu tensor, nu[i][j] = from i to j
  int pos;
 public:
  LongCoupling( int nodes, double deltat, int index,
              const Propagator& prepropag, const Population& postpop );
  ~LongCoupling(void) override;
  void step(void) override;
  void output( Output& output ) const override;
};

#endif
