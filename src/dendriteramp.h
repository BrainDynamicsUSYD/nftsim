/** @file dendriteramp.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_DENDRITERAMP_H
#define NEUROFIELD_SRC_DENDRITERAMP_H

#include"dendrite.h"

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
    DendriteDE( int nodes, double deltat) : DE(nodes, deltat, 3) {}
    ~DendriteDE(void) override = default;
    void rhs( const vector<double>& y, vector<double>& dydt ) override;
  };
  DendriteDE* de;
  RK4* rk4;

  double time;


  void init( Configf& configf ) override;
 public:
  DendriteRamp( int nodes, double deltat, int index,
                const Propag& prepropag, const Couple& precouple );
  ~DendriteRamp(void) override;
  void step(void) override;

  inline const vector<double>& V(void) const {
    return (*de)[1];
  }
  void output( Output& output ) const override;
};

#endif
