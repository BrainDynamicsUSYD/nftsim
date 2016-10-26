/** @file coupling.cpp
  @brief A new Coupling class, where nu_ab follows a smooth function.

  The smooth variation of nu_ab is defined by the difference of
  two arctangent functions.


  @author Farah Deeba, Paula Sanz-Leon, Sahand Assadzadeh
*/

#include<iostream>
#include<vector>
#include<algorithm>
using std::cerr;
using std::endl;
using std::cout;
#include "coupling_diff_arctan.h"


void CouplingDiffArctan::init( Configf& configf ) {
  //read initial ramp conditions from config file
  configf.param("nu_min",nu_min);
  configf.param("nu_max",nu_max);
  configf.param("delta",delta);
  configf.param("t_half_up",t_half_up);
  configf.param("t_half_down",t_half_down);
  if(t_half_up > t_half_down) {
    cerr<<"t_half_up must be less than t_half_down" <<endl;
  } else if(t_half_up == t_half_down) {
    cerr<<"t_half_up must be less than t_half_down" <<endl;
    exit (EXIT_FAILURE);
  }

  //size of time vector:
  time_int = time_tot/deltat;

  //pre-compute the profile
  for( int i=0; i<time_tot; i++ ) {
    diff_atan = (atan((i-t_half_up)/delta)-atan((i-t_half_down)/delta));
    deltanu.push_back(diff_atan);
  }
  //min and max value -  standardise values between 0 and 1
  diff_atan_min=*std::min_element(deltanu.begin(), deltanu.end());
  diff_atan_max=*std::max_element(deltanu.begin(), deltanu.end());

  n.clear();
  n.resize(nodes, nu_min);
  pos = (nu_min>0)?1:-1;

  for( int i=0; i<nodes; i++) {
    P[i] = n[i]*prepropag.phiinit(configf);
  }

  //initial time
  time = 0;
}

void CouplingDiffArctan::step(void) {
  // Return the right value at each time point
  time += deltat;
  for( int i=0; i<nodes; i++ ) {
    n[i]= nu_min + (nu_max-nu_min)*(((atan((time-t_half_up)/delta)-atan((time-t_half_down)/delta))-diff_atan_min)/(diff_atan_max-diff_atan_min));
  }
  Coupling::step();
}


CouplingDiffArctan::CouplingDiffArctan( int nodes, double deltat, int index,
                                    const Propagator& prepropag, const Population& postpop, double tempf )
  : Coupling(nodes,deltat,index,prepropag,postpop) {
  // total simulation time is stored in tempf (defined in solver.cpp line 73)
  time_tot = tempf;
}

CouplingDiffArctan::~CouplingDiffArctan(void) {
}
