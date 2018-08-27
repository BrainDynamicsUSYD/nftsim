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
#include "output.h"     // Output;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <cmath>    // std::exp;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::exp;
using std::string;
using std::vector;

/// Initialise FiringResponse, reading function and parameters to use from file.
void FiringResponse::init( Configf& configf ) {
  configf.param("Function", mode);
  if( mode == "Sigmoid" ) {
    configf.param("Theta",theta);
    configf.param("Sigma",sigma);
    configf.param("Qmax",Q_max);
  } else if( mode == "Linear") { // Equation is q = av + b;
    configf.param("a",a);
    configf.param("b",b);
  } else if( mode == "Quadratic") { // Equation is q = av^2 + b^v + c;
    configf.param("a",a);
    configf.param("b",b);
    configf.param("c",c);
  } else if( mode == "Cubic") { // Equation is q = av^2 + b^v + c;
    configf.param("a",a);
    configf.param("b",b);
    configf.param("c",c);
    configf.param("d",d);
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
  for( size_type i=0; i<nodes; i++ ) {
    v[i] = 0;
  }
  for( Array<Dendrite>::size_type  i=0; i<dendrites.size(); i++ ) {
    for( size_type j=0; j<nodes; j++ ) {
      v[j] += dendrites[i]->V()[j];
    }
  }
}

void FiringResponse::add2Dendrite( size_type index, const Propagator& prepropag,
                              const Coupling& precouple, Configf& configf ) {
  string temp(configf.find( label("Dendrite ",index+1)+":" ));
  dendrite_index.push_back(index);
  // PUT YOUR DENDRITE HERE
  if( temp == "Ramp" ) {
    dendrites.add( new DendriteRamp(nodes, deltat, index, prepropag, precouple) );
  } else if ( temp == "Integral" ) {
    dendrites.add( new DendriteIntegral(nodes, deltat, index, prepropag, precouple) );
  // END PUT YOUR DENDRITE HERE
  } else {
    dendrites.add( new Dendrite(nodes, deltat, index, prepropag, precouple) );
  }
}

void FiringResponse::fire( vector<double>& Q ) const {
  if(mode == "Sigmoid") {
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = Q_max/( 1.0 + exp( -(v[i]-theta)/sigma ) );
    }
  } else if (mode == "Linear") {
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = v[i]*a +b;
    }
  } else if (mode == "Quadratic") {
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = v[i]*v[i]*a + v[i]*b +c;
    }
  } else if (mode == "Cubic") {
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = v[i]*v[i]*v[i]*a + v[i]*v[i]*b + v[i]*c +d;
    }
  }
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
