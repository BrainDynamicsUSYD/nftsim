/** @file coupling_ramp.cpp
  @brief Defines CouplingRamp (`Coupling N: Ramp`), a coupling whose nu is piecewise linear in time.

  The configuration gives `nu:` (values of nu) and `timepoints:` (the
  strictly increasing times [s] at which nu takes those values). nu is
  nu[0] from t = 0 to timepoints[0], interpolated linearly in between, and
  held at its last value after the last time point.

  @author Paula Sanz-Leon, Romesh Abeysuriya
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
  if( nbp == 0 ) {
    cerr<<" The nu and timepoints vectors of CouplingRamp must not be empty." <<endl;
    exit(EXIT_FAILURE);
  }
  for( size_type i=1; i<nbp; i++ ) {
    if( !(tempt[i] > tempt[i-1]) ) {
      cerr<<" The timepoints of CouplingRamp must be strictly increasing." <<endl;
      exit(EXIT_FAILURE);
    }
  }
  tpts = tempt;
  nupts = tempn;

  // Assume that at t=0, nu=nu[0], ie, the segment between t=0 and timepoints[0] is constant.
  nu.assign(nodes, tempn[0]);
  pos = (tempn[0]>0)?1:-1;

  // The propagators are initialised before the couplings, so phi holds its initial value.
  for( size_type i=0; i<nodes; i++ ) {
    P[i] = nu[i]*prepropag[i];
  }

  steps = 0;
  time = 0.0;
}

void CouplingRamp::step() {
  // Compute time from an integer step count to avoid accumulating rounding error.
  steps++;
  time = static_cast<double>(steps)*deltat;
  // Piecewise-linear interpolation of nupts over tpts, constant outside [tpts[0], tpts[nbp-1]].
  double nu_now;
  if( time <= tpts[0] ) {
    nu_now = nupts[0];
  } else if( time >= tpts[nbp-1] ) {
    nu_now = nupts[nbp-1];
  } else {
    size_type j = 1;
    while( time > tpts[j] ) {
      j++;
    }
    nu_now = nupts[j-1] + (nupts[j]-nupts[j-1])*(time-tpts[j-1])/(tpts[j]-tpts[j-1]);
  }
  for( size_type i=0; i<nodes; i++ ) {
    nu[i] = nu_now;
  }
  Coupling::step();
}

CouplingRamp::CouplingRamp( size_type nodes, double deltat, size_type index,
                        const Propagator& prepropag, const Population& postpop )
  : Coupling(nodes,deltat,index,prepropag,postpop) {
}

CouplingRamp::~CouplingRamp() = default;
