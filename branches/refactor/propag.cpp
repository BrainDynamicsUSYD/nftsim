/***************************************************************************
                          propag.cpp  -  description
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "propag.h"

void Propag::init( Configf& configf )
{
  //double phiinit; configf.Param("Phi",phiinit);
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
    : NF(nodes,deltat,index), prepop(prepop), postpop(postpop), longside(longside)
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

void Propag::output( Array<Outputf>& outputfs ) const
{
  //outputfs.add( new Outputf(nodes,deltat,"neurofield.phi",p) );
}
