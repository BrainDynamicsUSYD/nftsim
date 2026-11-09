/** @file long_coupling.h
  @brief Declares LongCoupling (`Coupling N: Matrix`), a coupling between every pair of nodes.

  LongCoupling gives each pair of nodes its own synaptic strength, so the
  input to node j is P[j] = sum_i nu[i][j] phi[i], with nu given as one value
  or a Nodes x Nodes matrix. It supports spatially nonuniform connectivity
  and costs O(Nodes^2) per step.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_LONGCOUPLING_H
#define NFTSIM_SRC_LONGCOUPLING_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class LongCoupling : public Coupling {
 protected:
  void init( Configf& configf ) override;

  std::vector< std::vector<double> > n2d; ///< nu tensor, nu[i][j] = from i to j

 public:
  LongCoupling() = delete;                    // No default constructor allowed.
  LongCoupling(const LongCoupling&) = delete; // No copy constructor allowed.

  LongCoupling( size_type nodes, double deltat, size_type index,
              const Propagator& prepropag, const Population& postpop );
  ~LongCoupling() override;
  void step() override;
  void output( Output& output ) const override;
};

#endif //NFTSIM_SRC_LONGCOUPLING_H
