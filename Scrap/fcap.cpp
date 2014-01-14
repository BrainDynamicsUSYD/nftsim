#include"fcap.h"

void fCaP::FractionalIntegral::newHistory ( double newest_history )
{
  history.push_front( newest_history );
}

void fCaP::FractionalIntegral::step(void)
{
  m = 0;
  for( size_t tau=1; tau<history.size(); tau++ )
    m += history[tau] *pow( tau*deltat, alpha-1 );
  m *= deltat/tgamma(alpha);
  m += init;
  return m;
}

void fCaP::init( Configf& configf )
{
  configf.param("lambda",lambda);
  configf.param("mu",mu);

  configf.param("zeta",zeta);
  for( int i=0; i<nodes; i++ ) {
    newnu[i]  = new FractionalIntegral(zeta,deltat,init_nu);
    newnu2[i] = new FractionalIntegral(zeta,deltat,0);
  }
}

fCaP::fCaDE::Response::operator double() const
{
  return _d1;
}

void fCaP::fCaDE::Response::input( double input )
{
  d2.newHistory( lambda*mu*_d1*(input-1) -(lambda+mu)*_d2 );
  _d2 = d2;
  d1.newHistory( _d2 );
  _d1 = d1;
}
