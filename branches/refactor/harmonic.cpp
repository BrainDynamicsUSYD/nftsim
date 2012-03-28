#include<cmath>
#include "harmonic.h"

void Harmonic::init( Configf& configf )
{
  string buffer("Steady");
  configf.optional("phi",buffer);
  if( buffer != "Steady" )
    p.resize(nodes,atof(buffer.c_str()));
  else
    p.resize(nodes,prepop.Qinit(configf));
  oldp.resize(nodes,atof(buffer.c_str()));
  oldQ.resize(nodes,atof(buffer.c_str()));
  dpdt.resize(nodes,0.);
  double temp; configf.optional("Deltax",temp); // for compatibility with Wave
  configf.param("Tau",tau); prepop.growHistory(tau);
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
        Population& postpop, int longside )
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
