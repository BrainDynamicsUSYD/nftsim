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
#include <cmath>     // std::floor; std::lround;
#include <cstdlib>   // std::strtod;
#include <iomanip>   // std::setprecision;
#include <iostream>  // std::cerr; std::endl;
#include <limits>    // std::numeric_limits::max_digits10
#include <sstream>   // std::ostringstream;
#include <string>    // std::string;
#include <vector>    // std::vector;
using std::cerr;
using std::endl;
using std::lround;
using std::setprecision;
using std::string;
using std::vector;

namespace {
/// The shortest decimal form of x that reads back as exactly x, eg "1.33"
/// rather than "1.3300000000000001".
string shortest( double x ) {
  const int max_digits = std::numeric_limits<double>::max_digits10;
  std::ostringstream out;
  for( int digits=1; digits<=max_digits; digits++ ) {
    out.str("");
    out << setprecision(digits) << x;
    if( digits == max_digits || std::strtod(out.str().c_str(), nullptr) == x ) {
      break;
    }
  }
  return out.str();
}

/// Describes how a Tau that is not a whole number of steps is rounded, eg
/// "Tau: 1.33 is rounded to 10895 steps, ie Tau: 1.3299560546875".
string rounding( double tau, vector<double>::size_type steps, double deltat ) {
  std::ostringstream out;
  out << "Tau: " << shortest(tau) << " is rounded to " << steps
      << (steps == 1 ? " step" : " steps") << ", ie Tau: "
      << shortest(static_cast<double>(steps)*deltat);
  return out.str();
}
} // namespace

/// Initialises Tau::m by reading from a `.conf` file, setting Tau::max accordingly.
void Tau::init( Configf& configf ) {
  vector<double> temp = configf.numbers();
  if( std::any_of(temp.begin(), temp.end(), [](double x){return x < 0.0;}) ) {
    cerr << "Tau must be non-negative." << endl;
    exit(EXIT_FAILURE);
  }
  if( interpolate ) {
    initFractional(temp);
    return;
  }
  // Round (not truncate) to the nearest whole number of time-steps: truncation
  // lost a step whenever tau/deltat fell just below an integer, eg, 3e-4/1e-4.
  if( temp.size() == 1 ) {
    m[0] = static_cast<size_type>(lround(temp[0]/deltat));
    max = m[0];
    if( !is_step_multiple(temp[0], deltat) ) {
      cerr << "WARNING: Value of Tau not divisible by Deltat!\n"
           << "  It is recommended that Tau be specified as an exact integer\n"
           << "  multiple of Deltat. Your simulation will be run using:\n"
           << "    " << rounding(temp[0], m[0], deltat) << endl;
    }
  } else if( temp.size() == nodes ) {
    m.resize(nodes);
    for( size_type i=0; i<nodes; i++ ) {
      m[i] = static_cast<size_type>(lround(temp[i]/deltat));
      if( m[i]>max ) {
        max = m[i];
      }
    }
    // Warn if any provided Tau values are not exact integer multiples of
    // deltat, showing how the first few of them are rounded.
    const size_type max_shown = 3;
    vector<size_type> failed; // indices of the values that are not multiples
    for( size_type i=0; i<nodes; i++ ) {
      if( !is_step_multiple(temp[i], deltat) ) {
        failed.push_back(i);
      }
    }
    if( !failed.empty() ) {
      cerr << "WARNING: " << failed.size() << " of the " << nodes
           << " values of Tau are not divisible by Deltat!\n"
           << "  It is recommended that Tau be specified as an exact integer\n"
           << "  multiple of Deltat. Your simulation will be run using:\n";
      for( size_type k=0; k<failed.size() && k<max_shown; k++ ) {
        const size_type i = failed[k];
        cerr << "    Node " << i+1 << ": " << rounding(temp[i], m[i], deltat) << "\n";
      }
      if( failed.size() > max_shown ) {
        cerr << "    ... and " << failed.size()-max_shown << " more.\n";
      }
    }
  } else {
    cerr << "The number of Tau values has to be one or the same as the number of nodes." << endl;
    exit(EXIT_FAILURE);
  }
}

/**
  @brief Splits each delay into whole steps m and a fraction f, tau = (m+f)*deltat.

  Used with `Integration: SecondOrder`. Population::Q() then interpolates
  linearly between the firing rates stored m and m+1 steps back, which is
  second order in deltat, rather than rounding tau to the nearest step (an
  O(deltat) error in the delay). Delays within 1e-6 of a step of a whole
  number of steps are treated as whole (no interpolation).
*/
void Tau::initFractional( const vector<double>& temp ) {
  if( temp.size() != 1 && temp.size() != nodes ) {
    cerr << "The number of Tau values has to be one or the same as the number of nodes." << endl;
    exit(EXIT_FAILURE);
  }
  m.assign(temp.size(), 0);
  frac.assign(temp.size(), 0.0);
  max = 0;
  bool fractional = false;
  for( size_type i=0; i<temp.size(); i++ ) {
    if( is_step_multiple(temp[i], deltat) ) {
      m[i] = static_cast<size_type>(lround(temp[i]/deltat));
    } else {
      const double steps = temp[i]/deltat;
      m[i] = static_cast<size_type>(std::floor(steps));
      frac[i] = steps - static_cast<double>(m[i]);
      fractional = true;
    }
    // A fractional delay also reads the firing rate m+1 steps back.
    const size_type needed = m[i] + (frac[i] > 0.0 ? 1 : 0);
    if( needed > max ) {
      max = needed;
    }
  }
  if( !fractional ) {
    frac.clear();
  }
}

Tau::Tau( size_type nodes, double deltat, size_type index )
  : NF(nodes,deltat,index), max(0), m(1,0), delayed(nodes) {
}

Tau::~Tau() = default;

void Tau::step() {
}
