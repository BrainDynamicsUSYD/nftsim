/** @file coupling.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_COUPLING_H
#define NEUROFIELD_SRC_COUPLING_H

// C++ standard library headers
#include <vector> // std::vector;

// Neurofield headers
#include "configf.h"    // Configf;
#include "output.h"     // Output;
#include "nf.h"         // NF;
#include "propagator.h" // Propagator;
#include "population.h" // Population;

class Coupling : public NF {
  Coupling();
  Coupling(Coupling&);
 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

  const Propagator& prepropag;
  const Population& postpop;
  std::vector<double> n; ///< nu
  std::vector<double> P; ///< nu*phi
  int pos;
 public:
  Coupling( size_type nodes, double deltat, size_type index,
          const Propagator& prepropag, const Population& postpop );
  ~Coupling(void) override;
  void step(void) override;
  double nuinit( Configf& configf ) const;
  void output( Output& output ) const override;
  virtual const std::vector<double>& nuphi(void) const;
  inline double operator[]( size_type node ) const;
  virtual bool excite(void) const;
};

double Coupling::operator[]( size_type node ) const {
  return P[node];
}

#endif //NEUROFIELD_SRC_COUPLING_H
