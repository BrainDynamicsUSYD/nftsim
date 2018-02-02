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
using std::vector;

class DendriteRamp : public Dendrite {
 protected:
  struct DendriteDE : public DE {
    std::vector<double> alpha_vec, beta_vec;
    double alpha2 = 0.0, beta2 = 0.0;
    double t1 = 0.0, t2 = 0.0, t3 = 0.0, t4 = 0.0;
    virtual void init(double vinit);
    DendriteDE( size_type nodes, double deltat) : DE(nodes, deltat, 3) {}
    ~DendriteDE() override = default;
    void rhs( const std::vector<double>& y, std::vector<double>& dydt, size_type /*unused*/ ) override;
  };
  DendriteDE* de; //Must redeclare here otherwise we get Dendrite::DendriteDE.
  RK4* rk4;       //Must redeclare here otherwise we get seg-fault

  double time = 0.0;

  void init( Configf& configf ) override;
 public:
  DendriteRamp() = delete;                    // No default constructor allowed.
  DendriteRamp(const DendriteRamp&) = delete; // No copy constructor allowed.

  DendriteRamp( size_type nodes, double deltat, size_type index,
                const Propagator& prepropag, const Coupling& precouple );
  ~DendriteRamp() override;
  void step() override;

  inline const std::vector<double>& V() const {
    return (*de)[1]; //NOTE: Pretty sure this is an error: index=1 points to dv/dt, to get voltage, which is what I think is wanted here, it should be index=0
  }
  void output( Output& output ) const override;
};

#endif //NFTSIM_SRC_DENDRITERAMP_H
