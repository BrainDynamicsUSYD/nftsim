#include<cmath>
#include"stp.h"

void STP::init( Configf& configf )
{
  Couple::init(configf);
  configf.param("Rate",rate);
  configf.param("phi_r",phi_r);
  if( !configf.optional("nu_0",nu_0) ) {
    double phi = prepropag.phiinit(configf);
    nu_0 = n[0]-pos*rate*pow(phi,2)*(3*phi_r-phi);
  }
}

void STP::restart( Restartf& restartf )
{
}

void STP::dump( Dumpf& dumpf ) const
{
}

STP::STP( int nodes, double deltat, int index,
    const vector<double>& glu, const Propag& prepropag,
    const Population& postpop )
    : Couple(nodes,deltat,index,glu,prepropag,postpop), oldphi(nodes)
{
}

STP::~STP(void)
{
}

void STP::step(void)
{
  /*double dn;
  for( int i=0; i<nodes; i++ )
  {
    dn = rate* (prepropag.phi()[i]-oldphi[i])
        *prepropag.phi()[i]*( 9*20 -4*prepropag.phi()[i] );
    if( pos*( n[i]+dn ) < 0 )
      n[i] = 0;
    else
      n[i] += dn;
    oldphi[i] = prepropag.phi()[i];
  }*/
  for( int i=0; i<nodes; i++ ) {
    n[i] = pos*rate*pow(prepropag.phi()[i],2)*(3*phi_r-prepropag.phi()[i])
          +nu_0;
    if( pos*n[i]<0 ) n[i] = 0;
  }
}
