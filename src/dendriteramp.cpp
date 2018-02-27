/** @file dendriteramp.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "dendriteramp.h" // DendriteRamp;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "de.h"         // RK4;
#include "output.h"     // Output;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector>   // std::vector;
using std::vector;

/**
  @brief Computes the dendritic ramp.

  The dendritic ramp is given by:
  \f{eqnarray*}{
    \frac{dv}{dt}&=&W \\
    \frac{dW}{dt}&=&\left(\nu\phi - V - \left(\frac{1}{\alpha} + \frac{1}{\beta}\right) W\right) \alpha \beta \\
    \frac{d\nu\phi}{dt}&=&0
  \f}
*/
void DendriteRamp::DendriteDE::rhs( const vector<double>& y, vector<double>& dydt, size_type /*unused*/ ) {
  // y = {V,W==dv/dt,nuphi}
  // dydt = {dv/dt==W, dW/dt==d^2V/dt^2,dnuphi/dt}  d(nuphi)/dt from precouple
  dydt[0] = y[1];

  dydt[1] = (y[2] - y[0] - (1.0/alpha_vec[0] + 1.0/beta_vec[0]) * y[1]) * (alpha_vec[0] * beta_vec[0]);

  dydt[2] = 0.0;
}

void DendriteRamp::init( Configf& configf ) {
  Dendrite::init(configf);
  //configf.param("alpha1",alpha1);
  //configf.param("beta1",beta1);
  configf.param("alpha2",de->alpha2);
  configf.param("beta2",de->beta2);
  configf.param("t1",de->t1);
  configf.param("t2",de->t2);
  configf.param("t3",de->t3);
  configf.param("t4",de->t4);
  time = 0;
  de->init(v[0]); // call DendriteRamp::DendriteDE::init
  de->alpha_vec.push_back(alpha);
  de->beta_vec.push_back(beta);
}

void DendriteRamp::DendriteDE::init(const double vinit) {
  variables[0].clear();
  variables[1].clear();
  variables[2].clear();
  variables[0].resize(nodes, vinit);
  variables[1].resize(nodes, 0.0);
  variables[2].resize(nodes, 0.0);
}

DendriteRamp::DendriteRamp( size_type nodes, double deltat, size_type index,
                            const Propagator& prepropag, const Coupling& precouple )
  : Dendrite(nodes,deltat,index,prepropag,precouple)

{
  de = new DendriteDE(nodes, deltat);
  rk4 = new RK4(*de);
}

DendriteRamp::~DendriteRamp() {
  delete de;
  delete rk4;
}

void DendriteRamp::step() {
  for( size_type i=0; i<nodes; i++ ) {
    (*de)[2][i] = precouple[i];
  }
  time += deltat;
  // alpha_vec and beta_vec already initialized
  if (time > de->t1 && time <= de->t2) {  // ramp alpha, beta up
    de->alpha_vec[0] = alpha + (time - de->t1) * (de->alpha2-alpha) / (de->t2 - de->t1);
    de->beta_vec[0] = beta + (time - de->t1) * (de->beta2-beta) / (de->t2 - de->t1);
  }
//else if (time > de->t2 && time <= de->t3)  // alpha == alpha_0 + K
// {
//  de->alpha = de->alpha2;
//  de->beta = de->beta2;
//  }
  else if (time > de->t3 && time <= de->t4) {  // ramp nu down
    de->alpha_vec[0] =  de->alpha2 - (time - de->t3) * (de->alpha2 - alpha) / (de->t4 - de->t3);
    de->beta_vec[0] = de->beta2 - (time - de->t3) * (de->beta2 - beta) /(de->t4 - de->t3);
  }
//else if (time > t4)    // nu == nu_0
//{
//   de->alpha_vec[0] = alpha;
//   de->beta_vec[0] = beta;
//}
  rk4->step();
  for( size_type i=0; i<nodes; i++ ) {
    v[i] = (*de)[0][i]; // Voltage
  }
}

void DendriteRamp::output( Output& output ) const {
  // output("Dendrite",index+1,"V",V());
  output.singleNode("Dendrite", index+1,"beta",de->beta_vec);
}
