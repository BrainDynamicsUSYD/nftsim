/** @file dendrite.cpp
  @brief Defines the Dendrite class, which handles dendritic dynamics.

  Dendrite dynamics are applied to incoming activity to postsynaptic Population
  objects. Dendrite::step() integrates the dendritic equation (see dendrite.h)
  with classical RK4, written out over all nodes.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "dendrite.h"   // Dendrite;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "output.h"     // Output;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <iostream> // std::endl; std::cerr;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::string;
using std::vector;

void Dendrite::init( Configf& configf ) {
  if( !configf.next( label("Dendrite ",index+1) )) {
    std::cerr<<"Dendrite "<<index+1<<" not found."<<std::endl;
    exit(EXIT_FAILURE);
  }
  string buffer("Steady");
  configf.optional("V",buffer);
  if( buffer == "Steady" ) { // V = nu*phi, with the propagator's initial phi at each node
    const double nu0 = precouple.nuinit(configf);
    const vector<double> phi0 = prepropag.phiinit(configf);
    v.resize(nodes);
    for( size_type i=0; i<nodes; i++ ) {
      v[i] = nu0*phi0[i];
    }
  } else {
    v.assign(nodes, atof(buffer.c_str()));
  }

  configf.param("alpha",alpha);
  configf.param("beta",beta);

  // Initialize constant factors to speed up computation.
  factorab = 1./alpha + 1./beta;
  alphaxbeta = alpha * beta;

  dvdt.assign(nodes, 0.0); // start at rest
}

Dendrite::Dendrite( size_type nodes, double deltat, size_type index,
                    const Propagator& prepropag, const Coupling& precouple )
  : NF(nodes,deltat,index), v(nodes), dvdt(nodes, 0.0),
    prepropag(prepropag), precouple(precouple) {
}

Dendrite::~Dendrite() = default;

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
    double* __restrict__ V = v.data();
    double* __restrict__ W = dvdt.data();
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
    }
    return;
  }
  // Classical RK4 for the state (V, W = dV/dt) with
  //   dV/dt = W,  dW/dt = (nu*phi - V - (1/alpha + 1/beta) W) alpha beta,
  // written out over all nodes, with nu*phi held constant over the step.
  // It does the same arithmetic, in the same order, as the generic RK4::step()
  // in de.h would, but without a virtual rhs() call and a gather per node per
  // stage, which dominated nftsim's run time.
  const double* __restrict__ np = precouple.nuphi().data();
  double* __restrict__ V = v.data();
  double* __restrict__ W = dvdt.data();
  const double h = deltat;
  const double h2 = deltat*0.5;
  const double h6 = deltat/6.0;
  const double fab = factorab;
  const double ab = alphaxbeta;
  for( size_type i=0; i<nodes; i++ ) {
    const double p = np[i];
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
  }
}

void Dendrite::output( Output& output ) const {
  output("Dendrite",index+1,"V",v);
}
