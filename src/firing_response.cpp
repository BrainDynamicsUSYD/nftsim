/** @file firing_response.cpp
  @brief Implements the FiringResponse class, for the soma response of neural populations.

  Each neural population is associated with a FiringResponse object which produces
  the soma response governed by a specified equation, for example Sigmoid:
  \f[
    S_{a}[V_a(\mathbf{r}, t), \theta_{a}(\mathbf{r},t)] = \frac{Q_{a}^{\text{max}}}
        {1 + \exp[-\{ V_a(\mathbf{r}, t) - \theta_a(\mathbf{r}, t)\}/\sigma'_a(\mathbf{r}, t)]}
  \f]

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "firing_response.h"  // FiringResponse;

// Other nftsim headers
#include "array.h"      // Array;
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "dendrite.h"   // Dendrite;
#include "dendrite_integral.h" // DendriteIntegral;
#include "dendriteramp.h"      // DendriteRamp;
#include "make_unique.h"       // nftsim::make_unique;
#include "output.h"     // Output;
#include "parallel.h"   // nftsim::par::range;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <cmath>    // std::exp;
#include <cstdlib>  // std::exit; EXIT_FAILURE;
#include <iostream> // std::cerr; std::endl;
#include <string>   // std::string;
#include <vector>   // std::vector;
#include <typeinfo> // typeid;
using std::exp;
using std::string;
using std::vector;

/// Initialise FiringResponse, reading function and parameters to use from file.
void FiringResponse::init( Configf& configf ) {
  string mode;
  configf.param("Function", mode);
  if( mode == "Sigmoid" ) {
    function = Function::Sigmoid;
    configf.param("Theta",theta);
    configf.param("Sigma",sigma);
    configf.param("Qmax",Q_max);
  } else if( mode == "Linear") { // Equation is q = av + b;
    function = Function::Linear;
    configf.param("a",a);
    configf.param("b",b);
  } else if( mode == "Quadratic") { // Equation is q = av^2 + bv + c;
    function = Function::Quadratic;
    configf.param("a",a);
    configf.param("b",b);
    configf.param("c",c);
  } else if( mode == "Cubic") { // Equation is q = av^3 + bv^2 + cv + d;
    function = Function::Cubic;
    configf.param("a",a);
    configf.param("b",b);
    configf.param("c",c);
    configf.param("d",d);
  } else {
    std::cerr << "Firing response Function '" << mode << "' not recognised."
              << " Valid options are: Sigmoid, Linear, Quadratic, Cubic."
              << std::endl;
    exit(EXIT_FAILURE);
  }

  for( Array<Dendrite>::size_type i=0; i<dendrites.size(); i++ ) {
    configf>>*dendrites[i];
  }
}

FiringResponse::FiringResponse( size_type nodes, double deltat, size_type index )
  : NF(nodes,deltat,index), v(nodes) {
}

FiringResponse::~FiringResponse() = default;

/// Step through dendrites, then sum up soma potentials.
void FiringResponse::step() {
  dendrites.step();
  // Summed node by node in dendrite order, so the sum is the same as adding
  // each dendrite's V over all nodes in turn.
  const Array<Dendrite>::size_type nd = dendrites.size();
  const nftsim::par::Range nr = nftsim::par::range(nodes); // this thread's nodes
  for( size_type j=nr.begin; j<nr.end; j++ ) {
    double sum = 0;
    for( Array<Dendrite>::size_type i=0; i<nd; i++ ) {
      sum += dendrites[i]->V()[j];
    }
    v[j] = sum;
  }
}

void FiringResponse::initFromInputs() {
  for( Array<Dendrite>::size_type i=0; i<dendrites.size(); i++ ) {
    dendrites[i]->initFromInputs();
  }
}

void FiringResponse::add2Dendrite( size_type index, const Propagator& prepropag,
                              const Coupling& precouple, Configf& configf ) {
  string temp(configf.find( label("Dendrite ",index+1)+":" ));
  dendrite_index.push_back(index);
  // PUT YOUR DENDRITE HERE
  if( temp == "Ramp" ) {
    dendrites.add( nftsim::make_unique<DendriteRamp>(nodes, deltat, index, prepropag, precouple) );
  } else if ( temp == "Integral" ) {
    dendrites.add( nftsim::make_unique<DendriteIntegral>(nodes, deltat, index, prepropag, precouple) );
  // END PUT YOUR DENDRITE HERE
  } else {
    dendrites.add( nftsim::make_unique<Dendrite>(nodes, deltat, index, prepropag, precouple) );
  }
}

void FiringResponse::useSecondOrder( bool second_order ) {
  for( Array<Dendrite>::size_type i=0; i<dendrites.size(); i++ ) {
    dendrites[i]->useSecondOrder(second_order);
  }
}

void FiringResponse::fire( vector<double>& Q ) const {
  const nftsim::par::Range nr = nftsim::par::range(nodes); // this thread's nodes
  switch( function ) {
  case Function::Sigmoid:
    for( size_type i=nr.begin; i<nr.end; i++ ) {
      Q[i] = Q_max/( 1.0 + exp( -(v[i]-theta)/sigma ) );
    }
    break;
  case Function::Linear:
    for( size_type i=nr.begin; i<nr.end; i++ ) {
      Q[i] = v[i]*a +b;
    }
    break;
  case Function::Quadratic:
    for( size_type i=nr.begin; i<nr.end; i++ ) {
      Q[i] = v[i]*v[i]*a + v[i]*b +c;
    }
    break;
  case Function::Cubic:
    for( size_type i=nr.begin; i<nr.end; i++ ) {
      Q[i] = v[i]*v[i]*v[i]*a + v[i]*v[i]*b + v[i]*c +d;
    }
    break;
  case Function::None:
    break;
  }
}

bool FiringResponse::steadyFiring( double v, double& q, double& dqdv ) const {
  switch( function ) {
  case Function::Sigmoid: {
    const double e = exp( -(v-theta)/sigma );
    const double f = 1.0/( 1.0 + e ); // 0 (not NaN) when e overflows
    q = Q_max/( 1.0 + e ); // as in fire()
    dqdv = Q_max*f*( 1.0 - f )/sigma;
    return true;
  }
  case Function::Linear:
    q = v*a +b;
    dqdv = a;
    return true;
  case Function::Quadratic:
    q = v*v*a + v*b +c;
    dqdv = 2.0*a*v + b;
    return true;
  case Function::Cubic:
    q = v*v*v*a + v*v*b + v*c +d;
    dqdv = 3.0*a*v*v + 2.0*b*v + c;
    return true;
  case Function::None:
    break;
  }
  return false;
}

void FiringResponse::output( Output& output ) const {
  output("Pop",index+1,"V",v);
}

void FiringResponse::outputDendrite( size_type index, Output& output ) const {
  for( Array<Dendrite>::size_type  i=0; i<dendrites.size(); i++ ) {
    if( dendrite_index[i] == index ) {
      dendrites[i]->output(output);
    }
  }
}

bool FiringResponse::nodeParallel() const {
  return typeid(*this) == typeid(FiringResponse);
}
