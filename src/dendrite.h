/** @file dendrite.h
  @brief Declares the Dendrite class, which handles dendritic dynamics.

  Dendrite dynamics are applied to incoming activity to postsynaptic Population
  objects. A Dendrite turns the weighted input nu*phi from its presynaptic
  Coupling into a contribution V to the soma potential, by solving
  \f[
    \frac{1}{\alpha\beta}\frac{d^2V}{dt^2}
      + \left(\frac{1}{\alpha} + \frac{1}{\beta}\right)\frac{dV}{dt} + V = \nu\phi.
  \f]
  Dendrite (the default, `Dendrite N: alpha: ... beta: ...`) integrates this
  with classical RK4, holding nu*phi constant over each step.
  DendriteIntegral (`Dendrite N: Integral ...`, dendrite_integral.h) uses the
  exact solution for nu*phi linear over the step. DendriteRamp
  (`Dendrite N: Ramp ...`, dendriteramp.h) ramps alpha and beta over time.
  The two derived classes reuse Dendrite::init() and its state (V, dV/dt).

  The equation is linear with constant coefficients, so the sum of the V of
  several dendrites with the same alpha and beta obeys it too, driven by the
  sum of their inputs. FiringResponse uses this to merge such dendrites into
  one (see merge() and FiringResponse::mergeDendrites()).

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_DENDRITE_H
#define NFTSIM_SRC_DENDRITE_H

// Forward declaration to break circular collaboration
class Dendrite;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "nf.h"         // NF;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class Dendrite : public NF {
 protected:
  double alpha = 0.0; ///< Mean decay rate of the soma response to a delta-function synaptic input.
  double beta = 0.0; ///< Mean rise rate of the soma response to a delta-function synaptic input.
  double factorab = 0.0;   ///< == 1./alpha + 1./beta, set by init().
  double alphaxbeta = 0.0; ///< == alpha * beta, set by init().

  std::vector<double> v;    ///< Membrane potential V.
  std::vector<double> dvdt; ///< dV/dt (not used by DendriteRamp, which keeps its own in its DE).

  // Integration: SecondOrder. The input nu*phi over the step [t_n, t_n+1] is
  // extrapolated linearly from its values at t_n and t_n-1 (like a second-order
  // Adams-Bashforth step), instead of being held at its value at t_n.
  bool second_order = false; ///< if true, use the second-order scheme
  bool started = false;      ///< false until the first second-order step
  std::vector<double> prevnp; ///< nu*phi at the previous step (second-order scheme)
  void startSecondOrder( const double* np ); ///< sets prevnp on the first step

  bool steady_v = false; ///< true if V was given as `Steady`; see initFromInputs()

  // Dendrite merging (see merge()). A dendrite that has merged others is
  // driven by the sum of its own coupling's nu*phi and theirs.
  /// precouple.nuphi(), the Coupling's own vector, which lives as long as the
  /// Coupling (held here so that input() can be inline).
  const std::vector<double>* own_input;
  /// nu*phi of the couplings of the dendrites merged into this one (each a
  /// Coupling's own vector, which lives as long as the Coupling).
  std::vector<const std::vector<double>*> merged_inputs;
  std::vector<double> summed_np; ///< sum of nu*phi over precouple and merged_inputs
  /// nu*phi driving this dendrite at the current step: the coupling's own,
  /// or the sum over the merged couplings.
  inline const double* input();
  const double* sumInputs(); ///< sums the inputs of a merged dendrite into summed_np

  void init( Configf& configf ) override;
 public:
  Dendrite() = delete;                // No default constructor allowed.
  Dendrite(const Dendrite&) = delete; // No copy constructor allowed.

  const Propagator& prepropag;
  const Coupling& precouple;

  Dendrite( size_type nodes, double deltat, size_type index,
            const Propagator& prepropag, const Coupling& precouple );
  ~Dendrite() override;
  void step() override;
  /// Select the second-order scheme (`Integration: SecondOrder`). Call before init().
  void useSecondOrder( bool so ) {
    second_order = so;
  }
  inline const std::vector<double>& V() const;
  /// True if this dendrite's type is linear and time-invariant, so that it can
  /// be merged with another of the same type with the same alpha and beta.
  /// True for Dendrite and DendriteIntegral only: subclasses (eg DendriteRamp,
  /// whose rates change in time) are excluded unless they override it.
  virtual bool mergeable() const;
  /// True if `other` can be merged into this dendrite exactly (to round-off):
  /// both mergeable, the same type, alpha, beta, nodes, Deltat and scheme, and
  /// neither has been stepped yet.
  bool canMerge( const Dendrite& other ) const;
  /// Merges `other` into this dendrite, which then integrates the sum of the
  /// two V, driven by the sum of their inputs; `other` must then no longer be
  /// stepped. The merged initial V and dV/dt are the sums. Call after
  /// initFromInputs() and before the first step(), and only if canMerge().
  virtual void merge( const Dendrite& other );
  /// Called once every coupling has its initial value. For `V: Steady`, sets
  /// V to the steady state of the input the dendrite actually receives, its
  /// coupling's initial nu*phi at each node (Coupling::nuphi()).
  virtual void initFromInputs();
  void output( Output& output ) const override;
};

const std::vector<double>& Dendrite::V() const {
  return v;
}

const double* Dendrite::input() {
  return merged_inputs.empty() ? own_input->data() : sumInputs();
}

#endif
