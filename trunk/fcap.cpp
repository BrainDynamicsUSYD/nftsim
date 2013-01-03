#include<iostream>
using std::cerr;
using std::endl;
#include<cmath>
#include"fcap.h"

void fCaP::init( Configf& configf )
{
  CaDP::init(configf);
  n = de[3];
  for( int i=0; i<nodes; i++ )
    drive[i].resize(1/1e-4,0); // memory 10 seconds, memory resolution 10 ms
  configf.param("alpha",alpha);
  if( alpha>=0 || alpha<=-1 ) {
    cerr<<"Fractional order alpha of Couple "<<index+1<<" must be between -1 and 0"<<endl;
    exit(EXIT_FAILURE);
  }
  order = ceil(alpha);
}

void fCaP::restart( Restartf& restartf )
{
}

void fCaP::dump( Dumpf& dumpf ) const
{
}

fCaP::fCaP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop )
    : CaDP(nodes,deltat,index,glu,prepropag,postpop), drive(nodes), oldnu(nodes)
{
}

fCaP::~fCaP(void)
{
}

void fCaP::step(void)
{
  CaDP::step();

  static double t=0;
  if( fmod(t,1e-4)==0 ) {
    for( int i=0; i<nodes; i++ ) {
      drive[i].pop_back();
      //drive[i].push_front( (de[3][i]-oldnu[i])/deltat );
      drive[i].push_front( nu()[i] );
    }
  }
  t+=deltat;

  for( int i=0; i<nodes; i++ ) {
    for( size_t tau=1; tau<drive[i].size(); tau++ )
      n[i] += deltat/tgamma(-alpha)*drive[i][tau]*pow(tau*10e-3,-alpha-1);
      //n[i] += drive[i][tau]*pow(tau*deltat*10,order-alpha-1)/tgamma(order-alpha)*deltat;
    if( pos*n[i] < 0 ) n[i] = 0;
    ///*de[3][i] =*/ oldnu[i] = n[i];
  }
}

const vector<double>& fCaP::nu(void) const
{
  return n;
}
