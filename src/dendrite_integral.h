/** @file dendrite_integral.h
  @brief Declares the DendriteIntegral class, which handles the response of the
         postsynaptic population using the integral form for the DendriteIntegral::step()
         member-function.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_DENDRITE_INTEGRAL_H
#define NFTSIM_SRC_DENDRITE_INTEGRAL_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "dendrite.h"   // Dendrite;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class DendriteIntegral : public Dendrite {
  // variables that are intialized once to speed up computation
  double aminusb = 0.0;  ///< == alpha - beta
  double expa = 0.0;     ///< == exp(-alpha*deltat)
  double expb = 0.0;     ///< == exp(-beta*deltat)
  double factorab = 0.0; ///< == 1./alpha + 1./beta;

  // variables that are used every timestep
  double adjustednp = 0.0;
  double C1 = 0.0;
  double dpdt = 0.0; ///< Temporal derivative of pulse density.
  double C1expa = 0.0;
  double C2expb = 0.0;
  double C1dtplusC2 = 0.0;

 protected:
  std::vector<double> dvdt; ///< Temporal derivative of the membrane potential.
  std::vector<double> oldnp; ///< Pulse density (nu*phi) of the previous time-step.

  void init( Configf& configf ) override;

 public:
  DendriteIntegral() = delete;                        // No default constructor allowed.
  DendriteIntegral(const DendriteIntegral&) = delete; // No copy constructor allowed.

  DendriteIntegral( size_type nodes, double deltat, size_type index,
            const Propagator& prepropag, const Coupling& precouple );
  ~DendriteIntegral() override;
  void step() override;
};

#endif //NFTSIM_SRC_DENDRITE_INTEGRAL_H
