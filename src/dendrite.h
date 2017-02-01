/** @file dendrite.h
  @brief Defines the Dendrite class, which handles the response of the postsynaptic population.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_DENDRITE_H
#define NEUROFIELD_SRC_DENDRITE_H

#include "population.h"

using std::vector;

class Dendrite : public NF {
  Dendrite(void); // default constructor
  Dendrite(Dendrite& ); // no copy constructor

 protected:
  struct DendriteDE : public DE {
    double alpha, beta;
    virtual void init( const double vinit);
    DendriteDE( size_type nodes, double deltat) : DE(nodes, deltat, 3) {}
    ~DendriteDE(void) override = default;
    void rhs( const vector<double>& y, vector<double>& dydt ) override;
  };
  DendriteDE* de;
  RK4* rk4;

  double alpha; ///< Mean decay rate of the soma response to a delta-function synaptic input (needed here for DendriteRamp).
  double beta; ///< Mean rise rate of the soma response to a delta-function synaptic input.

  vector<double> v; ///< Membrane potential.

  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;
 public:
  const Propagator& prepropag;
  const Coupling& precouple;

  Dendrite( size_type nodes, double deltat, size_type index,
            const Propagator& prepropag, const Coupling& precouple );
  ~Dendrite(void) override;
  void step(void) override;
  inline const vector<double>& V(void) const;
  void output( Output& output ) const override;
};

const vector<double>& Dendrite::V(void) const {
  return v;
}

#endif
