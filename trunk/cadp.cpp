#include<cmath>
#include"cadp.h"

void CaDP::CaDE::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y == { binding, H, Ca, nu, x, y }
  // binding, leave alone
  dydt[0] = 0;
  // H, leave alone
  dydt[1] = 0;
  // Ca
  dydt[2] = gnmda*y[0]*y[1] -y[2]/tCa;
  if( y[2]+dydt[2]*deltat < 0 ) dydt[2] = -y[2];
  // nu
  dydt[3] = y[4]*(max-y[3]) -y[5]*y[3];
  if( pos*(y[3]+dydt[3]*deltat) < 0 ) dydt[3] = -y[3];
  // x, y, leave alone
  dydt[4] = dydt[5] = 0;
}

double CaDP::CaDE::sig( double x, double beta ) const
{
  return 1/(1+exp(-beta*x));
}

double CaDP::CaDE::_x(double Ca) const
{
  return th +ltp*sig(Ca-pth,4e7);
}

void CaDP::CaDE::pot(void)
{
  for( int i=0; i<nodes; i++ )
    variables[4][i] = _x( variables[2][i] );
}

double CaDP::CaDE::_y(double Ca) const
{
  return th +ltd*sig(Ca-dth,4e7) -ltd*sig(Ca-pth,4e7);
}

void CaDP::CaDE::dep(void)
{
  for( int i=0; i<nodes; i++ )
    variables[5][i] = _y( variables[2][i] );
}

void CaDP::init( Configf& configf )
{
  de->init(configf);
  pos = de->pos;
}

void CaDP::CaDE::init( Configf& configf )
{
  double nuinit; configf.param("nu",nuinit);
  variables[3].clear(); variables[3].resize(nodes,nuinit);
  pos = (nuinit>0)?1:-1;
  configf.param("nu_max",max);
  if( !configf.optional("LTD",dth) )
    dth = .25e-6;
  if( !configf.optional("LTP",pth) )
    pth = .45e-6;
  if( !configf.optional("Threshold",th) )
    th = 1e-4;
  configf.param("x",ltp);
  configf.param("y",ltd);
  configf.param("B",B);
  configf.param("glu_0",glu_0);
  if( !configf.optional("tCa",tCa) )
    tCa = 50e-3;
  if( !configf.optional("gNMDA",gnmda) )
    gnmda = 2e-3;
}

void CaDP::restart( Restartf& restartf )
{
}

void CaDP::dump( Dumpf& dumpf ) const
{
}

CaDP::CaDP( int nodes, double deltat, int index, const vector<double>& glu,
        const Propag& prepropag, const Population& postpop )
    : Couple(nodes,deltat,index,glu,prepropag,postpop)
{
  de = new CaDE(nodes,deltat);
  rk4 = new RK4(*de);
}

CaDP::~CaDP(void)
{
  delete de;
  delete rk4;
}

void CaDP::step(void)
{
  for( int i=0; i<nodes; i++ ) {
    (*de)[0][i] = de->sig( glu[i] -de->glu_0, de->B );
    (*de)[1][i] = (195e-3-postpop[i])*de->sig( postpop[i]-45.5e-3,62 );
  }
  de->pot(); de->dep();
  rk4->step();
}

const vector<double>& CaDP::nu(void) const
{
  return (*de)[3];
}

void CaDP::output( Output& output ) const
{
  output.prefix("Couple",index+1);
  output("nu",(*de)[3]);
  output("Ca",(*de)[2]);
  output("B", (*de)[0]);
  output("x", (*de)[4]);
  output("y", (*de)[5]);
}
