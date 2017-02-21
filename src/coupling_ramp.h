/**
 * @file coupling_ramp.h
 * Coupling class to produce piecewise linear segments to vary synaptic couplings nu.
 * In other words it produces a temporal profile for nu.
 *
 * @brief Can use "brief" tag to explicitly generate comments for file documentation.
 *
 * @author Paula Sanz-Leon , Romesh Abeysuriya
 *
 */

/**
   Define properties of CouplingRamp

   + param[in]     ns, tpts: vector with the values of nus at specific time points specified in vector tpts.
   + param[in]     pairs   : total number of pairs of (nu, time) to define the segments
*/

#ifndef NEUROFIELD_SRC_COUPLINGRAMP_H
#define NEUROFIELD_SRC_COUPLINGRAMP_H

// Other neurofield headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class CouplingRamp : public Coupling {
  CouplingRamp();
  CouplingRamp(CouplingRamp&);
 protected:
  std::vector<double> ns;
  std::vector<double> tpts;
  std::vector<double> deltanu;
  double time = 0.0;
  std::vector<double>::size_type pairs;
 public:
  void init( Configf& configf ) override;
  void step(void) override;
  CouplingRamp( size_type nodes, double deltat, size_type index,
              const Propagator& prepropag, const Population& postpop );
  ~CouplingRamp(void) override;
};

#endif // NEUROFIELD_SRC_COUPLINGRAMP_H
