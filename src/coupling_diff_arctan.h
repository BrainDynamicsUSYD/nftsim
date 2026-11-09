/** @file coupling_diff_arctan.h
  @brief A new Coupling class, where nu_ab follows a smooth function.

  The smooth variation of nu_ab is defined by the difference of
  two arctangent functions.


  @author Farah Deeba, Paula Sanz-Leon, Sahand Assadzadeh
*/

#ifndef COUPLING_DIFF_ARCTAN_H
#define COUPLING_DIFF_ARCTAN_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling
#include "propagator.h" // Propagator;
#include "population.h" // Population;

class CouplingDiffArctan : public Coupling {
 protected:
  double nu_min = 0.0, nu_max = 0.0;
  double t_half_down = 0.0, t_half_up = 0.0;
  double delta = 0.0; ///< Time interval [s] in which the function will go from 0.25 to 0.75 of nu_max.
  double time_tot;
  double diff_atan_min = 0.0,  diff_atan_max = 0.0;
  double time = 0.0;
  size_type steps = 0; ///< Number of steps taken; time = steps*deltat.
  double profile( double t ) const; ///< Unscaled difference-of-arctangents profile at time t [s].

 public:
  CouplingDiffArctan() = delete;                          // No default constructor allowed.
  CouplingDiffArctan(const CouplingDiffArctan&) = delete; // No copy constructor allowed.

  void init( Configf& configf ) override;
  void step() override;
  /// A DiffArctan coupling has no `nu:`; it starts at `nu_min:`.
  double nuinit( Configf& configf ) const override;
  CouplingDiffArctan( size_type nodes, double deltat, size_type index,
                    const Propagator& prepropag, const Population& postpop, double tempf );
  ~CouplingDiffArctan() override;
};

#endif //COUPLING_DIFF_ARCTAN_H

