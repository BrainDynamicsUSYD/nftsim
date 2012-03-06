/***************************************************************************
                          propag.cpp  -  description
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "propag.h"

void Propag::init( Configf& configf )
{
  p.resize(nodes);
  configf.Param("Tau",tau);
}

void Propag::restart( Restartf& restartf )
{
}

void Propag::dump( Dumpf& dumpf ) const
{
}

Propag::Propag( int nodes, double deltat, int index, const Population* const prepop,
        const Population* const postpop, int longside )
    : NF(nodes,deltat,index), prepop(prepop), postpop(postpop),
         tau(nodes,deltat,index), longside(longside)
{
}

Propag::~Propag(void)
{
}

void Propag::step(void)
{
  p = prepop->Q(tau);
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
