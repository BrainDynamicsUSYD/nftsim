/**
 * @file coupling_ramp.cpp
 * Produce piecewise linear segments to vary synaptic couplings nu **over time**.
 * Assumes that at t=0, nu=nus[0], ie, the segment between t=0 and timepoints[0] is constant.
 *
 * @author Paula Sanz-Leon , Romesh Abeysuriya
 *
 */

/**
   Reads from the configuration file
   + @param[in]     nus, timepoints: vector with the values of nus at specific time points specified in vector timepoints.
   + @param[in]     pairs   : total number of pairs of (nu, time) to define the segments
*/
#include <iostream>
using std::cerr;
using std::endl;
using std::cout;
#include "coupling_ramp.h"

void CouplingRamp::init( Configf& configf ) {

  // Number of different nus and timepoints
  configf.param("pairs", pairs);
  // Read values of nus
  vector<double> tempn;
  configf.next("nus");
  tempn = configf.numbers();
  // Read timepoints at which nu should change
  vector<double> tempt;
  configf.next("timepoints");
  tempt = configf.numbers();

  double temp;

  // Check that both vectors are of length pairs
  if( tempt.size() != tempn.size()) {
    cerr<<" The lengths of nus and timepoints are not equal." <<endl;
    exit(EXIT_FAILURE);
  }

  if( (tempt.size() != pairs) || (tempn.size() != pairs)) {
    cerr<<"The length of either *nus* or *timepoints* does not match the number specified in *pairs*" <<endl;
    exit(EXIT_FAILURE);
  }

  for ( vector<double>::size_type i=0; i<=pairs; i++ ) {
    temp = deltat*((tempn[i+1]-tempn[i])/(tempt[i+1]-tempt[i]));
    deltanu.push_back(temp);
  }
  // Assume that at t=0, nu=nus[0], ie, the segment between t=0 and timepoints[0] is constant.
  n.clear();
  n.resize(nodes,tempn[0]);
  pos = (tempn[0]>0)?1:-1;

  for( size_type i=0; i<nodes; i++ ) {
    P[i] = n[i]*prepropag.phiinit(configf);
  }

  time = 0;
  for(vector<double>::size_type i=0; i<pairs; i++) {
    tpts.push_back(tempt[i]);
  }
}

void CouplingRamp::step() {
  time += deltat;
  for ( vector<double>::size_type j=1; j<pairs; j++ ) {
    if( time >= tpts[j-1] && time < tpts[j] ) {
      for( size_type i=0; i<nodes; i++ ) {
        n[i] += deltanu[j-1];
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
