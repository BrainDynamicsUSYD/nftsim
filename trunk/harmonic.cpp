#include<cmath>
#include "harmonic.h"

void Harmonic::init( Configf& configf )
{
  double Q = prepop.Qinit(configf);
  string buffer("Steady");
  configf.optional("phi",buffer);
  if( buffer != "Steady" )
    Q = atof(buffer.c_str());
  p.resize(nodes,Q);
  oldp.resize(nodes,Q);
  oldQ.resize(nodes,Q);
  dpdt.resize(nodes,0.);
  configf.param("Tau",tau); prepop.growHistory(tau);
  double temp; configf.optional("Deltax",temp); // for compatibility with Wave
  configf.optional("Range",temp);
  configf.param("gamma",gamma);
  twoongamma = 2./gamma;
  expgamma = exp(-gamma*deltat);
}

void Harmonic::restart( Restartf& restartf )
{
}

void Harmonic::dump( Dumpf& dumpf ) const
{
}

Harmonic::Harmonic( int nodes, double deltat, int index, Population& prepop,
        Population& postpop, int longside, string topology )
    : Propag(nodes,deltat,index,prepop,postpop,longside,topology)
{
}

Harmonic::~Harmonic(void)
{
}

void Harmonic::step(void)
{
  // This implementation assumes gamma is constant
  // and Q(t) is linear for the timestep.
  const vector<double>& Q = prepop.Q(tau);
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
