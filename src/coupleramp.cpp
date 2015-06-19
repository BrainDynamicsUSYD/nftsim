#include<iostream>
using std::cerr;
using std::endl;
#include "coupleramp.h"

void CoupleRamp::init( Configf& configf )
{
  configf.param("nu1",nu1);
  configf.param("nu2",nu2);
  configf.param("t1",t1);
  configf.param("t2",t2);

  deltanu = deltat*(nu2-nu1)/(t2-t1);

  n.clear(); n.resize(nodes,nu1);
  pos = (nu1>0)?1:-1;

  for( int i=0; i<nodes; i++ )
    P[i] = n[i]*prepropag.phiinit(configf);

  time = 0;
}


void CoupleRamp::step(void)
{
  time += deltat;
  if(time >= t1  && time <= t2)
    for( int i=0; i<nodes; i++ )
      n[i] += deltanu;

  Couple::step();
}

CoupleRamp::CoupleRamp( int nodes, double deltat, int index,
        const Propag& prepropag, const Population& postpop )
    : Couple(nodes,deltat,index,prepropag,postpop)
{
}

CoupleRamp::~CoupleRamp(void)
{
}