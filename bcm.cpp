#include<iostream>
using std::endl;
#include<string>
using std::string;
#include<cmath>
#include"bcm.h"

void BCM::init( Configf& configf )
{
  CaDP::init(configf);
  configf.param("gain",gain);
}

BCM::BCM( int nodes, double deltat, int index, const vector<double>& glu,
          const Population& prepop, const Population& postpop )
    : CaDP( nodes, deltat, index, glu, prepop, postpop )
{
}

BCM::~BCM(void)
{
}

void BCM::step(void)
{
  for( int i=0; i<nodes; i++ ) {
    double dg = deltat*(-gain*n[i] -g[i]/100 );
    if( g[i]+dg < 0 )
      g[i] = 0;
    else
      g[i] += dg;
  }
  CaDP::step();
}

vector<Output*> BCM::output(void) const
{
  vector<Output*> temp = CaDP::output();
  temp.push_back( new Output( label("Couple.",index+1)+".g", g ) );
  return temp;
}
