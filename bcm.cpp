#include<deque>
using std::deque;
#include"bcm.h"

void BCM::BCMDE::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y == { binding, H, Ca, nu, x, y, gNMDA, d/dt(gNMDA) }
  CaDE::rhs(y,dydt);
  // recalculate dCadt with NMDAR plasticity

  // Ca
  dydt[2] = y[8]*y[0]*y[1] -y[2]/tCa; // replace gnmda with y[6]
  if( y[2]+dydt[2]*deltat < 0 ) dydt[2] = -y[2];
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
  static deque<double> phihistory(int(5/deltat),8.87);
  static double average = 8.87;
  phihistory.push_back(prepropag[0]);
  average += ( prepropag[0] -phihistory.front() )/(phihistory.size()-1);
  phihistory.pop_front();
  /*for( int i=0; i<phihistory.size(); i++ )
    average += phihistory[i];
  average /= phihistory.size();*/

  for( int i=0; i<nodes; i++ ) {
    (*de)[8][i] = 8.87/average*2e-3;
  }
  CaDP::step();
}

void BCM::output( Output& output ) const
{
  output.prefix("Couple",index+1);
  output("nu",(*de)[3]);
  output("Ca",(*de)[2]);
  output("gNMDA",(*de)[8]);
}
