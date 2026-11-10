/** @file population.h
  @brief Declares Population, a neural or stimulus population and its firing-rate history.

  A neural population has a FiringResponse (with its dendrites), which turns
  the soma potential V into the firing rate Q; a stimulus population has a
  Timeseries instead. Each population keeps a ring buffer of past Q long
  enough for the longest delay Tau of the propagators it drives, and
  Population::Q() returns Q delayed by a Tau (interpolated between steps for
  fractional delays with `Integration: SecondOrder`).

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_POPULATION_H
#define NFTSIM_SRC_POPULATION_H

// Forward declaration to break circular collaboration
class Population;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "nf.h"         // NF;
#include "output.h"     // Output;
#include "tau.h"        // Tau; // Must be included before propagator.h
#include "propagator.h" // Propagator;
#include "firing_response.h"  // FiringResponse;
#include "timeseries.h" // Timeseries;

// C++ standard library headers
#include <memory> // std::unique_ptr;
#include <vector> // std::vector;

class Population : public NF {
  std::unique_ptr<FiringResponse> firing_response; ///< firing response of a neural population (null for a stimulus)
  std::unique_ptr<Timeseries> timeseries; ///< stimulus of a stimulus population (null for a neural one)
 protected:
  std::vector< std::vector<double> >::size_type qkey; ///< index to the present q in qhistory
  std::vector< std::vector<double> > qhistory; ///< keyring of Q
  std::vector<double> q; ///< current Q, only for output purpose
  bool settled;  ///< if true, forbids add2Dendrite and growHistory
  double length = 0.0; ///< spatial length
  double qinit = 0.0;  ///< initial firing rate

  void init( Configf& configf ) override;
  const std::vector<double>& Qinterpolated( const Tau& tau ) const;

 public:
  Population() = delete;                   // No default constructor allowed.
  Population(const Population& ) = delete; // No copy constructor allowed.

  Population( size_type nodes, double deltat, size_type index );
  ~Population() override;
  void step() override;
  virtual const std::vector<double>& Q( const Tau& tau ) const;
  double Qinit( Configf& configf ) const;
  virtual const std::vector<double>& V() const;
  const std::vector<double>& glu() const;
  inline double sheetlength() const {
    return length;
  }
  virtual void add2Dendrite( size_type index, const Propagator& prepropag,
                             const Coupling& precouple, Configf& configf );
  virtual void growHistory( const Tau& tau );
  /// Select the second-order scheme (`Integration: SecondOrder`) for this
  /// population's dendrites. Call after add2Dendrite() and before init().
  void useSecondOrder( bool second_order );
  void output( Output& output ) const override;
  virtual void outputDendrite( size_type index, Output& output ) const;
};

#endif //NFTSIM_SRC_POPULATION_H
