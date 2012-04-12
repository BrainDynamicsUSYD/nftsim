#include<iostream>
using std::endl;
#include<string>
using std::string;
#include<cmath>
#include"bcm.h"

void BCM::init( Configf& configf )
{
  CaDP::init(configf);
  for( int i=0; i<1/deltat; i++ )
    history.push_back( vector<double>(nodes,0) );
  dumpf.open( string( label("neurofield.Couple.",index+1) +string(".th") ) );
}

void BCM::restart( Restartf& restartf )
{
  CaDP::restart(restartf);
}

void BCM::dump( Dumpf& dumpf ) const
{
  CaDP::dump(dumpf);
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
  key = (key+1) % history.size();
  history[key] = Ca;
  pth = 0;
  for( size_t i=0; i<history.size(); i++ )
    for( int j=0; j<nodes; j++ )
      pth += pow( history[i][j]/1e-6, 2 );
  pth /= history.size()*nodes/1e-6;
  pth += dth;
  CaDP::step();
  dumpf<<pth<<endl;
}
