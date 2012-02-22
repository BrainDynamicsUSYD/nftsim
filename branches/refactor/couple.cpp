/***************************************************************************
                          couple.cpp  -  governs coupling strength nu
                             -------------------
    copyright            : (C) 2007 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "couple.h"

void Couple::init( Configf& configf )
{
  double nuinit; configf.Param("nu",nuinit);
  n.resize(nodes,nuinit);
  pos = (nuinit>0)?1:0;
}

void Couple::restart( Restartf& restartf )
{
}

void Couple::dump( Dumpf& dumpf ) const
{
}

Couple::Couple( int nodes, double deltat, int index, const vector<double>& glu,
        const Population* const prepop, const Population* const postpop )
    : NF(nodes,deltat,index), glu(glu), prepop(prepop), postpop(postpop)
{
}

Couple::~Couple(void)
{
}

void Couple::step(void)
{
}

void Couple::output( Array<Outputf>& outputfs ) const
{
  //outputfs.add( new Outputf(nodes,deltat,"neurofield.nu",n) );
}

const vector<double>& Couple::nu(void) const
{
  return n;
}

bool Couple::excite(void) const
{
  return pos;
}
