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
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <cmath>    // std::exp;
#include <cstdlib>  // std::exit; EXIT_FAILURE; std::getenv;
#include <iostream> // std::cerr; std::endl;
#include <string>   // std::string;
#include <vector>   // std::vector;
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

namespace {
/// Merging is on unless NFTSIM_MERGE_DENDRITES is 0, off, no or false.
/// NFTSIM_MERGE_DENDRITES=verbose also reports the merged groups on stderr.
bool mergingFromEnvironment() {
  const char* env = std::getenv("NFTSIM_MERGE_DENDRITES");
  if( env == nullptr ) {
    return true;
  }
  const string value(env);
  return !( value == "0" || value == "off" || value == "no" || value == "false" );
}
bool mergingVerbose() {
  const char* env = std::getenv("NFTSIM_MERGE_DENDRITES");
  return env != nullptr && string(env) == "verbose";
}
} // namespace

FiringResponse::FiringResponse( size_type nodes, double deltat, size_type index )
  : NF(nodes,deltat,index), v(nodes), merge_dendrites(mergingFromEnvironment()) {
}

FiringResponse::~FiringResponse() = default;

/// Step through dendrites, then sum up soma potentials.
void FiringResponse::step() {
  if( !merge_done ) {
    mergeDendrites();
  }
  for( Dendrite* dendrite : stepped ) {
    dendrite->step();
  }
  for( size_type i=0; i<nodes; i++ ) {
    v[i] = 0;
  }
  for( const Dendrite* dendrite : stepped ) {
    const vector<double>& dv = dendrite->V();
    for( size_type j=0; j<nodes; j++ ) {
      v[j] += dv[j];
    }
  }
}

/**
  @brief Merges dendrites whose summed V obeys the dendritic equation exactly.

  The dendritic equation (dendrite.h) is linear with constant coefficients, so
  if dendrites b1, b2, ... of this population have the same alpha and beta and
  the same linear time-invariant type (Dendrite or DendriteIntegral, see
  Dendrite::mergeable()), V_b1 + V_b2 + ... obeys the same equation driven by
  nu_b1 phi_b1 + nu_b2 phi_b2 + ..., and each step of RK4 or of the exact
  integral (either scheme) is a linear map of the state and the inputs. So the
  group is integrated as one dendrite, its first member, started from the sums
  of their V and dV/dt (and, for DendriteIntegral, of their previous inputs).
  The soma potential then changes only by round-off, from the changed order of
  summation. This cuts the corticothalamic model's 11 dendrite integrations
  per node per step to 4.

  A dendrite whose own V is output (`Dendrite:` in the Output section) is not
  merged, so its V stays exact; the rest of its group is still merged.
  Merging is skipped if allowMerging(false) was called or the environment
  variable NFTSIM_MERGE_DENDRITES is 0, off, no or false.
*/
void FiringResponse::mergeDendrites() {
  merge_done = true;
  stepped.clear();
  vector<Array<Dendrite>::size_type> stepped_index; // indices into dendrites
  vector<vector<Array<Dendrite>::size_type>> groups; // for the verbose report
  for( Array<Dendrite>::size_type i=0; i<dendrites.size(); i++ ) {
    Dendrite* dendrite = dendrites[i];
    bool merged = false;
    if( merge_dendrites && !dendrite_output[i] ) {
      for( vector<Dendrite*>::size_type k=0; k<stepped.size(); k++ ) {
        if( !dendrite_output[stepped_index[k]] && stepped[k]->canMerge(*dendrite) ) {
          stepped[k]->merge(*dendrite);
          groups[k].push_back(i);
          merged = true;
          break;
        }
      }
    }
    if( !merged ) {
      stepped.push_back(dendrite);
      stepped_index.push_back(i);
      groups.push_back(vector<Array<Dendrite>::size_type>(1, i));
    }
  }
  if( mergingVerbose() ) {
    for( const auto& g : groups ) {
      if( g.size() > 1 ) {
        std::cerr << "Population " << index+1 << ": merged dendrites";
        for( auto i : g ) {
          std::cerr << " " << dendrite_index[i]+1;
        }
        std::cerr << std::endl;
      }
    }
  }
}

Array<Dendrite>::size_type FiringResponse::steppedDendrites() const {
  return merge_done ? stepped.size() : dendrites.size();
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
  dendrite_output.push_back(false);
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
  switch( function ) {
  case Function::Sigmoid:
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = Q_max/( 1.0 + exp( -(v[i]-theta)/sigma ) );
    }
    break;
  case Function::Linear:
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = v[i]*a +b;
    }
    break;
  case Function::Quadratic:
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = v[i]*v[i]*a + v[i]*b +c;
    }
    break;
  case Function::Cubic:
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = v[i]*v[i]*v[i]*a + v[i]*v[i]*b + v[i]*c +d;
    }
    break;
  case Function::None:
    break;
  }
}

void FiringResponse::output( Output& output ) const {
  output("Pop",index+1,"V",v);
}

void FiringResponse::outputDendrite( size_type index, Output& output ) const {
  for( Array<Dendrite>::size_type  i=0; i<dendrites.size(); i++ ) {
    if( dendrite_index[i] == index ) {
      if( merge_done ) { // too late to keep it out of a merged group
        std::cerr << "Dendrite " << index+1 << " was requested for output after"
                  << " the dendrites were merged." << std::endl;
        exit(EXIT_FAILURE);
      }
      dendrite_output[i] = true; // keep it out of mergeDendrites()
      dendrites[i]->output(output);
    }
  }
}
