/** @file tau.cpp
  @brief Definition of Tau, which handles the activity history.

  Tau objects are used to retrieve the appropriate delayed activity when the
  discrete time delay \f$\tau_{ab}\f$ is nonzero.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "tau.h"        // Tau;

// Other nftsim headers
#include "configf.h"    // Configf;

// C++ standard library headers
#include <algorithm> // std::any_of;
#include <cmath>     // std::remainder;
#include <iomanip>   // std::setprecision;
#include <iostream>  // std::cerr; std::endl; std::scientific;
#include <limits>    // std::numeric_limits::max_digits10
#include <vector>    // std::vector;
using std::cerr;
using std::endl;
using std::remainder;
using std::scientific;
using std::setprecision;
using std::vector;

/// Initialises Tau::m by reading from a `.conf` file, setting Tau::max accordingly.
void Tau::init( Configf& configf ) {
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    m[0] = static_cast<size_type>(temp[0]/deltat);
    max = m[0];
    if( remainder(temp[0], deltat) != 0.0 ) {
      int full_precision = std::numeric_limits<double>::max_digits10;
      cerr << "WARNING: Value of Tau not divisible by Deltat!\n"
           << "  It is recommended that Tau be specified as an exact integer\n"
           << "  multiple of Deltat. Your simulation will be run using:\n"
           << "    Tau: " << scientific << setprecision(full_precision)
                          << m[0]*deltat
           << endl;
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
    // Warn if any provided Tau values are not exact integer multiples of deltat
    if ( std::any_of(temp.begin(), temp.end(), [this](double x){return remainder(x, deltat) != 0.0;}) ) {
      //int full_precision = std::numeric_limits<double>::max_digits10;
      cerr << "WARNING: One or more values of Tau not divisible by Deltat!\n"
           << "  It is recommended that Tau be specified as an exact integer\n"
           << "  multiple of Deltat."
           << endl;
      //exit(EXIT_FAILURE);
    }
  } else {
    cerr << "The number of Tau values has to be one or the same as the number of nodes." << endl;
    exit(EXIT_FAILURE);
  }
}

Tau::Tau( size_type nodes, double deltat, size_type index )
  : NF(nodes,deltat,index), max(0), m(1,0) {
}

Tau::~Tau() = default;

void Tau::step() {
}
