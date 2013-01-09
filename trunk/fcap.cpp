#include<iostream>
using std::cerr;
using std::endl;
#include<cmath>
#include"fcap.h"

void fCaP::init( Configf& configf )
{
  CaDP::init(configf);
  oldnu = de[3];
  init_nu = de[3][0];

  configf.param("alpha",alpha);
  if( alpha<=0 /*|| alpha>2*/ ) {
    cerr<<"Fractional derivative order alpha of Couple "
        <<index+1<<" must satisfy 0<alpha"<<endl;
    exit(EXIT_FAILURE);
  }

  de.ltp = pow(de.ltp,alpha);
  de.ltd = pow(de.ltd,alpha);
}

void fCaP::restart( Restartf& restartf )
{
}

void fCaP::dump( Dumpf& dumpf ) const
{
}

fCaP::fCaP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop )
    : CaDP(nodes,deltat,index,glu,prepropag,postpop), local(nodes), drive(nodes)
{
}

fCaP::~fCaP(void)
{
}

void fCaP::step(void)
{
  CaDP::step();

  for( int i=0; i<nodes; i++ ) {
    // drive = history of eta(Omega-nu), most recent history at front
    // store only local averaged drives to reduce computation
    local[i].push_back( (de[3][i]-oldnu[i])/deltat );
    if( local[i].size() == 10 ) {
      double temp=0;
      for( size_t j=0; j<local[i].size(); j++ )
        temp += local[i][j];
      drive[i].push_front(temp);
      local[i].clear();
    }

    de[3][i] = 0;
    for( size_t tau=1; tau<drive[i].size(); tau++ )
      de[3][i] += drive[i][tau]*pow(tau*deltat,alpha-1);
    de[3][i] *= deltat/tgamma(alpha);
    de[3][i] += init_nu;
    if( de[3][i]*pos <0 ) de[3][i] = 0;
    oldnu[i] = de[3][i];
  }
}
