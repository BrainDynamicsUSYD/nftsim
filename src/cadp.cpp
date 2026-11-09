/** @file cadp.cpp
  @brief Defines CaDP, a coupling whose strength follows calcium-dependent plasticity.

  CaDP::step() sets the glutamate binding B and the voltage dependence H of
  each node, updates the potentiation and depression rates, integrates the
  state one step with RK4 and sets P = nu*phi.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "cadp.h"       // CaDP;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "make_unique.h" // nftsim::make_unique;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <cmath>     // std::exp;
#include <memory>   // std::unique_ptr;
#include <utility>  // std::move;
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
  dydt[3] = y[4]*(max-y[3]) - y[5]*y[3];
  // x, y, leave alone
  dydt[4] = dydt[5] = 0;
  // dnudt, nu
  dydt[6] = -2*z*y[6] + z*z*(y[3]-y[7]);
  dydt[7] = y[6];
}

double CaDP::CaDE::sig( double x, double beta ) const {
  return 1/(1+exp(-beta*x));
}

double CaDP::CaDE::_x(double Ca) const {
  return xth +ltp*sig(Ca-pth, cadp::ca_threshold_slope);
}

void CaDP::CaDE::pot() {
  for( size_type i=0; i<nodes; i++ ) {
    variables[4][i] = _x( variables[2][i] );
  }
}

double CaDP::CaDE::_y(double Ca) const {
  return yth + ltd*sig(Ca-dth, cadp::ca_threshold_slope) - ltd*sig(Ca-pth, cadp::ca_threshold_slope);
}

void CaDP::CaDE::dep() {
  for( size_type i=0; i<nodes; i++ ) {
    variables[5][i] = _y( variables[2][i] );
  }
}

void CaDP::init( Configf& configf ) {
  de->init(configf);
  pos = de->pos;
}

void CaDP::CaDE::init( Configf& configf ) {
  configf.param("nu",nu_init);
  variables[3].clear();
  variables[3].resize(nodes,nu_init);
  variables[7].clear();
  variables[7].resize(nodes,nu_init);
  variables[2].clear();
  variables[2].resize(nodes,cadp::ca_init);
  pos = (nu_init>0)?1:-1;
  configf.param("nu_max",max);
  configf.param("Dth",dth);
  configf.param("Pth",pth);
  configf.param("xyth",xth);
  yth = xth*(max-nu_init)/nu_init;
  configf.param("x",ltp);
  configf.param("y",ltd);
  configf.param("B",B);
  configf.param("glu_0",glu_0);
  if( !configf.optional("tCa",tCa) ) {
    tCa = cadp::default_tCa;
  }
  if( !configf.optional("gNMDA",gnmda) ) {
    gnmda = cadp::default_gNMDA;
  }
  if( !configf.optional("z",z) ) {
    z = cadp::default_z;
  }
}

CaDP::CaDP( size_type nodes, double deltat, size_type index,
            const Propagator& prepropag, const Population& postpop )
  : CaDP(nodes, deltat, index, prepropag, postpop,
         nftsim::make_unique<CaDE>(nodes,deltat)) {
}

CaDP::CaDP( size_type nodes, double deltat, size_type index,
            const Propagator& prepropag, const Population& postpop,
            std::unique_ptr<CaDE> de )
  : Coupling(nodes,deltat,index,prepropag,postpop), de(std::move(de)) {
  if( this->de ) {
    rk4 = nftsim::make_unique<RK4>(*this->de);
  }
}

CaDP::~CaDP() = default;

void CaDP::step() {
  const vector<double>& glu = postpop.glu();
  const vector<double>& V = postpop.V();
  for( size_type i=0; i<nodes; i++ ) {
    (*de)[0][i] = de->sig( glu[i] -de->glu_0, de->B );
    (*de)[1][i] = (cadp::nmda_reversal-V[i])*de->sig( V[i]-cadp::nmda_half_v, cadp::nmda_slope);
  }
  de->pot();
  de->dep();
  rk4->step();
  for( size_type i=0; i<nodes; i++ ) {
    P[i] = (*de)[7][i]*prepropag[i];
  }
}

void CaDP::output( Output& output ) const {
  output.prefix("Coupling",index+1);
  output("nutilde",(*de)[3]);
  output("nu",(*de)[7]);
  output("Ca",(*de)[2]);
  output("B", (*de)[0]);
  output("H", (*de)[1]);
}
