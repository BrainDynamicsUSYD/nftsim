#include<iostream>
using std::cerr;
using std::endl;
#include<cmath>
#include"fcap.h"

#define LOCAL 5

void fCaP::fCaDE::pot( vector<double>& Ca, vector<double>& x )
{
  /*for( int i=0; i<nodes; i++ ) {
    xhistory[i].push_front( CaDE::pot(Ca[i]) );
    if( xlocal[i].size() == LOCAL ) {
      double temp=0;
      for( size_t j=0; j<xlocal[i].size(); j++ )
        temp += xlocal[i][j];
      xhistory[i].push_front(temp);
      xlocal[i].clear();
    }    x[i] = 0;
    for( size_t tau=1; tau<xhistory[i].size(); tau++ )
      x[i] += xhistory[i][tau]*pow(tau*deltat,alpha);
    x[i] *= deltat/tgamma(-alpha_beta);
    x[i] += th;
  }*/
  CaDE::pot(Ca,x);
}

void fCaP::fCaDE::dep( vector<double>& Ca, vector<double>& y )
{
  /*for( int i=0; i<nodes; i++ ) {
    ylocal[i].push_front( CaDE::dep(Ca[i]) );
    if( ylocal[i].size() == LOCAL ) {
      double temp=0;
      for( size_t j=0; j<ylocal[i].size(); j++ )
        temp += ylocal[i][j];
      yhistory[i].push_front(temp);
      ylocal[i].clear();
    }
    y[i] = 0;
    for( size_t tau=1; tau<yhistory[i].size(); tau++ )
      y[i] += yhistory[i][tau]*pow(tau*deltat,alpha);
    y[i] *= deltat/tgamma(-alpha_beta);
    y[i] += th;
  }*/
  CaDE::dep(Ca,y);
}

void fCaP::init( Configf& configf )
{
  CaDP::init(configf);
  oldnu = (*de)[3];
  init_nu.push_back( (*de)[3][0] );

  configf.param("alpha",alpha); de->alpha = alpha;
  if( alpha<=0 ) {
    cerr<<"Fractional derivative order alpha of Couple "
        <<index+1<<" must satisfy 0<=alpha"<<endl;
    exit(EXIT_FAILURE);
  }
  double temp;
  for( int i=1; i<=ceil(alpha)-1; i++ )
    if( configf.optional( label("init.",i), temp ) )
      init_nu.push_back(temp);
    else
      init_nu.push_back(0);

  /*configf.param("beta",de->alpha_beta); de->alpha_beta = alpha -de->alpha_beta;
  if( alpha-de->alpha_beta<=0 ) {
    cerr<<"Fractional derivative order beta of Couple "
        <<index+1<<" must satisfy 0<=beta"<<endl;
    exit(EXIT_FAILURE);
  }*/

  de->ltp = pow( de->ltp, /*alpha-de->alpha_beta*/ alpha );
  de->ltd = pow( de->ltd, /*alpha-de->alpha_beta*/ alpha );
}

void fCaP::restart( Restartf& restartf )
{
}

void fCaP::dump( Dumpf& dumpf ) const
{
}

fCaP::fCaP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop )
    : CaDP(nodes,deltat,index,glu,prepropag,postpop), t(0), local(nodes), drive(nodes)
{
  delete de;
  de = new fCaDE(nodes,deltat);
}

fCaP::~fCaP(void)
{
}

void fCaP::step(void)
{
  CaDP::step();
  t += deltat;

  for( int i=0; i<nodes; i++ ) {
    // drive = history of eta(Omega-nu), most recent history at front
    // store only local averaged drives to reduce computation
    local[i].push_front( ((*de)[3][i]-oldnu[i])/deltat );
    if( local[i].size() == LOCAL ) {
      double temp=0;
      for( size_t j=0; j<local[i].size(); j++ )
        temp += local[i][j];
      drive[i].push_front(temp);
      local[i].clear();
    }

    (*de)[3][i] = 0;
    for( size_t tau=1; tau<drive[i].size(); tau++ )
      (*de)[3][i] += drive[i][tau]*pow(tau*deltat,alpha-1);
    (*de)[3][i] *= deltat/tgamma(alpha);
    for( int j=0; j<=ceil(alpha)-1; j++ )
      (*de)[3][i] += init_nu[j]*pow(t,j)/tgamma(j+1);
    if( (*de)[3][i]*pos <0 ) (*de)[3][i] = 0;
    oldnu[i] = (*de)[3][i];
  }
}
