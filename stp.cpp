#include<cmath>
#include"stp.h"

void STP::init( Configf& configf )
{
  Couple::init(configf);
  if( !configf.optional("nu_0",nu_0) ) {
    nu_0 = n[0]*prepropag.phiinit(configf)/transmitter[0];
  }
  configf.param("phi_r",phi_r);
  configf.param("kappa",kappa);
  configf.param("t_glu",t_glu);
  for( size_t i=0; i<oldphi.size(); i++ )
    oldphi[i] = prepropag.phiinit(configf);
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
    : Couple(nodes,deltat,index,glu,prepropag,postpop),
      oldphi(nodes), transmitter(nodes,1e-4)
{
}

STP::~STP(void)
{
}

void STP::step(void)
{
  double dn; double dt;
  for( int i=0; i<nodes; i++ )
  {
    double bracket = pow( prepropag[i]/phi_r, kappa-1 );
    double dXidphi = 1e-3*kappa/phi_r*bracket *pow(1+bracket*prepropag[i]/phi_r,-2);
    //1e-3/4/20/pow(cosh((prepropag[i]-40)/2/20),2); //1e-3/sqrt(2*3.14)/10*exp(-.5*pow((prepropag[i]-35)/10,2)); //1e-3*rate*exp(-rate*prepropag[i]);
    dt = dXidphi*( prepropag[i]-oldphi[i] ) -transmitter[i]/t_glu;
    if( transmitter[i]+dt <0 ) transmitter[i] = 0;
    else transmitter[i] += dt;

    dn = nu_0 *log(prepropag[i]/oldphi[i])
        *( dXidphi -transmitter[i]/prepropag[i] );
    if( pos*( n[i]+dn ) < 0 ) n[i] = 0;
    else n[i] += dn;
    oldphi[i] = prepropag[i];
  }
  /*double dn;
  for( int i=0; i<nodes; i++ )
  {
    dn = nu_0*1e-3 *prepropag[i]/pow(20+prepropag[i],2) *log(prepropag[i]/oldphi[i]);
    oldphi[i] = prepropag[i];
    if( pos*( n[i]+dn ) < 0 ) n[i] = 0;
    else n[i] += dn;
  }*/
}

vector<Output*> STP::output(void) const
{
  vector<Output*> temp = Couple::output();
  temp.push_back( new Output( label("Couple.",index+1)+".Xi",transmitter) );
  return temp;
}
