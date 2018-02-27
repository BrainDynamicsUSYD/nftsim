/** @file harmonic_integral.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "harmonic_integral.h"   // HarmonicIntegral;

// Other nftsim headers
#include "configf.h"    // Configf;
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

  // If "Steady" then phi was set to Q in Propagator::init, so the
  // following assignment does nothing. But if not "Steady" then
  // for HarmonicIntegral we still want phi=Q.
  Q = p[0];

  oldp.resize(nodes,Q);
  oldQ.resize(nodes,Q);
  dpdt.resize(nodes,0.);

  twoongamma = 2./gamma;
  expgamma = exp(-gamma*deltat);
}

HarmonicIntegral::HarmonicIntegral( size_type nodes, double deltat, size_type index,
                    Population& prepop, Population& postpop, size_type longside, string topology )
  : Propagator(nodes,deltat,index,prepop,postpop,longside,topology) {
}

HarmonicIntegral::~HarmonicIntegral() = default;

/**
  @brief Uses an integral approximation to advance the Harmonic Propogator.
  
  This implementation assumes gamma is constant and Q(t) is linear for the 
  timestep. If you are working in a regime that does not conform to these
  assumptions then use the Harmonic class, which uses a more general
  differential form for the step method.

  x(t)  =  C1 t e(pt) + C2 e(pt)
  
  TODO: add equations and reference for the integral approximation used here.
  \f{eqnarray*}{
    \frac{dp}{dt}&=&\left(C1 - \gamma \left(C2\right)\right) \\
    \phi&=&something
  \f}
  where
  \f{eqnarray*}{
    C1&=& stuff \\
    C2&=& stuff
  \f}
*/
void HarmonicIntegral::step() {
  const vector<double>& Q = prepop.Q(tau);
  for( size_type i=0; i<nodes; i++ ) {
    dQdt = ( Q[i] -oldQ[i] )/deltat;
    adjustedQ = oldQ[i] -twoongamma*dQdt -oldp[i];
    C1 = dpdt[i] -gamma*adjustedQ -dQdt;
    C1dtplusC2 = C1*deltat -adjustedQ;
    p[i] = C1dtplusC2*expgamma +Q[i] -twoongamma*dQdt;
    dpdt[i] = (C1-gamma*C1dtplusC2)*expgamma +dQdt;
    oldQ[i] = Q[i];
    oldp[i] = p[i];
  }
}
