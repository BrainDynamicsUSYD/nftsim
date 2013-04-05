#include"bcm.h"

void BCM::BCMDE::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y == { binding, H, Ca, nu, x, y, gNMDA, d/dt(gNMDA) }
  CaDE::rhs(y,dydt);
  // recalculate dCadt with NMDAR plasticity

  // gNMDA
  //dydt[6] = ( 1.0e2*(y[3] -dydt[3]/ltd*4) +5e-3 -y[6] )/10;
  dydt[9] = -(.1+.1)*y[9] +.1*.1*(1.6e2*y[3]-y[8]);
  dydt[8] = y[9];
  if( y[8]+dydt[8]*deltat < 0 ) dydt[8] = -y[8];
  // Ca
  dydt[2] = y[11]*y[0]*y[1] -y[2]/tCa; // replace gnmda with y[6]
  if( y[2]+dydt[2]*deltat < 0 ) dydt[2] = -y[2];
}

void BCM::BCMDE::pot(void)
{
  for( int i=0; i<nodes; i++ )
    variables[4][i] = _x( variables[10][i] );
}

void BCM::BCMDE::dep(void)
{
  for( int i=0; i<nodes; i++ )
    variables[5][i] = _y( variables[10][i] );
}

void BCM::init( Configf& configf )
{
  CaDP::init(configf);
  for( int i=0; i<nodes; i++ )
    (*de)[8][i] = de->gnmda;
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
  for( int i=0; i<nodes; i++ ) {
    (*de)[10][i] = (*de)[2][i]/pow((*de)[3][i]/13e-6,3/2);
    (*de)[11][i] = (*de)[8][i]/pow((*de)[3][i]/13e-6,3/2);
  }
  CaDP::step();
}

void BCM::output( Output& output ) const
{
  output.prefix("Couple",index+1);
  output("nutilde",(*de)[7]);
  output("nu",(*de)[3]);
  output("Ca",(*de)[10]);
  output("gNMDA",(*de)[11]);
}
