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
  double deltaPdeltat;
  double C1;
  double dpdt;
  double C1expa;
  double C2expb;
  double C1dtplusC2;
 protected:

  double alpha; // needed here for DendriteRamp
  double beta;

  vector<double> v;
  vector<double> dvdt;
  //vector<double> np;
  vector<double> oldnp;

  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;
 public:
  const Propag& prepropag;
  const Couple& precouple;

  Dendrite( int nodes, double deltat, int index,
            const Propag& prepropag, const Couple& precouple );
  ~Dendrite(void) override;
  void step(void) override;
  inline const vector<double>& V(void) const;
  void output( Output& output ) const override;
};

const vector<double>& Dendrite::V(void) const {
  return v;
}

#endif
