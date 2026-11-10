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

 public:
  FiringResponse(const FiringResponse& ) = delete; ///< No copy constructor allowed.
  FiringResponse() = delete;                       ///< No default constructor allowed.

  FiringResponse( size_type nodes, double deltat, size_type index );
  ~FiringResponse() override;
  void step() override;
  bool nodeParallel() const override; ///< true for this class only (not derived classes); see parallel.h
  virtual void add2Dendrite( size_type index, const Propagator& prepropag,
                             const Coupling& precouple, Configf& configf );

  /// Select the second-order scheme (`Integration: SecondOrder`) for all dendrites.
  void useSecondOrder( bool second_order );

  virtual void fire( std::vector<double>& Q ) const;
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
