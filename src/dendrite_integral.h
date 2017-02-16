/** @file dendrite_integral.h
  @brief Defines the DendriteIntegral class, which handles the response of the
         postsynaptic population using the integral form for the step()
         member-function.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_DENDRITE_INTEGRAL_H
#define NEUROFIELD_SRC_DENDRITE_INTEGRAL_H

// C++ standard library headers
#include <vector> // std::vector;

// Neurofield headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "dendrite.h"   // Dendrite;
#include "propagator.h" // Propagator;

class DendriteIntegral : public Dendrite {
  DendriteIntegral(void); // default constructor
  DendriteIntegral(DendriteIntegral& ); // no copy constructor

  // variables that are intialized once to speed up computation
  double aminusb;  ///< == alpha - beta
  double expa;     ///< == exp(-alpha*deltat)
  double expb;     ///< == exp(-beta*deltat)
  double factorab; ///< == 1./alpha + 1./beta;

  // variables that are used every timestep
  double adjustednp;
  double deltaPdeltat; //NOTE: doesn't seem to be used anywhere (???maybe replaced by current dpdt???).
  double C1;
  double dpdt; // Temporal derivative of pulse density.
  double C1expa;
  double C2expb;
  double C1dtplusC2;

 protected:
  std::vector<double> dvdt; ///< Temporal derivative of the membrane potential.
  std::vector<double> oldnp; ///< Pulse density (nu*phi) of the previous time-step.

  void init( Configf& configf ) override;

 public:

  DendriteIntegral( size_type nodes, double deltat, size_type index,
            const Propagator& prepropag, const Coupling& precouple );
  ~DendriteIntegral(void) override;
  void step(void) override;
};

#endif //NEUROFIELD_SRC_DENDRITE_INTEGRAL_H
