#include<iostream>
using std::cerr;
using std::endl;
#include<cmath>
#include"fcap.h"

void fCaP::fCaDE::init( Configf& configf )
{
  CaDE::init(configf);
  configf.param("alpha",alpha);
  configf.param("lambda_x",lambda_x);
  configf.param("mu_x",mu_x);
  configf.param("beta",beta);
  configf.param("lambda_y",lambda_y);
  configf.param("mu_y",mu_y);
  ltp = pow(ltp,alpha);
  lambda_x = pow(lambda_x,alpha); lambda_x = pow(lambda_x,alpha);
  ltd = pow(ltd,beta);
  lambda_y = pow(lambda_y,beta);  lambda_y = pow(lambda_y,beta);
  for( int i=0; i<nodes; i++ ) {
    x1[i] = new FractionalIntegral(alpha,deltat);
    y1[i] = new FractionalIntegral(beta,deltat);
    x2[i] = new FractionalIntegral(alpha,deltat);
    y2[i] = new FractionalIntegral(beta,deltat);
  }
}

void fCaP::fCaDE::rhs( const vector<double>& y, vector<double>& dydt )
{
  CaDE::rhs(y,dydt); // keep old stuff
  // y == { binding, H, Ca, nu, x, y }
  // change nu
  //double m = .6;
  //dydt[3] = y[4]*(pow(max,m)-pow(y[3],m)) -y[5]*pow(y[3],m);
  //dydt[3] = y[4]*pow(max-y[3],m) -y[5]*pow(y[3],m);
  //if( pos*(y[3]+dydt[3]*deltat) < 0 ) dydt[3] = -y[3];
}

void fCaP::fCaDE::pot(void)
{
  //CaDE::pot();
  for( int i=0; i<nodes; i++ ) {
    double& x = variables[4][i];
    x1[i]->newHistory( _x(variables[2][i]) -x );
    x2[i]->newHistory( *x1[i]*lambda_x*mu_x -(lambda_x+mu_x)*x );
    x = *x2[i];
  }
}

void fCaP::fCaDE::dep(void)
{
  //CaDE::dep();
  for( int i=0; i<nodes; i++ ) {
    double& y = variables[5][i];
    y1[i]->newHistory( _y(variables[2][i]) -y );
    y2[i]->newHistory( *y1[i]*lambda_y*mu_y -(lambda_y+mu_y)*y );
    y = *y2[i];
  }
}

void fCaP::init( Configf& configf )
{
  CaDP::init(configf);
  oldnu = (*de)[3];
  init_nu = (*de)[3][0];

  configf.param("zeta",zeta);
  if( zeta<=0 ) {
    cerr<<"Fractional derivative order zeta of Couple "
        <<index+1<<" must satisfy 0<=zeta"<<endl;
    exit(EXIT_FAILURE);
  }
  for( int i=0; i<nodes; i++ )
    newnu[i] = new FractionalIntegral(zeta,deltat);
}

void fCaP::restart( Restartf& restartf )
{
}

void fCaP::dump( Dumpf& dumpf ) const
{
}

fCaP::fCaP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop )
    : CaDP(nodes,deltat,index,glu,prepropag,postpop), newnu(nodes), oldnu(nodes)
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
    newnu[i]->newHistory( ((*de)[3][i]-oldnu[i])/deltat );
    (*de)[3][i] = *newnu[i] +init_nu;
    if( (*de)[3][i]*pos <0 ) (*de)[3][i] = 0;
    oldnu[i] = (*de)[3][i];
  }
}

fCaP::FractionalIntegral::operator double() const
{
  double integral = 0;
  for( size_t tau=1; tau<history.size(); tau++ )
    integral += history[tau] *pow( tau*deltat, alpha-1 );
  integral *= deltat/tgamma(alpha);
  return integral;
}

void fCaP::FractionalIntegral::newHistory ( double newest_history )
{
  history.push_front( newest_history );
}
