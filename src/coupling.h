/** @file coupling.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_COUPLING_H
#define NEUROFIELD_SRC_COUPLING_H

// Forward declaration to break circular collaboration
class Coupling;

// Other neurofield headers
#include "configf.h"    // Configf;
#include "nf.h"         // NF;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class Coupling : public NF {
  Coupling();                // No default constructor allowed.
  Coupling(const Coupling&); // No copy constructor allowed.
 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

  const Propagator& prepropag;
  const Population& postpop;
  std::vector<double> nu; ///< \f$\nu\f$
  std::vector<double> P;  ///< \f$\nu\phi\f$
  int pos = 0;
 public:
  Coupling( size_type nodes, double deltat, size_type index,
          const Propagator& prepropag, const Population& postpop );
  ~Coupling() override;
  void step() override;
  double nuinit( Configf& configf ) const;
  void output( Output& output ) const override;
  virtual const std::vector<double>& nuphi() const;
  inline double operator[]( size_type node ) const;
  virtual bool excite() const;
};

double Coupling::operator[]( size_type node ) const {
  return P[node];
}

#endif //NEUROFIELD_SRC_COUPLING_H
