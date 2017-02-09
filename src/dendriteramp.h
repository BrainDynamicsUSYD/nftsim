/** @file dendriteramp.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_DENDRITERAMP_H
#define NEUROFIELD_SRC_DENDRITERAMP_H

#include "dendrite.h"

using std::vector;

class DendriteRamp : public Dendrite {
  DendriteRamp(void);  // default constructor
  DendriteRamp(DendriteRamp& );  // no copy constructor

 protected:
  struct DendriteDE : public DE {
    vector<double> alpha_vec, beta_vec;
    double alpha2, beta2;
    double t1, t2, t3, t4;
    virtual void init( const double vinit);
    DendriteDE( size_type nodes, double deltat) : DE(nodes, deltat, 3) {}
    ~DendriteDE(void) override = default;
    void rhs( const vector<double>& y, vector<double>& dydt ) override;
  };
  DendriteDE* de;
  RK4* rk4;

  double time;

  void init( Configf& configf ) override;
 public:
  DendriteRamp( size_type nodes, double deltat, size_type index,
                const Propagator& prepropag, const Coupling& precouple );
  ~DendriteRamp(void) override;
  void step(void) override;

  inline const vector<double>& V(void) const {
    return (*de)[1]; //NOTE: Pretty sure this is an error: index=1 points to dv/dt, to get voltage, which is what I think is wanted here, it should be index=0
  }
  void output( Output& output ) const override;
};

#endif //NEUROFIELD_SRC_DENDRITERAMP_H
