/** @file long_coupling.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_LONGCOUPLING_H
#define NEUROFIELD_SRC_LONGCOUPLING_H

// C++ standard library headers
#include <vector> // std::vector;

// Neurofield headers
#include "configf.h"    // Configf;
#include "output.h"     // Output;
#include "coupling.h"   // Coupling;
#include "propagator.h" // Propagator;
#include "population.h" // Population;

class LongCoupling : public Coupling {
  LongCoupling();
  LongCoupling(LongCoupling&);
 protected:
  void init( Configf& configf ) override;

  std::vector< std::vector<double> > n2d; ///< nu tensor, nu[i][j] = from i to j
  int pos;
 public:
  LongCoupling( size_type nodes, double deltat, size_type index,
              const Propagator& prepropag, const Population& postpop );
  ~LongCoupling(void) override;
  void step(void) override;
  void output( Output& output ) const override;
};

#endif //NEUROFIELD_SRC_LONGCOUPLING_H
