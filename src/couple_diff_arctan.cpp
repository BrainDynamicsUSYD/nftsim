/** @file couple.cpp
  @brief A new Couple class, where nu_ab follows a smooth function. 

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
#include "couple_diff_arctan.h"


void Couple_diff_arctan::init( Configf& configf ) {
  //read initial ramp conditions from config file
  configf.param("nu_min",nu_min);
  configf.param("nu_max",nu_max);
  configf.param("delt",delt);
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

  //initialize ramp
  for( int i=0; i<time_tot; i++ ) {
      ramp = (atan((i-t_half_up)/delt)-atan((i-t_half_down)/delt));
      deltanu.push_back(ramp);
  }
  //min and max ramp values
  ramp_min=*std::min_element(deltanu.begin(), deltanu.end());
  ramp_max=*std::max_element(deltanu.begin(), deltanu.end());
  
  n.clear(); n.resize(nodes,nu_min);
  pos = (nu_min>0)?1:-1;

  for( int i=0; i<nodes; i++) {
    P[i] = n[i]*prepropag.phiinit(configf);
  }

  //initial time
  time = 0; 
}

void Couple_diff_arctan::step(void) {
  //ramping of nu_ab (nu_ab = n)
  time += deltat;
  for( int i=0; i<nodes; i++ ) {
    n[i]= nu_min + (nu_max-nu_min)*(((atan((time-t_half_up)/delt)-atan((time-t_half_down)/delt))-ramp_min)/(ramp_max-ramp_min));
  }
  Couple::step();
}


Couple_diff_arctan::Couple_diff_arctan( int nodes, double deltat, int index,
                const Propag& prepropag, const Population& postpop, double tempf )
  : Couple(nodes,deltat,index,prepropag,postpop) {
  // total simulation time is stored in tempf (defined in solver.cpp line 73)  
  time_tot = tempf;
}

Couple_diff_arctan::~Couple_diff_arctan(void) {
}
