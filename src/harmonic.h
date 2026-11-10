/** @file harmonic.h
  @brief Declaration of the Harmonic Propagator class.

  The Harmonic Propagator is used for spatially homogeneous models, where the
  Laplacian Operator term is zero and one finds a damped oscillator response:
  \f[
    \frac{1}{\gamma^2}\frac{d^2\phi}{dt^2} + \frac{2}{\gamma}\frac{d\phi}{dt} + \phi = Q(t - \tau).
  \f]
  Harmonic (`Propagator N: Harmonic`, and Wave or WaveLegacy on a single
  node) integrates it with RK4, holding the delayed Q constant over a step
  (or linear, with `Integration: SecondOrder`). HarmonicIntegral
  (harmonic_integral.h) uses the exact solution for Q linear over the step
  instead. The two share only the Propagator base: their state and step
  differ entirely.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_HARMONIC_H
#define NFTSIM_SRC_HARMONIC_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "de.h"         // DE; RK4;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <string> // std::string;
#include <vector> // std::vector;

class Harmonic : public Propagator {
  // variables that are initialized once to speed up computation
  double gammasquared = 0.0; ///< == gamma^2;
  double twoongamma = 0.0;   ///< == 2.0/gamma;

  struct HarmonicDE : public DE {
    double gammasquared = 0.0, twoongamma = 0.0;
    std::vector<double> dQdt; ///< dQ/dt over the step; empty unless Integration: SecondOrder
    virtual void init(double pinit);
    HarmonicDE( size_type nodes, double deltat) : DE(nodes, deltat, 3) {}
    ~HarmonicDE() override = default;
    void rhs( const std::vector<double>& y, std::vector<double>& dydt, size_type /*unused*/ ) override;
  };
  HarmonicDE de; ///< State {phi, dphi/dt, Q}.
  RK4 rk4;       ///< Integrates de (declared after it, so constructed after it).
  std::vector<double> oldQ; ///< delayed Q at the previous step (Integration: SecondOrder)

 protected:
  void init( Configf& configf ) override;

 public:
  Harmonic() = delete;                // No default constructor allowed.
  Harmonic(const Harmonic&) = delete; // No copy constructor allowed.

  Harmonic( size_type nodes, double deltat, size_type index, Population& prepop,
            Population& postpop, size_type longside, std::string topology );
  ~Harmonic() override;
  void step() override;
};

#endif //NFTSIM_SRC_HARMONIC_H
