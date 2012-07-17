#include<cmath>
#include"stp.h"

void STP::init( Configf& configf )
{
  double nuinit; configf.param("nu",nuinit);
  Xi_nu[2].clear(); Xi_nu[2].resize(nodes,nuinit);
  pos = Xi_nu.pos = (nuinit>0)?1:-1;
  if( !configf.optional("nu_0",nu_0) ) {
    Xi_nu.nu_0 = nuinit*prepropag.phiinit(configf)/Xi_nu[1][0];
  }
  configf.param("phi_r",Xi_nu.phi_r);
  configf.param("kappa",Xi_nu.kappa);
  configf.param("t_Xi",Xi_nu.t_Xi);
  configf.param("Xi_max",Xi_nu.t_Xi);
  for( int i=0; i<nodes; i++ )
    Xi_nu[3][i] = prepropag.phiinit(configf);
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
      Xi_nu(nodes,deltat)
      //oldphi(nodes), transmitter(nodes,1e-4)
{
  Xi_nu[1].clear(); Xi_nu[1].resize(nodes,1e-4);
}

STP::~STP(void)
{
}

void STP::step(void)
{
  Xi_nu[0] = prepropag.phi();
  Xi_nu.step();
  Xi_nu[3] = prepropag.phi();
  /*double dn; double dt;
  for( int i=0; i<nodes; i++ )
  {
    double bracket = pow( prepropag[i]/phi_r, kappa-1 );
    double dXidphi = 1e-3*kappa/phi_r*bracket *pow(1+bracket*prepropag[i]/phi_r,-2);
    //1e-3/4/20/pow(cosh((prepropag[i]-40)/2/20),2); //1e-3/sqrt(2*3.14)/10*exp(-.5*pow((prepropag[i]-35)/10,2)); //1e-3*rate*exp(-rate*prepropag[i]);
    dt = dXidphi*( prepropag[i]-oldphi[i] ) -transmitter[i]/t_Xi;
    if( transmitter[i]+dt <0 ) transmitter[i] = 0;
    else transmitter[i] += dt;

    dn = nu_0 *log(prepropag[i]/oldphi[i])
        *( dXidphi -transmitter[i]/prepropag[i] );
    if( pos*( n[i]+dn ) < 0 ) n[i] = 0;
    else n[i] += dn;
    oldphi[i] = prepropag[i];
  }*/
  /*double dn;
  for( int i=0; i<nodes; i++ )
  {
    dn = nu_0*1e-3 *prepropag[i]/pow(20+prepropag[i],2) *log(prepropag[i]/oldphi[i]);
    oldphi[i] = prepropag[i];
    if( pos*( n[i]+dn ) < 0 ) n[i] = 0;
    else n[i] += dn;
  }*/
}

const vector<double>& STP::nu(void) const
{
  return Xi_nu[2];
}

vector<Output*> STP::output(void) const
{
  vector<Output*> temp;
  temp.push_back( new Output( label("Couple.",index+1)+".Xi",Xi_nu[1]) );
  temp.push_back( new Output( label("Couple.",index+1)+".nu",Xi_nu[2]) );
  return temp;
}
