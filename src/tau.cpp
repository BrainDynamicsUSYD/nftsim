/** @file tau.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// C++ standard library headers
#include <vector>   // std::vector;
using std::vector;

#include <iostream> // std::cerr; std::endl;
using std::cerr;
using std::endl;

#include <cmath> // std::remainder;
using std::remainder;

// Neurofield headers
#include "tau.h"

void Tau::init( Configf& configf ) {
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    if( remainder(temp[0],deltat) >deltat ) {
      cerr<<"Value of tau not divisible by Deltat!"<<endl;
      exit(EXIT_FAILURE);
    }
    m[0] = temp[0]/deltat;
    max = m[0];
  } else if( temp.size() == nodes ) {
    if( remainder(temp[0],deltat) >deltat ) {
      cerr<<"Value of tau not divisible by Deltat!"<<endl;
      exit(EXIT_FAILURE);
    }
    m.resize(nodes);
    for( size_type i=0; i<nodes; i++ ) {
      m[i] = size_type(temp[i]/deltat);
      if( m[i]>max ) {
        max = m[i];
      }
    }
  } else {
    cerr << "The number of Tau has to be one or the same as the number of nodes." << endl;
    exit(EXIT_FAILURE);
  }
}

Tau::Tau( size_type nodes, double deltat, size_type index )
  : NF(nodes,deltat,index), max(0), m(1,0) {
}

Tau::~Tau() = default;

void Tau::step() {
}
