/**
 * @file coupling_ramp.cpp
 * Produce piecewise linear segments to vary synaptic couplings nu **over time**.
 * Assumes that at t=0, nu=nu[0], ie, the segment between t=0 and timepoints[0] is constant.
 *
 * @author Paula Sanz-Leon , Romesh Abeysuriya
 *
 */

/**
   Reads from the configuration file
   + @param[in]  nu: vector with the values of nu at specific time points.
   + @param[in]  timepoints: vector of specific time points corresponding to the nu.
*/

// Main module header
#include "coupling_ramp.h" // CouplingRamp;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <iostream> // std::cerr; std::endl;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::vector;

void CouplingRamp::init( Configf& configf ) {
  // Read values of nu
  vector<double> tempn;
  configf.next("nu");
  tempn = configf.numbers();
  // Read timepoints at which nu should change
  vector<double> tempt;
  configf.next("timepoints");
  tempt = configf.numbers();

  // Check that both vectors are of the same length.
  if( tempt.size() != tempn.size()) {
    cerr<<" The provided nu and timepoints vectors are not the same length." <<endl;
    exit(EXIT_FAILURE);
  }

  nbp = tempt.size(); // number of breakpoints (equivalently tempn.size();)
  ndnu = nbp - static_cast<size_type>(1); // number of delta nu.

  deltanu.reserve(ndnu); // Reserve memory to avoid growing inside loop.
  for ( size_type i=0; i<ndnu; i++ ) {
    deltanu[i] = deltat*((tempn[i+1]-tempn[i])/(tempt[i+1]-tempt[i]));
  }
  // Assume that at t=0, nu=nu[0], ie, the segment between t=0 and timepoints[0] is constant.
  nu.assign(nodes, tempn[0]);
  pos = (tempn[0]>0)?1:-1;

  for( size_type i=0; i<nodes; i++ ) {
    P[i] = nu[i]*prepropag.phiinit(configf);
  }

  time = 0.0;
  for( size_type i=0; i<nbp; i++ ) {
    tpts.push_back(tempt[i]);
  }
}

void CouplingRamp::step() {
  time += deltat;
  for ( size_type j=1; j<nbp; j++ ) {
    if( time >= tpts[j-1] && time < tpts[j] ) {
      for( size_type i=0; i<nodes; i++ ) {
        nu[i] += deltanu[j-1];
      }
    }
  }
  Coupling::step();
}

CouplingRamp::CouplingRamp( size_type nodes, double deltat, size_type index,
                        const Propagator& prepropag, const Population& postpop )
  : Coupling(nodes,deltat,index,prepropag,postpop) {
}

CouplingRamp::~CouplingRamp() = default;
