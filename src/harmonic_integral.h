/** @file harmonic_integral.h
  @brief Declares HarmonicIntegral, a Harmonic propagator stepped with an exact integral.

  HarmonicIntegral (`Propagator N: HarmonicIntegral`) solves the same damped
  oscillator as Harmonic (see harmonic.h), for spatially homogeneous models,
  but steps it with the exact solution for a delayed Q that varies linearly
  over each step, rather than with RK4. Use Harmonic if gamma changes in time
  or Q is far from linear over a step.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_HARMONIC_INTEGRAL_H
#define NFTSIM_SRC_HARMONIC_INTEGRAL_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <string> // std::string;
#include <vector> // std::vector;

class HarmonicIntegral : public Propagator {
 protected:
  void init( Configf& configf ) override;

  std::vector<double> oldp;
  std::vector<double> oldQ;
  std::vector<double> dpdt;

  // variables that initializes once
  double twoongamma;
  double expgamma;
 public:
  HarmonicIntegral() = delete;                        // No default constructor allowed.
  HarmonicIntegral(const HarmonicIntegral&) = delete; // No copy constructor allowed.

  HarmonicIntegral( size_type nodes, double deltat, size_type index, Population& prepop,
            Population& postpop, size_type longside, std::string topology );
  ~HarmonicIntegral() override;
  void step() override;
  bool nodeParallel() const override; ///< true for this class only (not derived classes); see parallel.h
};

#endif //NFTSIM_SRC_HARMONIC_INTEGRAL_H
