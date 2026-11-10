/** @file dendrite.cpp
  @brief Defines the Dendrite class, which handles dendritic dynamics.

  Dendrite dynamics are applied to incoming activity to postsynaptic Population
  objects. The differential equation defining the dendritic dynamics are in the
  member function Dendrite::DendriteDE::rhs.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "dendrite.h"   // Dendrite;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "de.h"         // RK4;
#include "output.h"     // Output;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <iostream> // std::endl; std::cerr;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::string;
using std::vector;

/**
  @brief Computes the derivatives of the dendritic response function.

  The dendritic response function is given by:
  \f{eqnarray*}{
    \frac{dv}{dt}&=&W \\
    \frac{dW}{dt}&=&\left(\nu\phi - V - \left(\frac{1}{\alpha} + \frac{1}{\beta}\right) W\right) \alpha \beta \\
    \frac{d\nu\phi}{dt}&=&0
  \f}
*/
void Dendrite::DendriteDE::rhs( const vector<double>& y, vector<double>& dydt, size_type /*unused*/ ) {
  // y = {V,W==dv/dt,nuphi}
  // dydt = {dv/dt==W, dW/dt==d^2V/dt^2,dnuphi/dt}  d(nuphi)/dt from precouple
  dydt[0] = y[1];

  dydt[1] = (y[2] - y[0] - factorab * y[1]) * alphaxbeta;

  dydt[2] = 0.0;
}

void Dendrite::init( Configf& configf ) {
  if( !configf.next( label("Dendrite ",index+1) )) {
    std::cerr<<"Dendrite "<<index+1<<" not found."<<std::endl;
    exit(EXIT_FAILURE);
  }
  string buffer("Steady");
  configf.optional("V",buffer);
  if( buffer == "Steady" ) {
    v.assign(nodes, precouple.nuinit(configf)*prepropag.phiinit(configf));
  } else {
    v.assign(nodes, atof(buffer.c_str()));
  }

  configf.param("alpha",alpha);
  configf.param("beta",beta);

  // Initialize constant factors to speed up computation.
  factorab = 1./alpha + 1./beta;
  alphaxbeta = alpha * beta;

  de->init(v); // call Dendrite::DendriteDE::init
  de->factorab = factorab;
  de->alphaxbeta = alphaxbeta;
}

void Dendrite::DendriteDE::init(const vector<double>& vinit) {
  variables[0] = vinit;
  variables[1].assign(nodes, 0.0);
  variables[2].assign(nodes, 0.0);
}

Dendrite::Dendrite( size_type nodes, double deltat, size_type index,
                    const Propagator& prepropag, const Coupling& precouple )
  : NF(nodes,deltat,index), v(nodes), prepropag(prepropag), precouple(precouple) {
  de = new DendriteDE(nodes, deltat);
  rk4 = new RK4(*de);
}

Dendrite::~Dendrite() {
  delete de;
  delete rk4;
}

void Dendrite::startSecondOrder( const double* np ) {
  // Zero slope on the first step: nu*phi at t_-1 is taken equal to nu*phi at t_0.
  if( !started ) {
    prevnp.assign(np, np+nodes);
    started = true;
  }
}

void Dendrite::step() {
  if( second_order ) {
    // Integration: SecondOrder. As below, but nu*phi is extrapolated linearly
    // over the step from its values at t_n (np) and t_n-1 (prevnp), so the RK4
    // stages use nu*phi at t_n, t_n + h/2 and t_n + h. The input's own
    // truncation error is then O(h^2) over a step, giving a second-order scheme
    // for the coupled system (holding nu*phi constant is first order).
    const double* __restrict__ np = precouple.nuphi().data();
    startSecondOrder(np);
    double* __restrict__ pp = prevnp.data();
    double* __restrict__ V = (*de)[0].data();
    double* __restrict__ W = (*de)[1].data();
    double* __restrict__ P = (*de)[2].data();
    const double h = deltat;
    const double h2 = deltat*0.5;
    const double h6 = deltat/6.0;
    const double fab = factorab;
    const double ab = alphaxbeta;
    for( size_type i=0; i<nodes; i++ ) {
      const double p0 = np[i];
      const double dp = p0 - pp[i];
      const double pm = p0 + 0.5*dp; // extrapolated to t_n + h/2
      const double p1 = p0 + dp;     // extrapolated to t_n + h
      pp[i] = p0;
      P[i] = p0;
      const double v0 = V[i];
      const double w0 = W[i];
      const double kv1 = w0;
      const double kw1 = (p0 - v0 - fab * w0) * ab;
      const double v1 = v0 + (h2 * kv1);
      const double w1 = w0 + (h2 * kw1);
      const double kv2 = w1;
      const double kw2 = (pm - v1 - fab * w1) * ab;
      const double v2 = v0 + (h2 * kv2);
      const double w2 = w0 + (h2 * kw2);
      const double kv3 = w2;
      const double kw3 = (pm - v2 - fab * w2) * ab;
      const double v3 = v0 + (h * kv3);
      const double w3 = w0 + (h * kw3);
      const double kv4 = w3;
      const double kw4 = (p1 - v3 - fab * w3) * ab;
      V[i] = v0 + h6*(kv1 + (2.0 * kv2) + (2.0 * kv3) + kv4);
      W[i] = w0 + h6*(kw1 + (2.0 * kw2) + (2.0 * kw3) + kw4);
      v[i] = V[i];
    }
    return;
  }
  // Classical RK4 for DendriteDE, written out over all nodes. It performs the
  // same arithmetic, in the same order, as RK4::step() with DendriteDE::rhs
  // (so results are unchanged), but avoids a virtual rhs() call and a gather
  // per node per stage, which dominated nftsim's run time. nu*phi is held
  // constant over the step (d(nu*phi)/dt = 0 in DendriteDE::rhs).
  const double* __restrict__ np = precouple.nuphi().data();
  double* __restrict__ V = (*de)[0].data();
  double* __restrict__ W = (*de)[1].data();
  double* __restrict__ P = (*de)[2].data();
  const double h = deltat;
  const double h2 = deltat*0.5;
  const double h6 = deltat/6.0;
  const double fab = factorab;
  const double ab = alphaxbeta;
  for( size_type i=0; i<nodes; i++ ) {
    const double p = np[i];
    P[i] = p;
    const double v0 = V[i];
    const double w0 = W[i];
    const double kv1 = w0;
    const double kw1 = (p - v0 - fab * w0) * ab;
    const double v1 = v0 + (h2 * kv1);
    const double w1 = w0 + (h2 * kw1);
    const double kv2 = w1;
    const double kw2 = (p - v1 - fab * w1) * ab;
    const double v2 = v0 + (h2 * kv2);
    const double w2 = w0 + (h2 * kw2);
    const double kv3 = w2;
    const double kw3 = (p - v2 - fab * w2) * ab;
    const double v3 = v0 + (h * kv3);
    const double w3 = w0 + (h * kw3);
    const double kv4 = w3;
    const double kw4 = (p - v3 - fab * w3) * ab;
    V[i] = v0 + h6*(kv1 + (2.0 * kv2) + (2.0 * kv3) + kv4);
    W[i] = w0 + h6*(kw1 + (2.0 * kw2) + (2.0 * kw3) + kw4);
    v[i] = V[i];
  }
}

void Dendrite::output( Output& output ) const {
  output("Dendrite",index+1,"V",v);
}
