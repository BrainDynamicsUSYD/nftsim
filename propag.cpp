#include<iostream>
using std::cout;
using std::endl;
#include "propag.h"

void Propag::init( Configf& configf )
{
  double Q = prepop.Qinit(configf);
  p.resize(nodes,Q);
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
        Population& postpop, int longside, string topology )
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
  vector<Output*> temp;
  temp.push_back( new Output( label("Propag.",index+1)+".phi", p ) );
  return temp;
}
