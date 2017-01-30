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

  // variables that are intialized once to speed up computation
  double aminusb;  ///< == alpha - beta
  double expa;     ///< == exp(-alpha*deltat)
  double expb;     ///< == exp(-beta*deltat)
  double factorab; ///< == 1./alpha + 1./beta;

  // variables that are used every timestep
  double adjustednp;
  double deltaPdeltat; //NOTE: doesn't seem to be used anywhere (???maybe replaced by current dpdt???).
  double C1;
  double dpdt; // Temporal derivative of pulse density.
  double C1expa;
  double C2expb;
  double C1dtplusC2;
 protected:

  double alpha; ///< Mean decay rate of the soma response to a delta-function synaptic input (needed here for DendriteRamp).
  double beta; ///< Mean rise rate of the soma response to a delta-function synaptic input.

  vector<double> v; ///< Membrane potential.
  vector<double> dvdt; ///< Temporal derivative of the membrane potential.
  //vector<double> np; ///< Pulse density (nu*phi).
  vector<double> oldnp; ///< Pulse density (nu*phi) of the previous time-step.

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
