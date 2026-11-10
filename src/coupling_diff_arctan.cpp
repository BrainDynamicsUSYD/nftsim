/** @file coupling_diff_arctan.cpp
  @brief Defines CouplingDiffArctan (`Coupling N: DiffArctan`), a coupling whose nu rises and falls smoothly.

  nu follows the difference of two arctangents,
  atan((t - t_half_up)/delta) - atan((t - t_half_down)/delta), scaled so
  that it spans [nu_min, nu_max] over the simulated time: it rises around
  t_half_up and falls around t_half_down, each over a time of about delta.

  @author Farah Deeba, Paula Sanz-Leon, Sahand Assadzadeh
*/

// Main module header
#include "coupling_diff_arctan.h" // CouplingDiffArctan;

// Other nftsim  headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <cmath>     // std::atan; std::lround;
#include <cstdlib>   // std::atof; std::exit; EXIT_FAILURE;
#include <iostream>  // std::cerr; std::endl;
using std::atan;
using std::lround;
using std::cerr;
using std::endl;

void CouplingDiffArctan::init( Configf& configf ) {
  //read initial ramp conditions from config file
  configf.param("nu_min",nu_min);
  configf.param("nu_max",nu_max);
  configf.param("delta",delta);
  configf.param("t_half_up",t_half_up);
  configf.param("t_half_down",t_half_down);
  if( t_half_up >= t_half_down ) {
    cerr<<"CouplingDiffArctan: t_half_up must be less than t_half_down." <<endl;
    exit(EXIT_FAILURE);
  }

  // Find the min and max of the profile over exactly the times at which
  // step() evaluates it (time = k*deltat, k = 1..nsteps), to standardise
  // nu between nu_min and nu_max.
  size_type nsteps = static_cast<size_type>(lround(time_tot/deltat));
  for( size_type k=1; k<=nsteps; k++ ) {
    double diff_atan = profile(static_cast<double>(k)*deltat);
    if( k == 1 || diff_atan < diff_atan_min ) {
      diff_atan_min = diff_atan;
    }
    if( k == 1 || diff_atan > diff_atan_max ) {
      diff_atan_max = diff_atan;
    }
  }
  if( !(diff_atan_max > diff_atan_min) ) {
    cerr<<"CouplingDiffArctan: the arctan profile is constant over the simulation time,"
        <<" so nu cannot be scaled between nu_min and nu_max. Check Time, t_half_up,"
        <<" t_half_down and delta." <<endl;
    exit(EXIT_FAILURE);
  }

  nu.clear();
  nu.resize(nodes, nu_min);
  pos = (nu_min>0)?1:-1;

  // The propagators are initialised before the couplings, so phi holds its initial value.
  for( size_type i=0; i<nodes; i++ ) {
    P[i] = nu[i]*prepropag[i];
  }

  //initial time
  steps = 0;
  time = 0;
}

double CouplingDiffArctan::nuinit( Configf& configf ) const {
  return atof(configf.lookup(label("Coupling ",index+1), "nu_min", true).c_str());
}

double CouplingDiffArctan::profile( double t ) const {
  return atan((t-t_half_up)/delta)-atan((t-t_half_down)/delta);
}

void CouplingDiffArctan::step() {
  // Return the right value at each time point
  steps++;
  time = static_cast<double>(steps)*deltat;
  double nu_now = nu_min + (nu_max-nu_min)*((profile(time)-diff_atan_min)/(diff_atan_max-diff_atan_min));
  for( size_type i=0; i<nodes; i++ ) {
    nu[i] = nu_now;
  }
  Coupling::step();
}


CouplingDiffArctan::CouplingDiffArctan( size_type nodes, double deltat, size_type index,
                                        const Propagator& prepropag, const Population& postpop,
                                        double tempf )
  : Coupling(nodes,deltat,index,prepropag,postpop) {
  // total simulation time is stored in tempf (defined in solver.cpp line 73)
  time_tot = tempf;
}

CouplingDiffArctan::~CouplingDiffArctan() {
}
