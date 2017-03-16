/** @file dendrite.h
  @brief Defines the Dendrite class, which handles the response of the postsynaptic population.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_DENDRITE_H
#define NEUROFIELD_SRC_DENDRITE_H

// Forward declaration to break circular collaboration
class Dendrite;

// Other neurofield headers
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
  Dendrite();                // No default constructor allowed.
  Dendrite(const Dendrite&); // No copy constructor allowed.

  // variables that are initialized once to speed up computation
  double factorab = 0.0;   ///< == 1./alpha + 1./beta;
  double alphaxbeta = 0.0; ///< == alpha * beta;

 protected:
  struct DendriteDE : public DE {
    double factorab = 0.0, alphaxbeta = 0.0;
    virtual void init(double vinit);
    DendriteDE( size_type nodes, double deltat) : DE(nodes, deltat, 3) {}
    ~DendriteDE() override = default;
    void rhs( const std::vector<double>& y, std::vector<double>& dydt ) override;
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
