#include "propag.h"

void Propag::init( Configf& configf )
{
  double Q = prepop.Qinit(configf);
  string buffer("Steady");
  configf.optional("phi",buffer);
  if( buffer != "Steady" )
    Q = atof(buffer.c_str());
  p.clear(); p.resize(nodes,Q);
  configf.optional("Tau",tau);
  prepop.growHistory(tau);
}

Propag::Propag( int nodes, double deltat, int index, Population& prepop,
        Population& postpop, int longside, string )
    : NF(nodes,deltat,index), prepop(prepop), postpop(postpop),
         tau(nodes,deltat,index), longside(longside), p(nodes)
{
}

Propag::~Propag(void)
{
}

void Propag::step(void)
{
  p = prepop.Q(tau);
}

double Propag::phiinit( Configf& configf ) const
{
  return prepop.Qinit(configf);
}

const vector<double>& Propag::phi(void) const
{
  return p;
}

void Propag::output( Output& output ) const
{
  output("Propag",index+1,"phi",p);
}
