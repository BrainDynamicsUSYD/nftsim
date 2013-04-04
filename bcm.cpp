#include"bcm.h"

void BCM::BCMDE::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y == { binding, H, Ca, nu, x, y, gNMDA, d/dt(gNMDA) }
  CaDE::rhs(y,dydt);
  // recalculate dCadt with NMDAR plasticity

  // gNMDA
  //dydt[6] = ( 1.0e2*(y[3] -dydt[3]/ltd*4) +5e-3 -y[6] )/10;
  dydt[7] = -(100+10)*y[7] +100*10*y[6]*(1.6e2*y[3]-y[6]);
  dydt[6] = y[7];
  if( y[6]+dydt[6]*deltat < 0 ) dydt[6] = -y[6];
  // Ca
  dydt[2] = y[6]*y[0]*y[1] -y[2]/tCa; // replace gnmda with y[6]
  if( y[2]+dydt[2]*deltat < 0 ) dydt[2] = -y[2];
}

void BCM::BCMDE::pot(void)
{
  for( int i=0; i<nodes; i++ )
    variables[4][i] = _x( variables[2][i] );
}

void BCM::BCMDE::dep(void)
{
  for( int i=0; i<nodes; i++ )
    variables[5][i] = _y( variables[2][i] );
}

void BCM::init( Configf& configf )
{
  CaDP::init(configf);
  for( int i=0; i<nodes; i++ )
    (*de)[6][i] = de->gnmda;
}

BCM::BCM( int nodes, double deltat, int index, const vector<double>& glu,
        const Propag& prepropag, const Population& postpop )
    : CaDP(nodes,deltat,index,glu,prepropag,postpop)
{
  delete de; de = new BCMDE(nodes,deltat);
  delete rk4; rk4 = new RK4(*de);
}

BCM::~BCM(void)
{
}

void BCM::step(void)
{
  static vector<double> oldnu(nodes,13e-6);
  for( int i=0; i<nodes; i++){
    (*de)[2][i] /= pow( (*de)[3][i]/13e-6, 3/3 );
    //(*de)[6][i] /= pow( (*de)[3][i]/oldnu[i], 3/3 );
    oldnu[i]=(*de)[3][i];
  }
  CaDP::step();
}

void BCM::output( Output& output ) const
{
  CaDP::output(output);
  output("gNMDA",(*de)[6]);
}
