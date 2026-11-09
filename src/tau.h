/** @file tau.h
  @brief Declaration of Tau, which handles the activity history.

  Tau objects are used to retrieve the appropriate delayed activity when the
  discrete time delay \f$\tau_{ab}\f$ is nonzero.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_TAU_H
#define NFTSIM_SRC_TAU_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "nf.h"         // NF;

// Forward declaration to break circular collaboration
class Population;

// C++ standard library headers
#include <vector> //std::vector;

class Tau : public NF {
  size_type max; ///< if tau is inhomogeneous, == biggest element
  std::vector<size_type> m; ///< tau values across nodes, size()==1 if homogeneous
  mutable std::vector<double> delayed; ///< per-Tau buffer for Population::Q() when tau is inhomogeneous or fractional
  /// Fractional part f of tau/deltat, as tau = (m+f)*deltat with 0 <= f < 1.
  /// Only used with `Integration: SecondOrder`, where Population::Q()
  /// interpolates linearly between the stored firing rates m and m+1 steps
  /// back. Empty when every delay is a whole number of steps (always the case
  /// with the default integration, which rounds tau to the nearest step).
  std::vector<double> frac;
  bool interpolate = false; ///< if true, keep fractional delays (set by useSecondOrder())
 protected:
  void init( Configf& configf ) override;
  void initFractional( const std::vector<double>& temp );
 public:
  Tau(const Tau&) = delete;  // No copy constructor allowed.

  Tau( size_type nodes, double deltat, size_type index );
  ~Tau() override;
  void step() override;
  /// Keep the fractional part of tau/deltat (interpolating the history)
  /// instead of rounding it to a whole number of steps. Must be called before init().
  void useSecondOrder( bool second_order ) {
    interpolate = second_order;
  }

  friend class Population;
};

#endif //NFTSIM_SRC_TAU_H
