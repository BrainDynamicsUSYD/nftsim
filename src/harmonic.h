/** @file harmonic.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_HARMONIC_H
#define NEUROFIELD_SRC_HARMONIC_H

#include "propagator.h"

using std::vector;

class Harmonic : public virtual Propagator {
  Harmonic(); // no default constructor
  Harmonic(Harmonic&); // no copy constructor
 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

  vector<double> oldp;
  vector<double> oldQ;
  vector<double> dpdt;
  double dQdt;

  // variables that initializes once
  double twoongamma;
  double expgamma;

  // variables that change every timestep
  double adjustedQ;
  double C1;
  double C1dtplusC2;
 public:
  Harmonic( size_type nodes, double deltat, size_type index, Population& prepop,
            Population& postpop, int longside, string topology );
  ~Harmonic(void) override;
  void step(void) override;
};

#endif //NEUROFIELD_SRC_HARMONIC_H
