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
  dydt[8] = -y[8]/t_BCM *(y[3]/y[7]-1) +(maxgnmda-y[8])/1000;
}

void BCM::BCMDE::init( Configf& configf )
{
  CaDE::init(configf);
  nu_init = variables[3][0];
  configf.param("t_BCM",t_BCM);
  for( int i=0; i<nodes; i++ )
    variables[8][i] = gnmda;
  maxgnmda = gnmda;
  //gnmda = t_BCM; // this hack is to allow BCM::init() to grab t_BCM
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

/*void BCM::init( Configf& configf )
{
  CaDP::init(configf);
  t_BCM = de->gnmda;
  de->gnmda = (*de)[8][0];
}*/

void BCM::step(void)
{
  /*static double init = 13e-6; //postpop()[0];
  static deque<double> history( int(t_BCM/deltat),init );
  static double average = init;
  history.push_back( (*de)[3][0] ); //postpop()[0]
  average += ( history.back() -history.front() )/(history.size()-1);
  history.pop_front();

  for( int i=0; i<nodes; i++ )
    (*de)[8][i] = 2e-3*pow( (*de)[7][0]/(*de)[3][0] ,2);*/
  CaDP::step();
}

void BCM::output( Output& output ) const
{
  output.prefix("Couple",index+1);
  output("nu",(*de)[7]);
  output("nutilde",(*de)[3]);
  output("Ca",(*de)[2]);
  output("gNMDA",(*de)[8]);
  output("x",(*de)[4]);
  output("y",(*de)[5]);
}
