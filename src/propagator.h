/** @file propagator.h
  @brief The base Propagator class definition.

  Propagators are used to compute the axonal propagation of instantaneous
  firing rate.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_PROPAGATOR_H
#define NFTSIM_SRC_PROPAGATOR_H

// Forward declaration to break circular collaboration
class Propagator;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "nf.h"         // NF;
#include "output.h"     // Output; Outlet;
#include "population.h" // Population;
#include "tau.h"        // Tau;

// C++ standard library headers
#include <string> //std::string;
#include <vector> //std::vector;

class Propagator : public NF {
 protected:
  void init( Configf& configf ) override;
  //void restart( Restartf& restartf );
  //void dump( Dumpf& dumpf ) const;

  Population& prepop;
  Population& postpop;
  Tau tau; ///< tau_ab
  size_type longside;

  double Q;
  std::vector<double> p; ///< phi_ab
  double range = 0.0;     ///< Characteristic axonal range
  double velocity = 0.0;  ///< Mean axonal conduction Speed
  double gamma = 0.0;     ///< Damping coefficient

 public:
  Propagator() = delete;                  // No default constructor allowed.
  Propagator(const Propagator&) = delete; // No copy constructor allowed.

  Propagator( size_type nodes, double deltat, size_type index, Population& prepop,
              Population& postpop, size_type longside, std::string topology );
  ~Propagator() override;
  void step() override;
  double phiinit( Configf& configf ) const;
  virtual const std::vector<double>& phi() const;
  inline double operator[]( size_type node ) const;
  void output( Output& output ) const override;
};

double Propagator::operator[]( size_type node ) const {
  return p[node];
}

#endif //NFTSIM_SRC_PROPAGATOR_H
