#include<cmath>
#include"stp.h"

void STP::init( Configf& configf )
{
  Couple::init(configf);
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
    : Couple(nodes,deltat,index,this->glu,prepropag,postpop),
      Ca(nodes,0), glu(nodes,0), oldlngluonphi(nodes,log(1e-15))
{
}

STP::~STP(void)
{
}

void STP::step(void)
{
  double dCa; double dglu; double dn;
  for( int i=0; i<nodes; i++ )
  {
    dCa = ( .1e-1*(1e-6-Ca[i])*prepropag.phi()[i] -Ca[i]/100e-3 )*deltat;
    if( Ca[i]+dCa < 0 )
      Ca[i] = 0;
    else
      Ca[i] += dCa;

    dglu = ( 1e6*(1e-3-glu[i])*Ca[i] -glu[i]/10e-3 )*deltat;
    if( glu[i]+dglu < 0 )
      glu[i] = 0;
    else
      glu[i] += dglu;

    dn = n[i] *( log( glu[i]/prepropag.phi()[i] )-oldlngluonphi[i] );
    if( pos*( n[i]+dn ) < 0 )
      n[i] = 0;
    else
      n[i] += dn;
    oldlngluonphi[i] = log( glu[i]/prepropag.phi()[i] );
  }
}

vector<Output*> STP::output(void) const
{
  vector<Output*> temp = Couple::output();
  temp.push_back( new Output( label("Couple.",index+1)+".Ca", Ca ) );
  temp.push_back( new Output( label("Couple.",index+1)+".glu", glu ) );
  return temp;
}
