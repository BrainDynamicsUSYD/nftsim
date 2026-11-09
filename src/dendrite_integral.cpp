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
