/** @file harmonic.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#include <cmath>
#include "harmonic.h"

void Harmonic::init( Configf& configf ) {
  Propagator::init(configf);

  // Propagator::init considers gamma optional but Harmonic requires gamma, so
  // either it or velocity must be set in the .conf file (gamma=velocity/range).
  if(gamma == 0.0) {
    cerr << "Harmonic requires gamma or velocity to be provided." << endl;
    exit(EXIT_FAILURE);
  }

  // If "Steady" then phi was set to Q in Propagator::init, so the
  // following assignment does nothing. But if not "Steady" then
  // for Harmonic we still want phi=Q. ?True for integral approx but maybe not general?
  Q = p[0];

  oldp.resize(nodes,Q);
  oldQ.resize(nodes,Q);
  dpdt.resize(nodes,0.);

  twoongamma = 2./gamma;
  expgamma = exp(-gamma*deltat);
}

Harmonic::Harmonic( size_type nodes, double deltat, size_type index, Population& prepop,
                    Population& postpop, int longside, string topology )
  : Propagator(nodes,deltat,index,prepop,postpop,longside,topology) {
}

Harmonic::~Harmonic() = default;

void Harmonic::step() {
  // This implementation assumes gamma is constant
  // and Q(t) is linear for the timestep.
  // x(t)  =  C1 t e(pt) + C2 e(pt),
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
