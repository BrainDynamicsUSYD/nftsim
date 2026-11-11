/** @file firing_response.h
  @brief Declares the FiringResponse class, for the soma response of neural populations.

  Each neural population is associated with a FiringResponse object, which produces
  the soma response.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_FIRING_RESPONSE_H
#define NFTSIM_SRC_FIRING_RESPONSE_H

// Forward declaration to break circular collaboration
class FiringResponse;

// Other nftsim headers
#include "array.h"      // Array;
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "dendrite.h"   // Dendrite;
#include "nf.h"         // NF;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <string> // std::string;
#include <vector> // std::vector;

class FiringResponse : public NF {
 protected:
  void init( Configf& configf ) override;

  /// Firing-rate function, parsed from `Function:` by init() so that fire()
  /// need not compare strings at every step.
  enum class Function { None, Sigmoid, Linear, Quadratic, Cubic };
  Function function = Function::None; ///< None until init() (BurstResponse never sets it).
  double theta = 0.0; ///< Mean firing threshold.
  double sigma = 0.0; ///< Standard deviation of the firing threshold.
  double Q_max = 0.0; ///< Maximum firing rate.
  double a = 0.0, b = 0.0, c = 0.0, d = 0.0;

  Array<Dendrite> dendrites; ///< array of dendrites
  std::vector<Array<Dendrite>::size_type> dendrite_index; ///< indices of dendrites
  std::vector<double> v; ///< soma potential for the population

  // Dendrite merging: dendrites of the same linear type with the same alpha
  // and beta are integrated as one, driven by the sum of their inputs (their
  // V sum exactly, up to round-off). See mergeDendrites().
  bool merge_dendrites; ///< merging allowed (default from NFTSIM_MERGE_DENDRITES)
  bool merge_done = false; ///< true once mergeDendrites() has run
  std::vector<Dendrite*> stepped; ///< dendrites that are stepped and summed into v
  /// Per dendrite: true if its own V is written to the output. Set by
  /// outputDendrite() (const, as it is reached through the const output
  /// interface), and such dendrites are never merged.
  mutable std::vector<bool> dendrite_output;
  /// Groups the dendrites that can be merged and merges each group into its
  /// first member. Runs at the first step(), after Solver has set up the
  /// outputs, so that dendrites whose own V is output can be left out.
  void mergeDendrites();

 public:
  FiringResponse(const FiringResponse& ) = delete; ///< No copy constructor allowed.
  FiringResponse() = delete;                       ///< No default constructor allowed.

  FiringResponse( size_type nodes, double deltat, size_type index );
  ~FiringResponse() override;
  void step() override;
  virtual void add2Dendrite( size_type index, const Propagator& prepropag,
                             const Coupling& precouple, Configf& configf );

  /// Select the second-order scheme (`Integration: SecondOrder`) for all dendrites.
  void useSecondOrder( bool second_order );
  /// Allow or forbid merging dendrites. Call before the first step(). The
  /// default is to merge, unless the environment variable
  /// NFTSIM_MERGE_DENDRITES is 0, off, no or false.
  void allowMerging( bool allow ) {
    merge_dendrites = allow;
  }
  /// Number of dendrite integrations per step: the number of dendrites less
  /// those merged into another (all of them before the first step()).
  Array<Dendrite>::size_type steppedDendrites() const;

  virtual void fire( std::vector<double>& Q ) const;
  /// Firing rate q = S(v) and its slope dq/dv for a constant soma potential v,
  /// as fire() computes them, for the start-up steady-state check
  /// (steady_state.h). Returns false if the steady-state firing rate is not a
  /// function of v alone: BurstResponse, whose threshold follows its own slow
  /// variables Xtilde and Htilde, never sets `function`, so it returns false.
  /// (GlutamateResponse fires as FiringResponse does: its glutamate only
  /// drives plastic couplings.)
  bool steadyFiring( double v, double& q, double& dqdv ) const;
  /// Called once every population, propagator and coupling has been
  /// initialised, for state whose initial value depends on other objects:
  /// each dendrite's Steady V (Dendrite::initFromInputs), and, in
  /// GlutamateResponse, glutamate's steady state. Overrides must call this.
  virtual void initFromInputs();
  inline const std::vector<double>& V() const;
  void output( Output& output ) const override;
  virtual void outputDendrite( size_type index, Output& output ) const;
};

const std::vector<double>& FiringResponse::V() const {
  return v;
}

#endif //NFTSIM_SRC_FIRING_RESPONSE_H
