/** @file dendrite_integral.cpp
  @brief DendriteIntegral handles the dendritic response of the postsynaptic
         population using the integral form for the step() member-function.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#include "dendrite_integral.h"


void DendriteIntegral::init( Configf& configf ) {
  Dendrite::init(configf);

  // NOTE: We have to set these here as the base Dendrite::init is setting
  // the private versions of these variables. If they're used by the new
  // differential form of Dendrite::step() then make them not-private so we
  // can just rely on the base class member, then clean-up. If they're not
  // used by the new Dendrite::step() then remove them from Dendrite and keep
  // them here... Either way, get rid of the redundancy.
  aminusb = alpha - beta;
  expa = exp(-alpha*deltat);
  expb = exp(-beta*deltat);
  factorab = 1./alpha + 1./beta;
}

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

