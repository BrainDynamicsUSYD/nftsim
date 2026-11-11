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
#include "restrict.h"   // NFTSIM_RESTRICT;

// C++ standard library headers
#include <functional> // std::equal_to;
#include <iostream> // std::endl; std::cerr;
#include <typeinfo> // typeid;
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
    // Provisional: initFromInputs() replaces this with the coupling's actual
    // initial nu*phi, which differs from nuinit()*phi for per-node nu lists
    // and for Matrix couplings (whose input at node j sums over all nodes).
    steady_v = true;
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
    own_input(&precouple.nuphi()), prepropag(prepropag), precouple(precouple) {
}

Dendrite::~Dendrite() = default;

void Dendrite::initFromInputs() {
  if( steady_v ) {
    v = precouple.nuphi();
    dvdt.assign(nodes, 0.0);
  }
}

bool Dendrite::mergeable() const {
  return typeid(*this) == typeid(Dendrite);
}

bool Dendrite::canMerge( const Dendrite& other ) const {
  const std::equal_to<double> same; // exact equality is intended
  return mergeable() && other.mergeable() && typeid(*this) == typeid(other)
         && same(alpha, other.alpha) && same(beta, other.beta)
         && nodes == other.nodes && same(deltat, other.deltat)
         && second_order == other.second_order && !started && !other.started;
}

void Dendrite::merge( const Dendrite& other ) {
  for( size_type i=0; i<nodes; i++ ) {
    v[i] += other.v[i];
    dvdt[i] += other.dvdt[i];
  }
  merged_inputs.push_back(&other.precouple.nuphi());
  merged_inputs.insert(merged_inputs.end(),
                       other.merged_inputs.begin(), other.merged_inputs.end());
  summed_np.resize(nodes);
}

const double* Dendrite::sumInputs() {
  const double* NFTSIM_RESTRICT np0 = own_input->data();
  const double* NFTSIM_RESTRICT np1 = merged_inputs[0]->data();
  double* NFTSIM_RESTRICT sum = summed_np.data();
  // One pass for the common groups of two and three dendrites.
  if( merged_inputs.size() == 1 ) {
    for( size_type i=0; i<nodes; i++ ) {
      sum[i] = np0[i] + np1[i];
    }
    return sum;
  }
  const double* NFTSIM_RESTRICT np2 = merged_inputs[1]->data();
  for( size_type i=0; i<nodes; i++ ) {
    sum[i] = (np0[i] + np1[i]) + np2[i];
  }
  for( std::vector<const std::vector<double>*>::size_type c=2; c<merged_inputs.size(); c++ ) {
    const double* NFTSIM_RESTRICT npc = merged_inputs[c]->data();
    for( size_type i=0; i<nodes; i++ ) {
      sum[i] += npc[i];
    }
  }
  return sum;
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
    const double* NFTSIM_RESTRICT np = input();
    startSecondOrder(np);
    double* NFTSIM_RESTRICT pp = prevnp.data();
    double* NFTSIM_RESTRICT V = v.data();
    double* NFTSIM_RESTRICT W = dvdt.data();
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
  const double* NFTSIM_RESTRICT np = input();
  double* NFTSIM_RESTRICT V = v.data();
  double* NFTSIM_RESTRICT W = dvdt.data();
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
