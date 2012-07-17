#include<cmath>
#include"stp.h"

STP::STP( int nodes, double deltat, int index,
    const vector<double>& glu, const Propag& prepropag,
    const Population& postpop )
    : Couple(nodes,deltat,index,glu,prepropag,postpop),
      de(nodes,deltat), rk4(de)
{
  de[1].clear(); de[1].resize(nodes,1e-4);
}

STP::~STP(void)
{
}

void STP::STPde::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y == { phi, Xi, nu, oldphi }
  // phi, leave alone
  dydt[0] = 0;
  // Xi
  double dXidphi = Xi_max*kappa/phi_r*pow( y[0]/phi_r, kappa-1 )
            *pow(1+pow( y[0]/phi_r, kappa ),-2);
  dydt[1] = dXidphi*(y[0]-y[3])/deltat -y[0]/t_Xi;
  if( y[0]+dydt[0]*deltat<0 ) dydt[0] = -y[0];
  // nu
  dydt[2] = nu_0 *log(y[0]/y[3]) *( dXidphi -y[1]/y[0] );
  if( pos*(y[2]+dydt[2]*deltat)<0 ) dydt[2] = -y[2];
  // oldphi, leave alone
  dydt[3] = 0;
}

void STP::init( Configf& configf )
{
  double nuinit; configf.param("nu",nuinit);
  de[2].clear(); de[2].resize(nodes,nuinit);
  pos = de.pos = (nuinit>0)?1:-1;
  if( !configf.optional("nu_0",de.nu_0) ) {
    de.nu_0 = nuinit*prepropag.phiinit(configf)/de[1][0];
  }
  configf.param("phi_r",de.phi_r);
  configf.param("kappa",de.kappa);
  configf.param("t_Xi",de.t_Xi);
  configf.param("Xi_max",de.t_Xi);
  for( int i=0; i<nodes; i++ )
    de[3][i] = prepropag.phiinit(configf);
}

void STP::restart( Restartf& restartf )
{
}

void STP::dump( Dumpf& dumpf ) const
{
}

void STP::step(void)
{
  de[0] = prepropag.phi();
  rk4.step();
  de[3] = prepropag.phi();
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
  return de[2];
}

vector<Output*> STP::output(void) const
{
  vector<Output*> temp;
  temp.push_back( new Output( label("Couple.",index+1)+".Xi",de[1]) );
  temp.push_back( new Output( label("Couple.",index+1)+".nu",de[2]) );
  return temp;
}
