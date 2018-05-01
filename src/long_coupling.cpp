/** @file long_coupling.cpp
  @brief A brief, one sentence description.

  This class provides support for spatially nonuniform connectivity. 

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "long_coupling.h" // LongCoupling;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <cmath>     // std::pow;
#include <iostream>  // std::cerr; std::endl;
#include <vector>    // std::vector;
using std::cerr;
using std::endl;
using std::vector;

void LongCoupling::init( Configf& configf ) {
  configf.next("nu");
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    n2d.resize(nodes);
    for( size_type i=0; i<nodes; i++ ) {
      n2d[i].resize(nodes,temp[0]);
    }
    pos = (temp[0]>0)?1:-1;
  } else if( temp.size() == size_t(pow(nodes,2)) ) {
    n2d.resize(nodes);
    for( size_type i=0; i<nodes; i++ ) {
      n2d[i].resize(nodes);
      for( size_type j=0; j<nodes; j++ ) {
        n2d[i][j] = temp[i+j*nodes];
      }
    }
    pos = (temp[0]>0)?1:-1;
  } else {
    cerr<<"ERROR:: BAD INPUT ARGUMENT SIZE: Coupling: Matrix - must be either a single value of nu or must be specifically a matrix of Nodes x Nodes."<<endl;
    exit(EXIT_FAILURE);
  }
}

LongCoupling::LongCoupling( size_type nodes, double deltat, size_type index,
                        const Propagator& prepropag, const Population& postpop )
  : Coupling(nodes,deltat,index,prepropag,postpop) {
}

LongCoupling::~LongCoupling() = default;

void LongCoupling::step() {
  for( size_type j=0; j<nodes; j++ ) {
    P[j] = 0;
    for( size_type i=0; i<nodes; i++ ) {
      P[j] += n2d[i][j]*prepropag[i];
    }
  }
}

void LongCoupling::output( Output& output ) const {
  // outputting a 2D matrix rather than a vector:
  // for each "to", output all "from"
  for( size_type i=0; i<nodes; i++ ) {
    output("C.Matrix",index+1,label("nu.",i+1),n2d[i]);
  }
}
