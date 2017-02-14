/** @file harmonic_integral.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_HARMONIC_INTEGRAL_H
#define NEUROFIELD_SRC_HARMONIC_INTEGRAL_H

#include "propagator.h"

using std::vector;

class HarmonicIntegral : public virtual Propagator {
  HarmonicIntegral(); // no default constructor
  HarmonicIntegral(HarmonicIntegral&); // no copy constructor
 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

  double gamma;  ///< damping coefficient

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
  HarmonicIntegral( size_type nodes, double deltat, size_type index, Population& prepop,
            Population& postpop, int longside, string topology );
  ~HarmonicIntegral(void) override;
  void step(void) override;
};

#endif //NEUROFIELD_SRC_HARMONIC_INTEGRAL_H
