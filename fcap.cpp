#include<iostream>
using std::cerr;
using std::endl;
#include<cmath>
#include"fcap.h"

void fCaP::fCaDE::init( Configf& configf )
{
  CaDE::init(configf);
  xresponse.init(configf); yresponse.init(configf);
  /*xresponse[0].clear(); xresponse[0].resize(nodes,th);
  yresponse[0].clear(); yresponse[0].resize(nodes,th);*/
}

void fCaP::fCaDE::pot(void)
{
  CaDE::pot();
  xresponse.input(variables[4][0]);
  variables[4][0] = ltp*pow(xresponse,2)/(pow(xresponse,2)+pow(ltp/2,2));
  /*for( int i=0; i<nodes; i++ )
    xresponse[2][i] = variables[4][i];
  xrk4.step();
  for( int i=0; i<nodes; i++ )
    variables[4][i] = xresponse[0][i];*/
}

void fCaP::fCaDE::dep(void)
{
  CaDE::dep();
  yresponse.input(variables[5][0]);
  variables[5][0] = ltd*pow(yresponse,2)/(pow(yresponse,2)+pow(ltd/2,2));
  /*for( int i=0; i<nodes; i++ )
    yresponse[2][i] = variables[5][i];
  yrk4.step();
  for( int i=0; i<nodes; i++ )
    variables[5][i] = yresponse[0][i];*/
}

void fCaP::init( Configf& configf )
{
  CaDP::init(configf);
  oldnu = (*de)[3];
  init_nu = (*de)[3][0];

  configf.param("zeta",zeta);
  for( int i=0; i<nodes; i++ ) {
    newnu[i]  = new FractionalIntegral(zeta,deltat,init_nu);
    newnu2[i] = new FractionalIntegral(zeta,deltat,0);
  }
}

void fCaP::restart( Restartf& restartf )
{
}

void fCaP::dump( Dumpf& dumpf ) const
{
}

fCaP::fCaP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop )
    : CaDP(nodes,deltat,index,glu,prepropag,postpop), newnu(nodes), newnu2(nodes), oldnu(nodes)
{
  delete de;
  delete rk4;
  de = new fCaDE(nodes,deltat);
  rk4 = new RK4(*de);
}

fCaP::~fCaP(void)
{
  for( int i=0; i<nodes; i++ )
    delete newnu[i];
}

void fCaP::step(void)
{
  CaDP::step();
  for( int i=0; i<nodes; i++ ) {
    double& newn = (*de)[3][i];
    // put in the "rate" == eta(Omega-nu) from CaDP::rhs()
    newnu[i]->newHistory( (newn-oldnu[i])/deltat );
    //newnu[i]->newHistory( -newn +de->max*de->_x((*de)[2][i]) );
    //newnu2[i]->newHistory( *newnu[i]*lambda*mu -(lambda+mu)*newn );
    newn = *newnu[i]; //+init_nu;
    if( newn*pos <0 ) newn = 0;
    oldnu[i] = newn;
  }
}

fCaP::FractionalIntegral::operator double() const
{
  double integral = 0;
  for( size_t tau=1; tau<history.size(); tau++ )
    integral += history[tau] *pow( tau*deltat, alpha-1 );
  integral *= deltat/tgamma(alpha);
  integral += init;
  return integral;
}

void fCaP::FractionalIntegral::newHistory ( double newest_history )
{
  history.push_front( newest_history );
}

void fCaP::fCaDE::Response::init( Configf& configf )
{
  configf.param("lambda",lambda);
  configf.param("mu",mu);
}

/*void fCaP::fCaDE::Response::rhs(
        const vector<double>& y, vector<double>& dydt )
{
  // y = { y, dydt, xtilde }
  dydt[0] = y[1];
  if( y[0]+dydt[0] <0 ) dydt[0] = -y[0];
  dydt[1] = -(lambda+mu)*y[1] +lambda*mu*y[0]*(y[2]-1);
  dydt[2] = 0;
}*/

fCaP::fCaDE::Response::operator double() const
{
  return _d1;
}

void fCaP::fCaDE::Response::input( double input )
{
  d2.newHistory( lambda*mu*_d1*(input-1) -(lambda+mu)*_d2 );
  _d2 = d2;
  d1.newHistory( _d2 );
  _d1 = d1;
}
