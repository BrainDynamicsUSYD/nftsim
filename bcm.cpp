//#include<deque>
//using std::deque;
#include"bcm.h"

void BCM::BCMDE::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y == { binding, H, Ca, nu, x, y, gNMDA, d/dt(gNMDA) }
  CaDE::rhs(y,dydt);
  // recalculate dCadt with NMDAR plasticity

  // Ca
  dydt[2] = y[8]*y[0]*y[1] -y[2]/tCa; // replace gnmda with y[8]
  if( y[2]+dydt[2]*deltat < 0 ) dydt[2] = -y[2];
  // gNMDA
  dydt[8] = -y[8]/t_BCM *(y[3]/y[7]-1) +(gnmda_0-y[8])/1000;
  if( y[7]==0 ) dydt[8] = 0;
}

void BCM::BCMDE::init( Configf& configf )
{
  CaDE::init(configf);
  configf.param("t_BCM",t_BCM);
  for( int i=0; i<nodes; i++ )
    variables[8][i] = gnmda;
  gnmda_0 = gnmda;
}

BCM::BCM( int nodes, double deltat, int index,
        const Propag& prepropag, const Population& postpop )
    : CaDP(nodes,deltat,index,prepropag,postpop)
{
  delete de; de = new BCMDE(nodes,deltat);
  delete rk4; rk4 = new RK4(*de);
}

BCM::~BCM(void)
{
}

void BCM::output( Output& output ) const
{
  output.prefix("Couple",index+1);
  output("nu",(*de)[7]);
  output("nutilde",(*de)[3]);
  output("Ca",(*de)[2]);
  output("B",(*de)[0]);
  output("H",(*de)[1]);
  output("gNMDA",(*de)[8]);
}
