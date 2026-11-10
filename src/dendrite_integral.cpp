/** @file dendrite_integral.cpp
  @brief DendriteIntegral handles the dendritic response of the postsynaptic
         population using the integral form for the DendriteIntegral::step()
         member-function.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "dendrite_integral.h" // DendriteIntegral;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <cmath>     // std::exp;

void DendriteIntegral::init( Configf& configf ) {
  Dendrite::init(configf);

  oldnp = v;

  // Initialize constant factors to speed up computation.
  aminusb = alpha - beta;
  expa = exp(-alpha*deltat);
  expb = exp(-beta*deltat);
  factorab = 1./alpha + 1./beta;
}

DendriteIntegral::DendriteIntegral( size_type nodes, double deltat, size_type index,
                    const Propagator& prepropag, const Coupling& precouple )
  : Dendrite(nodes, deltat, index, prepropag, precouple), dvdt(nodes,0), oldnp(nodes) {
}

DendriteIntegral::~DendriteIntegral() = default;

void DendriteIntegral::step() {
  if( second_order ) {
    stepSecondOrder();
    return;
  }
  // assume that alpha, beta are constant and nu*phi is linear for the time step
  // Per-node temporaries are locals (they were class members, which forced a
  // store per node and blocked vectorisation).
  const double* __restrict__ np = precouple.nuphi().data(); // current nu*phi
  double* __restrict__ vv = v.data();
  double* __restrict__ dv = dvdt.data();
  double* __restrict__ onp = oldnp.data();
  const double invdt = 1.0/deltat;
  if(alpha!=beta) {
    const double inv_amb = 1.0/aminusb;
    for(size_type i=0; i<nodes; i++) {
      const double dpdt = ( np[i] -onp[i] )*invdt;
      const double adjustednp = onp[i] -factorab*dpdt -vv[i];
      const double C1 = ( adjustednp*beta -dv[i] +dpdt )*inv_amb;
      const double C1expa = C1*expa;
      const double C2expb = expb*(-C1-adjustednp);
      vv[i] = C1expa+C2expb+np[i] -factorab*dpdt;
      dv[i] = C1expa*(-alpha) +C2expb*(-beta)+dpdt;
      onp[i] = np[i]; //Save current pulse density for next step
    }
  } else { // alpha==beta
    for(size_type i=0; i<nodes; i++) {
      const double dpdt = ( np[i] -onp[i] )*invdt;
      const double adjustednp = onp[i] -factorab*dpdt -vv[i];
      const double C1 = dv[i] -alpha*adjustednp -dpdt;
      const double C1dtplusC2 = C1*deltat -adjustednp;
      vv[i] = C1dtplusC2*expa +np[i] -factorab*dpdt;
      dv[i] = (C1-alpha*C1dtplusC2)*expa +dpdt;
      onp[i] = np[i]; //Save current pulse density for next step
    }
  }
}

/**
  @brief The exact solution for nu*phi linear over the step, extrapolated forward.

  Used with `Integration: SecondOrder`. DendriteIntegral::step() takes nu*phi
  to rise linearly from its previous value to its current value over the
  step [t_n, t_n+1], which delays the input by one step (first order). Here
  nu*phi starts at its current value p0 = nu*phi(t_n) with the slope of the
  last step, reaching p1 = 2 nu*phi(t_n) - nu*phi(t_n-1) at t_n+1, which is
  second order. The solution formula is the same with the end points changed.
*/
void DendriteIntegral::stepSecondOrder() {
  const double* __restrict__ np = precouple.nuphi().data(); // nu*phi at t_n
  startSecondOrder(np);
  double* __restrict__ vv = v.data();
  double* __restrict__ dv = dvdt.data();
  double* __restrict__ pp = prevnp.data(); // nu*phi at t_n-1
  const double invdt = 1.0/deltat;
  if(alpha!=beta) {
    const double inv_amb = 1.0/aminusb;
    for(size_type i=0; i<nodes; i++) {
      const double p0 = np[i];
      const double dp = p0 - pp[i];
      const double p1 = p0 + dp;
      const double dpdt = dp*invdt;
      const double adjustednp = p0 -factorab*dpdt -vv[i];
      const double C1 = ( adjustednp*beta -dv[i] +dpdt )*inv_amb;
      const double C1expa = C1*expa;
      const double C2expb = expb*(-C1-adjustednp);
      vv[i] = C1expa+C2expb+p1 -factorab*dpdt;
      dv[i] = C1expa*(-alpha) +C2expb*(-beta)+dpdt;
      pp[i] = p0;
    }
  } else { // alpha==beta
    for(size_type i=0; i<nodes; i++) {
      const double p0 = np[i];
      const double dp = p0 - pp[i];
      const double p1 = p0 + dp;
      const double dpdt = dp*invdt;
      const double adjustednp = p0 -factorab*dpdt -vv[i];
      const double C1 = dv[i] -alpha*adjustednp -dpdt;
      const double C1dtplusC2 = C1*deltat -adjustednp;
      vv[i] = C1dtplusC2*expa +p1 -factorab*dpdt;
      dv[i] = (C1-alpha*C1dtplusC2)*expa +dpdt;
      pp[i] = p0;
    }
  }
}
