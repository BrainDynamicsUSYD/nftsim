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
#include <iomanip>  // std::setprecision;
#include <iostream> // std::cerr; std::endl;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::remainder;
using std::vector;

void Tau::init( Configf& configf ) {
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    m[0] = static_cast<size_type>(temp[0]/deltat);
    max = m[0];
    if( remainder(temp[0], deltat) != 0.0 ) {
      cerr<<"WARNING: Value of tau not divisible by Deltat!\n";
      cerr<<"    It is recommended that Tau be specified as an exact integer\n";
      cerr<<"    multiple of Deltat. Your configuration file requested:\n";
      cerr<<"        Tau: "<<std::setprecision(16)<<temp[0]<<"\n";
      cerr<<"    but the simulation will run using:\n";
      cerr<<"        Tau: "<<std::setprecision(16)<<m[0]*deltat<<endl;
      //exit(EXIT_FAILURE);
    }
  } else if( temp.size() == nodes ) {
    m.resize(nodes);
    for( size_type i=0; i<nodes; i++ ) {
      m[i] = static_cast<size_type>(temp[i]/deltat);
      if( m[i]>max ) {
        max = m[i];
      }
    }
    if( remainder(temp[0], deltat) != 0.0 ) {
      cerr<<"WARNING: Value of tau not divisible by Deltat!\n";
      cerr<<"    It is recommended that Tau be specified as an exact integer\n";
      cerr<<"    multiple of Deltat. For example, your configuration file requested:\n";
      cerr<<"        Tau[0]: "<<std::setprecision(16)<<temp[0]<<"\n";
      cerr<<"    but the simulation will run using:\n";
      cerr<<"        Tau[0]: "<<std::setprecision(16)<<m[0]*deltat<<endl;
      //exit(EXIT_FAILURE);
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
