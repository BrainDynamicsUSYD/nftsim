/** @file dendrite.h
  @brief Declares the Dendrite class, which handles dendritic dynamics.

  Dendrite dynamics are applied to incoming activity to postsynaptic Population
  objects.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_DENDRITE_H
#define NFTSIM_SRC_DENDRITE_H

// Forward declaration to break circular collaboration
class Dendrite;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "de.h"         // DE; RK4;
#include "nf.h"         // NF;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class Dendrite : public NF {
  // variables that are initialized once to speed up computation
  double factorab = 0.0;   ///< == 1./alpha + 1./beta;
  double alphaxbeta = 0.0; ///< == alpha * beta;

 protected:
  struct DendriteDE : public DE {
    double factorab = 0.0, alphaxbeta = 0.0;
    virtual void init(double vinit);
    DendriteDE( size_type nodes, double deltat) : DE(nodes, deltat, 3) {}
    ~DendriteDE() override = default;
    void rhs( const std::vector<double>& y, std::vector<double>& dydt, size_type /*unused*/ ) override;
  };
  DendriteDE* de;
  RK4* rk4;

  double alpha = 0.0; ///< Mean decay rate of the soma response to a delta-function synaptic input (needed here for DendriteRamp).
  double beta = 0.0; ///< Mean rise rate of the soma response to a delta-function synaptic input.

  std::vector<double> v; ///< Membrane potential.

  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;
 public:
  Dendrite() = delete;                // No default constructor allowed.
  Dendrite(const Dendrite&) = delete; // No copy constructor allowed.

  const Propagator& prepropag;
  const Coupling& precouple;

  Dendrite( size_type nodes, double deltat, size_type index,
            const Propagator& prepropag, const Coupling& precouple );
  ~Dendrite() override;
  void step() override;
  inline const std::vector<double>& V() const;
  void output( Output& output ) const override;
};

const std::vector<double>& Dendrite::V() const {
  return v;
}

#endif
