/** @file dendrite_integral.cpp
  @brief DendriteIntegral handles the dendritic response of the postsynaptic
         population using the integral form for the step() member-function.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#include "dendrite_integral.h"

DendriteIntegral::DendriteIntegral( size_type nodes, double deltat, size_type index,
                    const Propagator& prepropag, const Coupling& precouple )
  : Dendrite(nodes, deltat, index, prepropag, precouple) {
}

DendriteIntegral::~DendriteIntegral() = default;

void DendriteIntegral::step() {
  // assume that alpha, beta are constant and nu*phi is linear for the time step
  if(alpha!=beta) {
    for(size_type i=0; i<nodes; i++) {
      dpdt = ( precouple[i] -oldnp[i] )/deltat;
      adjustednp = oldnp[i] -factorab*dpdt -v[i];
      C1 = ( adjustednp*beta -dvdt[i] +dpdt )/aminusb;
      C1expa = C1*expa;
      C2expb = expb*(-C1-adjustednp);
      v[i] = C1expa+C2expb+precouple[i] -factorab*dpdt;
      dvdt[i] = C1expa*(-alpha) +C2expb*(-beta)+dpdt;
      oldnp[i]=precouple[i]; //Save current pulse density for next step
    }
  } else { // alpha==beta
    for(size_type i=0; i<nodes; i++) {
      dpdt = ( precouple[i] -oldnp[i] )/deltat;
      adjustednp = oldnp[i] -factorab*dpdt -v[i];
      C1 = dvdt[i] -alpha*adjustednp -dpdt;
      C1dtplusC2 = C1*deltat -adjustednp;
      v[i] = C1dtplusC2*expa +precouple[i] -factorab*dpdt;
      dvdt[i] = (C1-alpha*C1dtplusC2)*expa +dpdt;
      oldnp[i]=precouple[i]; //Save current pulse density for next step
    }
  }
}
