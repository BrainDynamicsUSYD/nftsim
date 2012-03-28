#include "propag.h"

void Propag::init( Configf& configf )
{
  p.resize(nodes);
  configf.param("Tau",tau);
  prepop.growHistory(tau);
}

void Propag::restart( Restartf& restartf )
{
}

void Propag::dump( Dumpf& dumpf ) const
{
}

Propag::Propag( int nodes, double deltat, int index, Population& prepop,
        Population& postpop, int longside )
    : NF(nodes,deltat,index), prepop(prepop), postpop(postpop),
         tau(nodes,deltat,index), longside(longside)
{
}

Propag::~Propag(void)
{
}

void Propag::step(void)
{
  p = prepop.Q(tau);
}

const vector<double>& Propag::phi(void) const
{
  return p;
}

vector<Output*> Propag::output(void) const
{
  Output* temp = new Output( label("Propag.",index+1)+".phi", p );
  return vector<Output*>(1,temp);
}
