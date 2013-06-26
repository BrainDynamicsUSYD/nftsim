#include<cmath>
#include<string>
#include"stp.h"

STP::STP( int nodes, double deltat, int index,
    const vector<double>& glu, const Propag& prepropag,
    const Population& postpop )
    : Couple(nodes,deltat,index,glu,prepropag,postpop),
      de(nodes,deltat), rk4(de)
{
}

STP::~STP(void)
{
}

void STP::STPde::rhs( const vector<double>& y, vector<double>& dydt )
{
  /*// y == { phi, oldphi, p, t_Xi, Ca, Xi, nu }
  const double& phi    = y[0];
  const double& oldphi = y[1];
  const double& p      = y[2];
  const double& t_Xi   = y[3];
  const double& Ca     = y[4];
  const double& Xi     = y[5];
  const double& nu     = y[6];
  // leave alone algebraic expressions
  dydt[0] = dydt[1] = dydt[2] = dydt[3] = 0;
  // Ca
  dydt[4] = 1e-7*phi -Ca/50e-3;
  if( y[4]+dydt[4]*deltat<0 ) dydt[4] = -y[4]/deltat;
  // Xi
  dydt[5] = p*phi*(Xi_max-Xi) -Xi*t_Xi;
  if( y[5]+dydt[5]*deltat<0 ) dydt[5] = -y[5]/deltat;
  // nu
  dydt[6] = nu_0 /pow(phi,2) *( -phi*dydt[5] -(phi-oldphi)/deltat*Xi );
  if( pos*(y[6]+dydt[6]*deltat)<0 ) dydt[6] = -y[6]/deltat;*/

  /*// y == { phi, Xi, nu, oldphi }
  // phis, leave alone
  dydt[0] = dydt[3] = 0;
  // Xi
  //dydt[1] = y[0]*(Xi_max-y[1])*kappa/phi_r *pow( y[0]/phi_r, kappa-1 )
            // *pow( 1+ pow(y[0]/phi_r,kappa), -2 ) -y[1]/t_Xi;
  if( y[1]+dydt[1]*deltat<0 ) dydt[1] = -y[1]/deltat;
  // nu
  dydt[2] = nu_0 /pow(y[0],2) *( y[0]*dydt[1] -(y[0]-y[3])/deltat*y[1] );
  if( pos*(y[2]+dydt[2]*deltat)<0 ) dydt[2] = -y[2]/deltat;*/

  // y == { phi, Xi, nu, oldphi, dXidt }
  // phis, leave alone
  dydt[0] = dydt[3] = 0;
  // Xi as 2nd order DE
  if( undersaturate )
  dydt[1] = y[0]*(Xi_max-y[1]) *kappa/phi_r*pow(y[0]/phi_r,kappa)
      *pow(1+pow(y[0]/phi_r,kappa),-2) -y[1]/t_Xi /*-1*y[4]*/;
  else
  dydt[1] = y[0]*(Xi_max-y[1])
           *pow(y[0],kappa)/(pow(y[0],kappa)+phi_r)-y[1]/t_Xi /*-y[4]*/;
  dydt[4] /= t_Xi;
  //dydt[1] = y[4];
  if( y[1]+dydt[1]*deltat<0 ) dydt[1] = -y[1]/deltat;
  // nu
  dydt[2] = nu_0 /pow(y[0],2) *( y[0]*dydt[1] -(y[0]-y[3])/deltat*y[1] );
  if( pos*(y[2]+dydt[2]*deltat)<0 ) dydt[2] = -y[2]/deltat;
}

void STP::init( Configf& configf )
{
  Couple::init(configf);
  de.pos = pos; de[2].clear(); de[2].resize(nodes,n[0]);
  configf.param("phi_r",de.phi_r);
  configf.param("kappa",de.kappa);
  configf.param("t_Xi",de.t_Xi); de.t_Xi = de.t_Xi;
  configf.param("Xi_max",de.Xi_max);
  de[1].clear(); de[1].resize(nodes,de.Xi_max/10.);
  de.nu_0 = n[0]*prepropag.phiinit(configf)/de[1][0];
  for( int i=0; i<nodes; i++ )
    de[3][i] = prepropag.phiinit(configf);
  de.undersaturate = false;
  string temp; if( configf.optional("Undersaturate",temp) )
    de.undersaturate = (temp=="True");
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
  /*for( int i=0; i<nodes; i++ ) {
    // p
    de[2][i] = .7*pow(de[4][i],4)/(pow(de[4][i],4)+.1e-26)
              +.6*pow(de[4][i]/(de[4][i]+.03e-6),4);
    if( de[2][i]>1 ) de[2][i] = 1;
    // t_Xi
    de[3][i] = de.t_Xi +(de.t_Xi*2-de[3][i])* (de[4][i]/(de[4][i]+.05e-6));
  }*/
  //de.t_Xi = 50e-3 -45e-3*prepropag[0]/(prepropag[0]+150);
  rk4.step();
  de[3] = prepropag.phi();
  Couple::step();
}

const vector<double>& STP::nu(void) const
{
  return de[2];
}

void STP::output( Output& output) const
{
  output.prefix("Couple",index+1);
  //output("Ca",de[4]);
  output("Xi",de[1]);
  output("nu",de[2]);
  //output("dXidphi",de.dXidphi);
  //output("Xionphi",de.Xionphi);
  //output("dphidt",de.dphidt);
}
