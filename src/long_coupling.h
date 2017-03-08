/** @file long_coupling.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_LONGCOUPLING_H
#define NEUROFIELD_SRC_LONGCOUPLING_H

// Other neurofield headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class LongCoupling : public Coupling {
  LongCoupling();                    // No default constructor allowed.
  LongCoupling(const LongCoupling&); // No copy constructor allowed.
 protected:
  void init( Configf& configf ) override;

  std::vector< std::vector<double> > n2d; ///< nu tensor, nu[i][j] = from i to j
  int pos = 0;
 public:
  LongCoupling( size_type nodes, double deltat, size_type index,
              const Propagator& prepropag, const Population& postpop );
  ~LongCoupling(void) override;
  void step(void) override;
  void output( Output& output ) const override;
};

#endif //NEUROFIELD_SRC_LONGCOUPLING_H
