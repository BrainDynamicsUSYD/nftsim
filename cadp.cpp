#include<cmath>
#include"cadp.h"

void CaDP::CaDE::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y == { binding, H, Ca, nu }
  // binding, leave alone
  dydt[0] = 0;
  // H, leave alone
  dydt[1] = 0;
  // Ca
  dydt[2] = gnmda*y[0]*y[1] -y[2]/tCa;
  if( y[2]+dydt[2]*deltat < 0 ) dydt[2] = -y[2];
  // nu
  dydt[3] = po(y[2])*(max-y[3]) -de(y[2])*y[3];
  if( pos*(y[3]+dydt[3]*deltat) < 0 ) dydt[3] = -y[3];
}

double CaDP::CaDE::sig( double x, double beta ) const
{
  return 1/(1+exp(-beta*x));
}

double CaDP::CaDE::po(double Ca) const
{
  return th +ltp*sig(Ca-pth,4e7);
}

double CaDP::CaDE::de(double Ca) const
{
  return th +ltd*sig(Ca-dth,4e7) -ltd*sig(Ca-pth,4e7);
}

void CaDP::init( Configf& configf )
{
  double nuinit; configf.param("nu",nuinit);
  de[3].clear(); de[3].resize(nodes,nuinit);
  pos = de.pos = (nuinit>0)?1:-1;
  configf.param("nu_max",de.max);
  if( !configf.optional("LTD",de.dth) )
    de.dth = .25e-6;
  if( !configf.optional("LTP",de.pth) )
    de.pth = .45e-6;
  if( !configf.optional("Threshold",de.th) )
    de.th = 1e-4;
  configf.param("x",de.ltp);
  configf.param("y",de.ltd);
  configf.param("B",de.B);
  configf.param("glu_0",de.glu_0);
  if( !configf.optional("tCa",de.tCa) )
    de.tCa = 50e-3;
  if( !configf.optional("gNMDA",de.gnmda) )
    de.gnmda = 2e-3;
}

void CaDP::restart( Restartf& restartf )
{
}

void CaDP::dump( Dumpf& dumpf ) const
{
}

CaDP::CaDP( int nodes, double deltat, int index, const vector<double>& glu,
        const Propag& prepropag, const Population& postpop )
  : Couple(nodes,deltat,index,glu,prepropag,postpop),
    de(nodes,deltat), rk4(de)
{
}

CaDP::~CaDP(void)
{
}

void CaDP::step(void)
{
  for( int i=0; i<nodes; i++ ) {
    de[0][i] = de.sig( glu[i] -de.glu_0, de.B );
    de[1][i] = (195e-3-postpop[i])*de.sig( postpop[i]-45.5e-3,62 );
  }
  rk4.step();
}

const vector<double>& CaDP::nu(void) const
{
  return de[3];
}

vector<Output*> CaDP::output(void) const
{
  vector<Output*> temp;
  temp.push_back( new Output( label("Couple.",index+1)+".nu", de[3] ) );
  temp.push_back( new Output( label("Couple.",index+1)+".Ca", de[2] ) );
  temp.push_back( new Output( label("Couple.",index+1)+".B",  de[0] ) );
  return temp;
}
