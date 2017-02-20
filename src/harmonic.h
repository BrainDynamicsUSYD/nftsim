/** @file harmonic.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_HARMONIC_H
#define NEUROFIELD_SRC_HARMONIC_H

#include "propagator.h"
#include "de.h"

using std::vector;

class Harmonic : public virtual Propagator {
  Harmonic(); // no default constructor
  Harmonic(Harmonic&); // no copy constructor

  // variables that are initialized once to speed up computation
  double gammasquared = 0.0; ///< == gamma^2;
  double twoongamma = 0.0;   ///< == 2.0/gamma;

  struct HarmonicDE : public DE {
    double gammasquared = 0.0, twoongamma = 0.0;
    virtual void init( const double pinit);
    HarmonicDE( size_type nodes, double deltat) : DE(nodes, deltat, 3) {}
    ~HarmonicDE(void) override = default;
    void rhs( const vector<double>& y, vector<double>& dydt ) override;
  };
  HarmonicDE* de;
  RK4* rk4;

 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

 public:
  Harmonic( size_type nodes, double deltat, size_type index, Population& prepop,
            Population& postpop, int longside, string topology );
  ~Harmonic(void) override;
  void step(void) override;
};

#endif //NEUROFIELD_SRC_HARMONIC_H
