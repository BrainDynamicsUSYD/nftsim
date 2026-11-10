/** @file steady_state.h
  @brief Declares checkSteadyState(), which warns if the initial state is not a steady state.

  nftsim starts a simulation as if it were at a steady state, from the
  initial firing rate `Q:` of each population (with `phi: Steady` and
  `V: Steady`, the defaults). With spatially uniform, constant activity every
  propagator has phi_ab = Q_b as its fixed point (Map, Harmonic and
  HarmonicIntegral exactly; the Wave and WaveLegacy schemes to
  O((gamma*Deltat)^4)), each dendrite has V_ab = nu_ab*phi_ab, and the soma
  potential is V_a = sum_b V_ab, so a neural population is at a steady state
  only if Q_a = S_a(V_a), with S_a its firing function. checkSteadyState()
  evaluates this for the objects' actual initial values, once every object
  has been initialised, and warns on stderr for each population (and each
  propagator) that is not at a steady state, since the simulation then starts
  with a transient. It only reads the objects, so it does not change the
  simulation.
*/

#ifndef NFTSIM_SRC_STEADY_STATE_H
#define NFTSIM_SRC_STEADY_STATE_H

// Other nftsim headers
#include "coupling.h"   // Coupling;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <cstddef> // std::size_t;
#include <ostream> // std::ostream;
#include <string>  // std::string;
#include <vector>  // std::vector;

namespace steady_state {

/// Relative tolerance on the mismatch between a configured and an implied
/// value. The shipped configurations that are at a steady state give Q to 6
/// or more significant figures and agree with their implied Q to 1.3e-5 or
/// better, while those that are not are off by 2% or more; a transient of
/// 0.1% of the firing rate is negligible in practice.
constexpr double tolerance = 1e-3;
/// Absolute tolerance on firing rates (s^-1), so that rates within round-off
/// of 0 are not compared relatively.
constexpr double abs_tolerance = 1e-9;

/// One connection, as Solver builds it: propagator and coupling from
/// population `pre` to population `post` (indices into the population list).
struct Connection {
  std::size_t pre;
  std::size_t post;
  const Propagator* propagator;
  const Coupling* coupling;
};

/// The check's result for one population.
struct PopulationResult {
  bool checked = false;    ///< false for stimulus and BurstResponse populations
  bool consistent = true;  ///< configured Q matches the implied Q at every node
  double configured = 0.0; ///< configured initial Q
  double implied = 0.0;    ///< Q implied by the initial inputs, at `node`
  double v = 0.0;          ///< initial soma input sum_b nu_ab*phi_ab, at `node`
  std::size_t node = 0;    ///< node of largest mismatch (0-based)
  bool uniform = true;     ///< implied Q is the same at every node
};

/// The check's result for the whole network.
struct Result {
  std::vector<PopulationResult> populations; ///< one per population
  std::vector<bool> phi_consistent; ///< one per connection: initial phi equals presynaptic Q
  /// A consistent steady state was sought (only if something warned, and the
  /// network is spatially uniform with steady-state firing functions).
  bool searched = false;
  std::string not_searched_reason; ///< why not, if something warned
  bool converged = false;       ///< Newton's method converged
  std::vector<double> suggested; ///< steady-state Q per population, if converged
};

/// True if `x` and `y` agree to the relative `tolerance` (or both are within
/// `abs_tolerance` of each other).
bool close( double x, double y );

/** @brief Checks that the initial state is a steady state, writing warnings to `warnings`.

  Call once every population, propagator and coupling has been initialised
  (and initFromInputs() has run). For each neural population, the implied
  firing rate at each node is S_a(sum_b P_ab), where P_ab = nu_ab*phi_ab is
  the coupling's initial input (Coupling::nuphi()), and it is compared with
  the configured Q. Each propagator's initial phi is compared with its
  presynaptic population's Q. If anything differs, it also seeks the steady
  state nearest the configured Q (see the implementation) and suggests it.
*/
Result checkSteadyState( const std::vector<const Population*>& pops,
                         const std::vector<Connection>& connections,
                         std::ostream& warnings );

} // namespace steady_state

#endif //NFTSIM_SRC_STEADY_STATE_H
