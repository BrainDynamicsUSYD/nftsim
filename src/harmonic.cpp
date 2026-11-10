/** @file harmonic.cpp
  @brief Definition of the Harmonic Propagator class.

  The Harmonic Propgator is used for spatially homogeneous models, where the
  Laplacian Operator term is zero and one finds a damped oscillator response.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "harmonic.h"   // Harmonic;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "population.h" // Population;

// C++ standard library headers
#include <iostream>  // std::cerr; std::endl;
#include <string>    // std::string;
#include <vector>    // std::vector;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

/**
  @brief Computes the derivatives of the Harmonic Propagator function.

  The Harmonic propagator equation is given by:
  \f{eqnarray*}{
    \frac{d^2\phi}{dt^2}&=& \gamma^2 \left(Q - \frac{2}{\gamma} \frac{d\phi}{dt} - \phi \right)
  \f}
*/
void Harmonic::HarmonicDE::rhs( const vector<double>& y, vector<double>& dydt, size_type n ) {
  // y = {phi,dphi/dt,Q}
  // dydt = {dphi/dt, d^2phi/dt^2,dQ/dt}
  dydt[0] = y[1];
  dydt[1] = gammasquared * (y[2] - (twoongamma * y[1]) - y[0]);
  // Q is constant over the step, or linear with Integration: SecondOrder.
  dydt[2] = dQdt.empty() ? 0.0 : dQdt[n];
}

void Harmonic::init( Configf& configf ) {
  Propagator::init(configf);

  // Propagator::init considers gamma optional but Harmonic requires gamma, so
  // either it or velocity must be set in the .conf file (gamma=velocity/range).
  if(gamma == 0.0) {
    cerr << "Harmonic requires gamma or velocity to be provided." << endl;
    exit(EXIT_FAILURE);
  }

  // The firing-rate history starts at the presynaptic population's initial Q.
  if( second_order ) {
    oldQ.assign(nodes, Q);
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
                    Population& postpop, size_type longside, string topology )
  : Propagator(nodes,deltat,index,prepop,postpop,longside,topology) {
  de = new HarmonicDE(nodes, deltat);
  rk4 = new RK4(*de);
}

Harmonic::~Harmonic() {
  delete de;
  delete rk4;
}

void Harmonic::step() {
  if( second_order ) {
    // The delayed Q at the new time, t_n+1, is already known (populations step
    // before propagators), so take Q as linear between its values at t_n and
    // t_n+1 rather than holding it at the t_n+1 value over the whole step.
    // RK4 integrates the linear Q exactly (dQ/dt is constant in rhs), leaving
    // the step second order overall, like HarmonicIntegral.
    const vector<double>& Qnew = prepop.Q(tau);
    de->dQdt.resize(nodes);
    for( size_type i=0; i<nodes; i++ ) {
      (*de)[2][i] = oldQ[i];
      de->dQdt[i] = (Qnew[i] - oldQ[i])/deltat;
      oldQ[i] = Qnew[i];
    }
    rk4->step();
    for( size_type i=0; i<nodes; i++ ) {
      p[i] = (*de)[0][i]; // phi
    }
    return;
  }
  const vector<double>& Q = prepop.Q(tau);
  //Copy the Q to its corresponding variable in the DE.
  for( size_type i=0; i<nodes; i++ ) {
    (*de)[2][i] = Q[i]; // Q
  }
  //Integrate the Harmonic Propagator one step forward in time.
  rk4->step();
  //Copy the phi from the updated DE to the local variable p.
  for( size_type i=0; i<nodes; i++ ) {
    p[i] = (*de)[0][i]; // phi
  }
}
