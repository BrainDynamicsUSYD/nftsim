/** @file coupling.cpp
  @brief Definition of the main class handling synaptic connections.

  This class assumes that the synaptic strengths are constant over space and
  time. The classes: CouplingRamp; CouplingDiffArctan; LongCoupling; and CaDP,
  which are derived from Coupling, implement temporally varying synaptic
  strengths or modulation by pre- or postsynaptic activity.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "coupling.h"   // Coupling;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <cstdlib>   // std::atof; std::exit; EXIT_FAILURE;
#include <iostream>  // std::cerr; std::endl;
#include <string>    // std::string;
#include <vector>    // std::vector;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

/// Read `.conf` file and initialise synaptic coupling strengths and weighted presynaptic inputs.
void Coupling::init( Configf& configf ) {
  configf.next("nu");
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    nu.clear();
    nu.resize(nodes,temp[0]);
    pos = (temp[0]>0)?1:-1;
  } else if( temp.size() == nodes ) {
    nu.clear();
    nu.resize(nodes);
    for( size_type i=0; i<nodes; i++ ) {
      nu[i] = temp[i];
    }
    pos = 0;
  } else {
    cerr<<"nu either has a homogeneous initial value or has one intial value per node."<<endl;
    exit(EXIT_FAILURE);
  }
  // The propagators are initialised before the couplings, so phi holds its initial value.
  for( size_type i=0; i<nodes; i++ ) {
    P[i] = nu[i]*prepropag[i];
  }
}

Coupling::Coupling( size_type nodes, double deltat, size_type index,
                const Propagator& prepropag, const Population& postpop )
  : NF(nodes,deltat,index), prepropag(prepropag), postpop(postpop), nu(nodes), P(nodes) {
}

Coupling::~Coupling() = default;

/// Calculate next value for synaptic-coupling-strength-weighted-presynaptic-inputs.
void Coupling::step() {
  for( size_type i=0; i<nodes; i++ ) {
    P[i] = nu[i]*prepropag[i];
  }
}

/** @brief Initial synaptic coupling strength, read from this coupling's section.

  Used (before the couplings are initialised) for a dendrite's steady-state
  initial potential. This is the first value of `nu:`, which Map, Matrix,
  Ramp, CaDP, BCM and BCM-Spatial couplings require; CouplingDiffArctan
  overrides it. The search is confined to this coupling's section; it used
  to run on to a later coupling's `nu:`.
*/
double Coupling::nuinit( Configf& configf ) const {
  return atof(configf.lookup(label("Coupling ",index+1), "nu", true).c_str());
}

void Coupling::output( Output& output ) const {
  output("Coupling",index+1,"nu",nu);
}

bool Coupling::excite() const {
  return pos==1;
}

const vector<double>& Coupling::nuphi() const {
  return P;
}
