/** @file tau.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "tau.h"        // Tau;

// Other nftsim headers
#include "configf.h"    // Configf;

// C++ standard library headers
#include <cmath>    // std::remainder;
#include <iostream> // std::cerr; std::endl;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::remainder;
using std::vector;

void Tau::init( Configf& configf ) {
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    if( remainder(temp[0], deltat) != 0.0 ) {
      cerr<<"WARNING: Value of tau not divisible by Deltat!\n";
      cerr<<"    It is STRONGLY RECOMMENDED that any times that you specify\n";
      cerr<<"    in your configuration files be exact integral multiples of\n";
      cerr<<"    Deltat. Due to the finite precision of floating-point numbers,\n";
      cerr<<"    exact multiples are only possible for machine representable\n";
      cerr<<"    values of Deltat. A simple means of achieving this is to use\n";
      cerr<<"    powers of two for Deltat (eg. 2^-13 == 1.220703125e-04)."<<endl;
      //exit(EXIT_FAILURE);
    }
    m[0] = static_cast<size_type>(temp[0]/deltat);
    max = m[0];
  } else if( temp.size() == nodes ) {
    if( remainder(temp[0], deltat) != 0.0 ) {
      cerr<<"WARNING: Value of tau not divisible by Deltat!\n";
      cerr<<"    It is STRONGLY RECOMMENDED that any times that you specify\n";
      cerr<<"    in your configuration files be exact integral multiples of\n";
      cerr<<"    Deltat. Due to the finite precision of floating-point numbers,\n";
      cerr<<"    exact multiples are only possible for machine representable\n";
      cerr<<"    values of Deltat. A simple means of achieving this is to use\n";
      cerr<<"    powers of two for Deltat (eg. 2^-13 == 1.220703125e-04)."<<endl;
      //exit(EXIT_FAILURE);
    }
    m.resize(nodes);
    for( size_type i=0; i<nodes; i++ ) {
      m[i] = static_cast<size_type>(temp[i]/deltat);
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
