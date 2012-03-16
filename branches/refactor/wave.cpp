/***************************************************************************
                          wave.cpp  -  description
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<cmath>
#include "wave.h"

void Wave::init( Configf& configf )
{
  double initphi; configf.Param("phi",initphi);
  p.resize(nodes,initphi);
  configf.Param("Deltax",deltax);
  configf.Param("Tau",tau);
  configf.Param("Range",range);
  configf.Param("gamma",gamma);
  oldp[0] = new Stencil(nodes,longside); *oldp[0] = p;
  oldp[1] = new Stencil(nodes,longside); *oldp[1] = p;
  oldQ[0] = new Stencil(nodes,longside); *oldQ[0] = prepop->Q(tau);
  oldQ[1] = new Stencil(nodes,longside); *oldQ[1] = prepop->Q(tau);

  dt2on12 = deltat*deltat/12.;
  dfact = dt2on12*gamma*gamma;
  dt2ondx2 = deltat*deltat/deltax/deltax;
  p2 = dt2ondx2*range*range*gamma*gamma;
  tenminus3p2 = 10.-3.*p2;
  twominus3p2 = 2.-3.*p2;
  exp1 = exp(-deltat*gamma);
  exp2 = exp(-2.*deltat*gamma);
}

void Wave::restart( Restartf& restartf )
{
}

void Wave::dump( Dumpf& dumpf ) const
{
}

Wave::Wave( int nodes, double deltat, int index, const Population* const prepop,
        const Population* const postpop, int longside )
    : Propag(nodes,deltat,index,prepop,postpop,longside)
{
}

Wave::~Wave(void)
{
  delete oldp[0]; delete oldp[1];
  delete oldQ[0]; delete oldQ[1];
}

void Wave::step(void)
{
  for( int i=0; i<nodes; i++, (*oldp[0])++, (*oldQ[0])++ ) {
    sump     = oldp[0]->n  +oldp[0]->s  +oldp[0]->w  +oldp[0]->e;
    diagsump = oldp[0]->nw +oldp[0]->ne +oldp[0]->sw +oldp[0]->se;
    sumQ     = oldQ[0]->n  +oldQ[0]->s  +oldQ[0]->w  +oldQ[0]->e;
    diagsumQ = oldQ[0]->nw +oldQ[0]->ne +oldQ[0]->sw +oldQ[0]->se;
    drive = dfact*( tenminus3p2*exp1*oldQ[0]->c +prepop->Q(tau)[i] +exp2*oldQ[1]->c +exp1*.5*p2*(sumQ+.5*diagsumQ) );
    p[i] = twominus3p2*exp1*oldp[0]->c +exp1*.5*p2*(sump+.5*diagsump) -exp2*oldp[1]->c +drive;
  }
  Stencil* temp = oldp[1];
  oldp[1] = oldp[0];
  oldp[0] = temp; *oldp[0] = p;
  temp = oldQ[1];
  oldQ[1] = oldQ[0];
  oldQ[0] = temp; *oldQ[0] = prepop->Q(tau);
}
