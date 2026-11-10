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
void DendriteRamp::DendriteDE::rhs( const vector<double>& y, vector<double>& dydt, size_type n ) {
  // y = {V,W==dv/dt,nuphi}
  // dydt = {dv/dt==W, dW/dt==d^2V/dt^2,dnuphi/dt}  d(nuphi)/dt from precouple
  dydt[0] = y[1];

  dydt[1] = (y[2] - y[0] - (1.0/alpha_vec[0] + 1.0/beta_vec[0]) * y[1]) * (alpha_vec[0] * beta_vec[0]);

  // nu*phi is constant over the step, or linear with Integration: SecondOrder.
  dydt[2] = dPdt.empty() ? 0.0 : dPdt[n];
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
  de->init(v); // call DendriteRamp::DendriteDE::init
  de->alpha_vec.push_back(alpha);
  de->beta_vec.push_back(beta);
}

void DendriteRamp::DendriteDE::init(const vector<double>& vinit) {
  variables[0] = vinit;
  variables[1].assign(nodes, 0.0);
  variables[2].assign(nodes, 0.0);
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
  if( second_order ) {
    // nu*phi starts at its value at t_n with the slope of the last step (so
    // RK4 integrates a linear input), and the ramped alpha and beta are
    // evaluated at the middle of the step rather than its end. Both make the
    // step second order (alpha and beta are still constant within a step).
    const double* np = precouple.nuphi().data();
    startSecondOrder(np);
    de->dPdt.resize(nodes);
    for( size_type i=0; i<nodes; i++ ) {
      (*de)[2][i] = np[i];
      de->dPdt[i] = (np[i] - prevnp[i])/deltat;
      prevnp[i] = np[i];
    }
  } else {
    for( size_type i=0; i<nodes; i++ ) {
      (*de)[2][i] = precouple[i];
    }
  }
  time += deltat;
  if( second_order ) {
    rampMidstep(time - 0.5*deltat);
  } else {
    ramp(time);
  }
  rk4->step();
  for( size_type i=0; i<nodes; i++ ) {
    v[i] = (*de)[0][i]; // Voltage
  }
}

/// Sets the ramped alpha and beta for time `t` (only inside the ramps).
void DendriteRamp::ramp( double t ) {
  // alpha_vec and beta_vec already initialized
  if (t > de->t1 && t <= de->t2) {  // ramp alpha, beta up
    de->alpha_vec[0] = alpha + (t - de->t1) * (de->alpha2-alpha) / (de->t2 - de->t1);
    de->beta_vec[0] = beta + (t - de->t1) * (de->beta2-beta) / (de->t2 - de->t1);
  }
//else if (time > de->t2 && time <= de->t3)  // alpha == alpha_0 + K
// {
//  de->alpha = de->alpha2;
//  de->beta = de->beta2;
//  }
  else if (t > de->t3 && t <= de->t4) {  // ramp nu down
    de->alpha_vec[0] =  de->alpha2 - (t - de->t3) * (de->alpha2 - alpha) / (de->t4 - de->t3);
    de->beta_vec[0] = de->beta2 - (t - de->t3) * (de->beta2 - beta) /(de->t4 - de->t3);
  }
//else if (time > t4)    // nu == nu_0
//{
//   de->alpha_vec[0] = alpha;
//   de->beta_vec[0] = beta;
//}
}

/**
  @brief Sets alpha and beta at time `t`, the middle of the current step.

  Used with `Integration: SecondOrder`. The rates are a piecewise-linear
  function of time: alpha up to t1, rising to alpha2 at t2, alpha2 until t3,
  falling back to alpha at t4, then alpha. DendriteRamp::ramp() only updates
  them inside the ramps, keeping the last value otherwise, which would hold a
  value from half a step before t2 (or t4) if evaluated at the mid-step.
*/
void DendriteRamp::rampMidstep( double t ) {
  double a = alpha;
  double b = beta;
  if( t > de->t1 && t <= de->t2 ) {
    a = alpha + (t - de->t1) * (de->alpha2-alpha) / (de->t2 - de->t1);
    b = beta + (t - de->t1) * (de->beta2-beta) / (de->t2 - de->t1);
  } else if( t > de->t2 && t <= de->t3 ) {
    a = de->alpha2;
    b = de->beta2;
  } else if( t > de->t3 && t <= de->t4 ) {
    a = de->alpha2 - (t - de->t3) * (de->alpha2 - alpha) / (de->t4 - de->t3);
    b = de->beta2 - (t - de->t3) * (de->beta2 - beta) / (de->t4 - de->t3);
  }
  de->alpha_vec[0] = a;
  de->beta_vec[0] = b;
}

void DendriteRamp::output( Output& output ) const {
  // output("Dendrite",index+1,"V",V());
  output.singleNode("Dendrite", index+1,"beta",de->beta_vec);
}
