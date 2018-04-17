/**
 * @file coupling_ramp.h
 * Coupling class to produce piecewise linear segments of increasing or decreasing nu.
 * In other words it produces a temporal profile for nu.
 *
 * @brief Can use "brief" tag to explicitly generate comments for file documentation.
 *
 * @author Paula Sanz-Leon , Romesh Abeysuriya
 *
 */


#ifndef NFTSIM_SRC_COUPLINGRAMP_H
#define NFTSIM_SRC_COUPLINGRAMP_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class CouplingRamp : public Coupling {
 protected:
  std::vector<double> tpts; ///< A vector of times [s] when breakpoints occur.
  std::vector<double> deltanu; ///< A vector with slopes of segments between breakpoints.
  double time = 0.0; ///< Time [s] used internally by CouplingRamp::step().
  size_type nbp = 0; ///< Number of breakpoints.
  size_type ndnu = 0; ///< Number of segments, with an explicit deltanu.
 public:
  CouplingRamp() = delete;                    // No default constructor allowed.
  CouplingRamp(const CouplingRamp&) = delete; // No copy constructor allowed.

  void init( Configf& configf ) override;
  void step() override;
  CouplingRamp( size_type nodes, double deltat, size_type index,
              const Propagator& prepropag, const Population& postpop );
  ~CouplingRamp() override;
};

#endif // NFTSIM_SRC_COUPLINGRAMP_H
