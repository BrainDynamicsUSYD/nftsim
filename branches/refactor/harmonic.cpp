#include<cmath>
#include "harmonic.h"

void Harmonic::init( Configf& configf )
{
  double initphi; configf.Param("phi",initphi);
  p.resize(nodes,initphi);
  oldp.resize(nodes,initphi);
  oldQ.resize(nodes,initphi);
  dpdt.resize(nodes,0.);
  double temp; configf.Optional("Deltax",temp); // for compatibility with Wave
  configf.Param("Tau",tau);
  configf.Optional("Range",temp);
  configf.Param("gamma",gamma);
  twoongamma = 2./gamma;
  expgamma = exp(-gamma*deltat);
}

void Harmonic::restart( Restartf& restartf )
{
}

void Harmonic::dump( Dumpf& dumpf ) const
{
}

Harmonic::Harmonic( int nodes, double deltat, int index, const Population* const prepop,
        const Population* const postpop, int longside )
    : Propag(nodes,deltat,index,prepop,postpop,longside)
{
}

Harmonic::~Harmonic(void)
{
}

void Harmonic::step(void)
{
  // This implementation assumes gamma is constant
  // and Q(t) is linear for the timestep.
  const vector<double>& Q = prepop->Q(tau);
  for( int i=0; i<nodes; i++ ) {
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
