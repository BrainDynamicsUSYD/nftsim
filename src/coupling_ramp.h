/** @file coupling_ramp.h
  @brief Declares CouplingRamp (`Coupling N: Ramp`), a coupling whose nu is piecewise linear in time.

  nu is interpolated linearly between values given at breakpoint times, and
  held constant before the first and after the last breakpoint.

  @author Paula Sanz-Leon, Romesh Abeysuriya
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
  std::vector<double> nupts; ///< Values of nu at the breakpoints tpts.
  double time = 0.0; ///< Time [s] used internally by CouplingRamp::step().
  size_type steps = 0; ///< Number of steps taken; time = steps*deltat.
  size_type nbp = 0; ///< Number of breakpoints.
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
