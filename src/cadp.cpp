/** @file cadp.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "cadp.h"       // CaDP;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <cmath>     // std::exp;
#include <vector>   // std::vector;
using std::exp;
using std::vector;

void CaDP::CaDE::rhs( const vector<double>& y, vector<double>& dydt, size_type /*unused*/ ) {
  // y == { binding, H, Ca, nutilde, x, y, dnudt, nu }
  // binding, leave alone
  dydt[0] = 0;
  // H, leave alone
  dydt[1] = 0;
  // Ca
  dydt[2] = gnmda*y[0]*y[1] -y[2]/tCa;
  if( y[2]+dydt[2]*deltat < 0 ) {
    dydt[2] = -y[2];
  }
  // nutilde
  //dydt[3] = y[4]*pow((max-y[3])/(max-13e-6),1+1./.7) -y[5]*pow(y[3]/13e-6,1+1./.7);
  //dydt[3] *= .7*13e-6;
  dydt[3] = y[4]*(max-y[3]) - y[5]*y[3];
  // x, y, leave alone
  dydt[4] = dydt[5] = 0;
  // dnudt, nu
  dydt[6] = -2*z*y[6] + z*z*(y[3]-y[7]);
  dydt[7] = y[6]; //dydt[3];
}

double CaDP::CaDE::sig( double x, double beta ) const {
  return 1/(1+exp(-beta*x));
}

double CaDP::CaDE::_x(double Ca) const {
  return xth +ltp*sig(Ca-pth, 4e7);
}

void CaDP::CaDE::pot() {
  for( size_type i=0; i<nodes; i++ ) {
    variables[4][i] = _x( variables[2][i] );
  }
}

double CaDP::CaDE::_y(double Ca) const {
  return yth + ltd*sig(Ca-dth, 4e7) - ltd*sig(Ca-pth, 4e7);
}

void CaDP::CaDE::dep() {
  for( size_type i=0; i<nodes; i++ ) {
    variables[5][i] = _y( variables[2][i] );
  }
}

void CaDP::init( Configf& configf ) {
  de->init(configf);
  pos = de->pos;
  nu_init = de->nu_init;
}

void CaDP::CaDE::init( Configf& configf ) {
  configf.param("nu",nu_init);
  variables[3].clear();
  variables[3].resize(nodes,nu_init);
  variables[7].clear();
  variables[7].resize(nodes,nu_init);
  variables[2].clear();
  variables[2].resize(nodes,.01e-6);
  pos = (nu_init>0)?1:-1;
  configf.param("nu_max",max);
  configf.param("Dth",dth);
  configf.param("Pth",pth);
  configf.param("xyth",xth);
  yth = xth*(max-nu_init)/nu_init;
  //yth = xth;
  configf.param("x",ltp);
  configf.param("y",ltd);
  configf.param("B",B);
  configf.param("glu_0",glu_0);
  if( !configf.optional("tCa",tCa) ) {
    tCa = 50e-3;
  }
  if( !configf.optional("gNMDA",gnmda) ) {
    gnmda = 2e-3;
  }
  if( !configf.optional("z",z) ) {
    z = .01;
  }
}

CaDP::CaDP( size_type nodes, double deltat, size_type index,
            const Propagator& prepropag, const Population& postpop )
  : Coupling(nodes,deltat,index,prepropag,postpop) {
  de = new CaDE(nodes,deltat);
  rk4 = new RK4(*de);
}

CaDP::~CaDP() {
  delete de;
  delete rk4;
}

void CaDP::step() {
  for( size_type i=0; i<nodes; i++ ) {
    (*de)[0][i] = de->sig( postpop.glu()[i] -de->glu_0, de->B );
    (*de)[1][i] = (195e-3-postpop.V()[i])*de->sig( postpop.V()[i]-45.5e-3, 62);
  }
  de->pot();
  de->dep();
  rk4->step();
  for( size_type i=0; i<nodes; i++ ) {
    P[i] = (*de)[7][0]*prepropag[i];
  }
}

void CaDP::output( Output& output ) const {
  output.prefix("Coupling",index+1);
  output("nutilde",(*de)[3]);
  output("nu",(*de)[7]);
  output("Ca",(*de)[2]);
  output("B", (*de)[0]);
  output("H", (*de)[1]);
  //output("x", (*de)[4]);
  //output("y", (*de)[5]);
}
