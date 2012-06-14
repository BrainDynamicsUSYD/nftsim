#include<cmath>
#include"stp.h"

void STP::init( Configf& configf )
{
  Couple::init(configf);
  if( !configf.optional("nu_0",nu_0) ) {
    double phi = prepropag.phiinit(configf);
    nu_0 = n[0]*phi/transmitter[0]; //n[0]-pos*rate*phi*(80-1.5*phi);
  }
  configf.param("Rate",rate);
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
    double dXidphi = 1e-3*rate*exp(-rate*prepropag.phi()[i]); //rate*prepropag.phi()[i]*(20-prepropag.phi()[i]);
    dt = dXidphi*( prepropag.phi()[i]-oldphi[i] ) -transmitter[i]/t_glu;
    if( transmitter[i]+dt <0 ) transmitter[i] = 0;
    else transmitter[i] += dt;

    dn = nu_0 *log(prepropag.phi()[i]/oldphi[i])
        *( dXidphi -transmitter[i]/prepropag.phi()[i] );
    if( pos*( n[i]+dn ) < 0 ) n[i] = 0;
    else n[i] += dn;
    oldphi[i] = prepropag.phi()[i];
  }
  /*for( int i=0; i<nodes; i++ ) {
    n[i] = pos*rate*prepropag.phi()[i]*(80-1.5*prepropag.phi()[i])
          +nu_0;
    if( pos*n[i]<0 ) n[i] = 0;
  }*/
}

vector<Output*> STP::output(void) const
{
  vector<Output*> temp = Couple::output();
  temp.push_back( new Output( label("Couple.",index+1)+".Xi",transmitter) );
  return temp;
}
