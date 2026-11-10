/** @file propagator.cpp
  @brief Propagator computes the axonal propagation of instantaneous firing rate.

  The base Propagator (`Propagator N: Map`) passes on the presynaptic firing
  rate delayed by Tau, phi = Q(t - Tau), without spatial or temporal
  spreading. Harmonic, HarmonicIntegral, Wave and WaveLegacy derive from it.

  Every propagator starts from the initial phi given by `phi:` (Steady, one
  number, or one number per node; see readPhi()). For Map this is phi at
  t = 0 only, which the couplings use in the first step; from then on
  phi = Q(t - Tau).

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "propagator.h" // Propagator;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "output.h"     // Output;
#include "parallel.h"   // NFTSIM_PARALLEL_FOR;
#include "population.h" // Population;

// C++ standard library headers
#include <cstdlib>  // std::exit; EXIT_FAILURE;
#include <ios>      // std::streamsize;
#include <iostream> // std::cerr; std::endl;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

/** @brief Reads the optional `phi:` entry at the current stream position.

  `phi` is the initial axonal field, phi at t = 0. It is optional, and must be
  the first entry after the propagator type (`Propagator 1: Wave - phi: ...`).
  It may be `Steady` (the default when it is absent), which starts phi at the
  presynaptic population's initial firing rate Q at every node; a single
  number, used at every node; or one number per node (`Nodes` numbers), as
  for `Tau` and `nu`. Anything else is an error.
*/
vector<double> Propagator::readPhi( Configf& configf ) const {
  if( configf.next("phi") ) {
    const std::streamsize start = configf.tell();
    string word;
    configf >> word;
    if( word != "Steady" ) {
      configf.seek(start);
      vector<double> values = configf.numbers();
      if( values.size() == 1 ) {
        return vector<double>(nodes, values[0]);
      }
      if( values.size() == nodes ) {
        return values;
      }
      cerr << "Propagator " << index+1 << ": phi must be Steady, a single number,"
           << " or one number per node (Nodes = " << nodes << "), but ";
      if( values.empty() ) {
        cerr << "'" << word << "' was given." << endl;
      } else {
        cerr << values.size() << " numbers were given." << endl;
      }
      exit(EXIT_FAILURE);
    }
  }
  return vector<double>(nodes, prepop.Qinit(configf));
}

void Propagator::init( Configf& configf ) {
  Q = prepop.Qinit(configf);
  p = readPhi(configf);
  configf.optional("Tau",tau);
  prepop.growHistory(tau);

  // Range is either used when velocity is specified, or
  // not used but read for compatibility with Wave propagator.
  configf.optional("Range",range);
  if( !configf.optional("gamma",gamma) ) {
    if( configf.optional("velocity",velocity) ) {
      if( range <= 0.0 ) {
        std::cerr << "Propagator " << index+1 << ": velocity given without a "
                  << "positive Range (Range must precede velocity), so gamma "
                  << "= velocity/Range cannot be computed." << std::endl;
        exit(EXIT_FAILURE);
      }
      gamma = velocity/range;
    }
  }
}

Propagator::Propagator( size_type nodes, double deltat, size_type index, Population& prepop,
                Population& postpop, size_type longside, string  /*unused*/)
  : NF(nodes,deltat,index), prepop(prepop), postpop(postpop),
    tau(nodes,deltat,index), longside(longside), p(nodes) {
}

Propagator::~Propagator() = default;

void Propagator::step() {
  const vector<double>& Q = prepop.Q(tau);
  NFTSIM_PARALLEL_FOR
  for( size_type i=0; i<nodes; i++ ) {
    p[i] = Q[i];
  }
}

/** @brief Initial phi at every node, read from this propagator's section.

  This is the phi that init() sets (see readPhi()), but it can be called
  before init(): a dendrite whose `V` is `Steady` needs it while the
  populations are read, which is before the propagators are. The stream
  position is restored afterwards.
*/
vector<double> Propagator::phiinit( Configf& configf ) const {
  const std::streamsize position = configf.tell();
  const string section = label("Propagator ",index+1);
  configf.go2(section);
  configf.next(section);
  vector<double> phi0 = readPhi(configf);
  configf.clear();
  configf.seek(position);
  return phi0;
}

const vector<double>& Propagator::phi() const {
  return p;
}

void Propagator::output( Output& output ) const {
  output("Propagator",index+1,"phi",p);
}
