/** @file dendriteramp.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_DENDRITERAMP_H
#define NFTSIM_SRC_DENDRITERAMP_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "de.h"         // DE; RK4;
#include "dendrite.h"   // Dendrite;
#include "output.h"     // Output;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class DendriteRamp : public Dendrite {
 protected:
  /// State {V, dV/dt, nu*phi} and the ramped rates, integrated by RK4.
  struct RampDE : public DE {
    std::vector<double> alpha_vec, beta_vec;
    std::vector<double> dPdt; ///< d(nu*phi)/dt over the step; empty unless Integration: SecondOrder
    double alpha2 = 0.0, beta2 = 0.0;
    double t1 = 0.0, t2 = 0.0, t3 = 0.0, t4 = 0.0;
    virtual void init(const std::vector<double>& vinit);
    RampDE( size_type nodes, double deltat) : DE(nodes, deltat, 3) {}
    ~RampDE() override = default;
    void rhs( const std::vector<double>& y, std::vector<double>& dydt, size_type /*unused*/ ) override;
  };
  RampDE de; ///< DendriteRamp's own state; Dendrite::v holds a copy of V.
  RK4 rk4;   ///< Integrates de (declared after it, so constructed after it).

  double time = 0.0;
  void ramp( double t ); ///< sets alpha_vec and beta_vec at time t (default scheme)
  void rampMidstep( double t ); ///< sets alpha_vec and beta_vec at time t (second-order scheme)

  void init( Configf& configf ) override;
 public:
  DendriteRamp() = delete;                    // No default constructor allowed.
  DendriteRamp(const DendriteRamp&) = delete; // No copy constructor allowed.

  DendriteRamp( size_type nodes, double deltat, size_type index,
                const Propagator& prepropag, const Coupling& precouple );
  ~DendriteRamp() override;
  void step() override;
  void output( Output& output ) const override;
};

#endif //NFTSIM_SRC_DENDRITERAMP_H
