/** @file harmonic.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#include <cmath>
#include "harmonic.h"

/**
  @brief Computes the derivatives of the dendritic response function.

  The Harmonic propgator equation is given by:
  \f{eqnarray*}{
    \frac{d^2\phi}{dt^2}&=& \gamma^2 \left(Q - \frac{2}{\gamma} \frac{d\phi}{dt} - \phi \right)
  \f}
*/
void Harmonic::HarmonicDE::rhs( const vector<double>& y, vector<double>& dydt ) {
  // y = {phi,dphi/dt,Q}
  // dydt = {dphi/dt, d^2phi/dt^2,dQ/dt}
  dydt[0] = y[1];
  dydt[1] = gammasquared * (y[2] - (twoongamma * y[1]) - y[0]);
  dydt[2] = 0.0;
}

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

  // Initialize constant factors to speed up computation.
  gammasquared = gamma * gamma;
  twoongamma = 2.0 / gamma;

  de->init(p[0]); // call Harmonic::HarmonicDE::init
  de->gammasquared = gammasquared;
  de->twoongamma = twoongamma;
}

void Harmonic::HarmonicDE::init(const double pinit) {
  variables[0].clear();
  variables[1].clear();
  variables[2].clear();
  variables[0].resize(nodes, pinit);
  variables[1].resize(nodes, 0.0);
  variables[2].resize(nodes, 0.0);
}

Harmonic::Harmonic( size_type nodes, double deltat, size_type index, Population& prepop,
                    Population& postpop, int longside, string topology )
  : Propagator(nodes,deltat,index,prepop,postpop,longside,topology) {
  de = new HarmonicDE(nodes, deltat);
  rk4 = new RK4(*de);
}

Harmonic::~Harmonic() {
  delete de;
  delete rk4;
}

void Harmonic::step() {
  const vector<double>& Q = prepop.Q(tau);
  //Copy the \nu\phi into their corresponding variable in the DE.
  for( size_type i=0; i<nodes; i++ ) {
    (*de)[2][i] = Q[i]; // Q
  }
  //Integrate the harmonic propagator one step forward in time.
  rk4->step();
  //Copy the phi from the updated DE to the local variable p.
  for( size_type i=0; i<nodes; i++ ) {
    p[i] = (*de)[0][i]; // phi
  }
}
