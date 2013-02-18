#include<iostream>
using std::cerr;
using std::endl;
#include<cmath>
#include"fcap.h"

void fCaP::fCaDE::init( Configf& configf )
{
  CaDE::init(configf);
  configf>>xresponse; configf>>yresponse;
  /*configf.param("alpha",alpha);
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
  }*/
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
  CaDE::pot();
  xresponse.step();
  for( int i=0; i<nodes; i++ )
    variables[4][i] = xresponse[i];
  /*for( int i=0; i<nodes; i++ ) {
    double& x = variables[4][i];
    x1[i]->newHistory( _x(variables[2][i]) -x );
    x2[i]->newHistory( *x1[i]*lambda_x*mu_x -(lambda_x+mu_x)*x );
    x = *x2[i];
  }*/
}

void fCaP::fCaDE::dep(void)
{
  CaDE::dep();
  yresponse.step();
  for( int i=0; i<nodes; i++ )
    variables[5][i] = yresponse[i];
  /*for( int i=0; i<nodes; i++ ) {
    double& y = variables[5][i];
    y1[i]->newHistory( _y(variables[2][i]) -y );
    y2[i]->newHistory( *y1[i]*lambda_y*mu_y -(lambda_y+mu_y)*y );
    y = *y2[i];
  }*/
}

void fCaP::init( Configf& configf )
{
  CaDP::init(configf);
  oldnu = (*de)[3];
  init_nu = (*de)[3][0];

  configf.param("zeta",zeta);
  configf.param("lambda",lambda);
  configf.param("mu",mu);
  for( int i=0; i<nodes; i++ ) {
    newnu[i]  = new FractionalIntegral(zeta,deltat);
    newnu2[i] = new FractionalIntegral(zeta,deltat);
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
    //newnu[i]->newHistory( (newn-oldnu[i])/deltat );
    newnu[i]->newHistory( -newn +de->max*de->_x((*de)[2][i]) );
    newnu2[i]->newHistory( *newnu[i]*lambda*mu -(lambda+mu)*newn );
    newn = *newnu2[i] +init_nu;
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
  lminusm = lambda -mu;
  expl = exp(-lambda*deltat);
  expm = exp(-mu*deltat);
  factorlm = 1./lambda +1./mu;
}

void fCaP::fCaDE::Response::restart( Restartf& restartf )
{
}

void fCaP::fCaDE::Response::dump( Dumpf& dumpf ) const
{
}

void fCaP::fCaDE::Response::step(void)
{
  // assumes lambda!=mu
  for( int i=0; i<nodes; i++ ) {
    double dpdt = ( phi[i] -oldphi[i] )/deltat;
    adjustedphi = oldphi[i] -factorlm*dpdt -value[i];
    C1 = ( adjustedphi*mu -dvdt[i] +dpdt )/lminusm;
    C1expl = C1*expl;
    C2expm = expm*(-C1-adjustedphi);
    value[i] = C1expl+C2expm+phi[i] -factorlm*dpdt;
    dvdt[i] = C1expl*(-lambda) +C2expm*(-mu)+dpdt;
    oldphi[i] = phi[i];
  }
}

double fCaP::fCaDE::Response::operator [] ( int index ) const
{
  return value[index];
}
