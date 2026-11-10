/** @file dendrite_integral.h
  @brief Declares DendriteIntegral (`Dendrite N: Integral`), a dendrite stepped with an exact integral.

  DendriteIntegral solves the same equation as Dendrite (see dendrite.h) but
  steps it with the exact solution for nu*phi that varies linearly over each
  step (from its value at the previous step to its current value), rather
  than with RK4 and nu*phi held constant. It uses Dendrite::init() and the
  same state (V, dV/dt). Its result differs from Dendrite's at O(deltat).

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

 protected:
  std::vector<double> oldnp; ///< Pulse density (nu*phi) of the previous time-step.

  void init( Configf& configf ) override;
  void stepSecondOrder(); ///< step() for Integration: SecondOrder

 public:
  DendriteIntegral() = delete;                        // No default constructor allowed.
  DendriteIntegral(const DendriteIntegral&) = delete; // No copy constructor allowed.

  DendriteIntegral( size_type nodes, double deltat, size_type index,
            const Propagator& prepropag, const Coupling& precouple );
  ~DendriteIntegral() override;
  void step() override;
  bool nodeParallel() const override; ///< true for this class only (not derived classes); see parallel.h
  void initFromInputs() override;
};

#endif //NFTSIM_SRC_DENDRITE_INTEGRAL_H
