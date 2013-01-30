#include<iostream>
using std::cerr;
using std::endl;
#include<cmath>
#include"fcap.h"

void fCaP::fCaDE::init( Configf& configf )
{
  CaDE::init(configf);
  configf.param("alpha",alpha);
  configf.param("tau_x",tau_x);
  configf.param("beta",beta);
  configf.param("tau_y",tau_y);
}

void fCaP::fCaDE::pot(void)
{
  /*for( int i=0; i<nodes; i++ ) {
    double& x = variables[4][i];
    xhistory[i].push_front(
      pow(_x(variables[2][i]),alpha) -x/pow(tau_x,alpha) );
    x = 0;
    for( size_t tau=1; tau<xhistory[i].size(); tau++ )
      x += xhistory[i][tau]*pow(tau*deltat,alpha-1);
    x *= deltat/tgamma(alpha);
  }*/
}

void fCaP::fCaDE::dep(void)
{
  /*for( int i=0; i<nodes; i++ ) {
    double& y = variables[5][i];
    yhistory[i].push_front(
      pow(_y(variables[2][i]),beta) -y/pow(tau_y,beta) );
    y = 0;
    for( size_t tau=1; tau<yhistory[i].size(); tau++ )
      y += yhistory[i][tau]*pow(tau*deltat,beta-1);
    y *= deltat/tgamma(beta);
  }*/
}

void fCaP::init( Configf& configf )
{
  CaDP::init(configf);
  /*oldnu = (*de)[3];
  init_nu = (*de)[3][0];

  configf.param("alpha",alpha);
  if( alpha<=0 ) {
    cerr<<"Fractional derivative order alpha of Couple "
        <<index+1<<" must satisfy 0<=alpha"<<endl;
    exit(EXIT_FAILURE);
  }

  de->ltp = pow( de->ltp, alpha );
  de->ltd = pow( de->ltd, alpha );*/
}

void fCaP::restart( Restartf& restartf )
{
}

void fCaP::dump( Dumpf& dumpf ) const
{
}

fCaP::fCaP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop )
    : CaDP(nodes,deltat,index,glu,prepropag,postpop)//,
    //history1st(nodes), history2nd(nodes)
{
  delete de;
  delete rk4;
  de = new fCaDE(nodes,deltat);
  rk4 = new RK4(*de);
}

fCaP::~fCaP(void)
{
}

void fCaP::step(void)
{
  CaDP::step();

  /*for( int i=0; i<nodes; i++ ) {
    // drive = history of eta(Omega-nu), most recent history at front
    history1st[i].push_front( ((*de)[3][i]-oldnu[i])/deltat );
    (*de)[3][i] = 0;
    for( size_t tau=1; tau<history1st[i].size(); tau++ )
      (*de)[3][i] += history1st[i][tau]*pow(tau*deltat,alpha-1);
    (*de)[3][i] *= deltat/tgamma(alpha);
    (*de)[3][i] += init_nu;
    if( (*de)[3][i]*pos <0 ) (*de)[3][i] = 0;
    oldnu[i] = (*de)[3][i];
  }*/

  /*for( int i=0; i<nodes; i++ ) {
    double& nui = (*de)[3][i];
    double eta = de->_x( (*de)[2][i] ) +de->_y( (*de)[2][i] );

    history1st[i].push_front( eta* ( nui-oldnu[i] )/deltat );
    double etadrive = 0;
    for( size_t tau=1; tau<history1st[i].size(); tau++ )
      etadrive += history1st[i][tau] *pow(tau*deltat,1-alpha);
    etadrive *= deltat/tgamma(alpha);

    history2nd[i].push_front( etadrive -2*eta*nui );
    nui = 0;
    for( size_t tau=1; tau<history2nd[i].size(); tau++ )
      nui += history2nd[i][tau] *pow(tau*deltat,alpha-1);
    nui *= deltat/tgamma(alpha);
    nui += init_nu;
    if( nui*pos <0 ) nui = 0;

    oldnu[i] = nui;
  }*/

}
