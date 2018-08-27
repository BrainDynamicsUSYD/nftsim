/** @file coupling.h
  @brief Declaration of the main class handling synaptic connections.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_COUPLING_H
#define NFTSIM_SRC_COUPLING_H

// Forward declaration to break circular collaboration
class Coupling;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "nf.h"         // NF;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class Coupling : public NF {
 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

  const Propagator& prepropag;
  const Population& postpop;
  std::vector<double> nu; ///< Synaptic coupling strength (\f$\nu\f$).
  std::vector<double> P;  ///< Presynaptic inputs weighted by synaptic coupling strength (\f$\nu\phi\f$).
  int pos = 0;
 public:
  Coupling() = delete;                ///< No default constructor allowed.
  Coupling(const Coupling&) = delete; ///< No copy constructor allowed.

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

#endif //NFTSIM_SRC_COUPLING_H
