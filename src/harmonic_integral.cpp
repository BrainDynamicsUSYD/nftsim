/** @file harmonic_integral.cpp
  @brief Defines HarmonicIntegral, a Harmonic propagator stepped with an exact integral.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "harmonic_integral.h"   // HarmonicIntegral;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "parallel.h"   // NFTSIM_PARALLEL_FOR;
#include "population.h" // Population;

// C++ standard library headers
#include <cmath>     // std::exp;
#include <iostream>  // std::cerr; std::endl;
#include <string>    // std::string;
#include <vector>    // std::vector;
using std::cerr;
using std::endl;
using std::exp;
using std::string;
using std::vector;


void HarmonicIntegral::init( Configf& configf ) {
  Propagator::init(configf);

  // Propagator::init considers gamma optional but Harmonic requires gamma, so
  // either it or velocity must be set in the .conf file (gamma=velocity/range).
  if(gamma == 0.0) {
    cerr << "HarmonicIntegral requires gamma or velocity to be provided." << endl;
    exit(EXIT_FAILURE);
  }

  // Start at rest at the initial phi of each node: phi = p, dphi/dt = 0, and
  // the previous delayed Q taken equal to phi (with phi: Steady, that is Q).
  // With an explicit phi this is the state of a propagator that had settled
  // to a constant input Q = phi before t = 0.
  oldp = p;
  oldQ = p;
  dpdt.assign(nodes,0.);

  twoongamma = 2./gamma;
  expgamma = exp(-gamma*deltat);
}

HarmonicIntegral::HarmonicIntegral( size_type nodes, double deltat, size_type index,
                    Population& prepop, Population& postpop, size_type longside, string topology )
  : Propagator(nodes,deltat,index,prepop,postpop,longside,topology) {
}

HarmonicIntegral::~HarmonicIntegral() = default;

/**
  @brief Advances the Harmonic propagator with the exact solution for Q linear over the step.

  This implementation assumes gamma is constant and Q(t) is linear for the
  timestep. If you are working in a regime that does not conform to these
  assumptions then use the Harmonic class, which uses a more general
  differential form for the step method.

  The propagator obeys
  \f[
    \frac{1}{\gamma^2}\frac{d^2\phi}{dt^2} + \frac{2}{\gamma}\frac{d\phi}{dt} + \phi = Q.
  \f]
  Over a step, with \f$Q(t) = Q_0 + s t\f$ (\f$Q_0\f$ the previous delayed Q
  and \f$s = (Q - Q_0)/\Delta t\f$), the solution is
  \f[
    \phi(t) = (C_1 t + C_2) e^{-\gamma t} + Q_0 + s t - \frac{2}{\gamma} s,
  \f]
  with \f$C_2 = \phi_0 - Q_0 + 2s/\gamma\f$ (= -adjustedQ) and
  \f$C_1 = \dot\phi_0 + \gamma C_2 - s\f$ fitted to \f$\phi\f$ and
  \f$d\phi/dt\f$ at the start of the step. It is evaluated at
  \f$t = \Delta t\f$, with \f$e^{-\gamma\Delta t}\f$ precomputed.
*/
void HarmonicIntegral::step() {
  const vector<double>& Q = prepop.Q(tau);
  const double invdt = 1.0/deltat;
  NFTSIM_PARALLEL_FOR
  for( size_type i=0; i<nodes; i++ ) {
    const double dQdt = ( Q[i] -oldQ[i] )*invdt;
    const double adjustedQ = oldQ[i] -twoongamma*dQdt -oldp[i];
    const double C1 = dpdt[i] -gamma*adjustedQ -dQdt;
    const double C1dtplusC2 = C1*deltat -adjustedQ;
    p[i] = C1dtplusC2*expgamma +Q[i] -twoongamma*dQdt;
    dpdt[i] = (C1-gamma*C1dtplusC2)*expgamma +dQdt;
    oldQ[i] = Q[i];
    oldp[i] = p[i];
  }
}
