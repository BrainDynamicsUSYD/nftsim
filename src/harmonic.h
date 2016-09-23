#ifndef NEUROFIELD_SRC_HARMONIC_H
#define NEUROFIELD_SRC_HARMONIC_H

#include"propag.h"

using std::vector;

class Harmonic : public virtual Propag {
  Harmonic(); // no default constructor
  Harmonic(Harmonic&); // no copy constructor
 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

  double gamma;  // damping coefficient

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
  Harmonic( int nodes, double deltat, int index, Population& prepop,
            Population& postpop, int longside, string topology );
  ~Harmonic(void) override;
  void step(void) override;
};

#endif
