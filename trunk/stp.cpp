#include<cmath>
#include"stp.h"

STP::STP( int nodes, double deltat, int index,
    const vector<double>& glu, const Propag& prepropag,
    const Population& postpop )
    : Couple(nodes,deltat,index,glu,prepropag,postpop),
      de(nodes,deltat), rk4(de)
{
  de[1].clear(); de[1].resize(nodes,1e-5);
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
  double dXidphi = Xi_max*kappa/phi_r *pow( y[0]/phi_r, kappa-1 )
            *pow( 1+ pow(y[0]/phi_r,kappa), -2 );
  dydt[1] = dXidphi*(y[0]-y[3])/deltat -y[1]/t_Xi;
  if( y[1]+dydt[1]*deltat<0 ) dydt[1] = -y[1]/deltat;
  // nu
  dydt[2] = nu_0 *log(y[0]/y[3])/deltat *( dXidphi -y[1]/y[0] );
  if( pos*(y[2]+dydt[2]*deltat)<0 ) dydt[2] = -y[2]/deltat;
  // oldphi, leave alone
  dydt[3] = 0;
}

void STP::init( Configf& configf )
{
  Couple::init(configf); de.pos = pos;
  de[2].clear(); de[2].resize(nodes,n[0]);
  if( !configf.optional("nu_0",de.nu_0) ) {
    de.nu_0 = n[0]*prepropag.phiinit(configf)/de[1][0];
  }
  configf.param("phi_r",de.phi_r);
  configf.param("kappa",de.kappa);
  configf.param("t_Xi",de.t_Xi);
  configf.param("Xi_max",de.Xi_max);
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
