/** @file nf.cpp
  @brief Definition of the base class for components of a neural field model.

  The constructor NF::NF handles the initialisation of `nodes`, `deltat`, and
  `index` for NF derived classes. Most other methods are virtual, being
  implemented by the derived classes.

  Also implemented here, as a friend function, is Configf's extraction
  `operator>>` for reading configuration information from a `.conf` file into
  NF derived objects.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "nf.h"         // NF;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "output.h"     // Output;

Configf& operator>> ( Configf& configf, NF& nf ) {
  nf.init(configf);
  return configf;
}

/*Restartf& operator>> ( Restartf& restartf, NF& nf )
{
  //nf.init( restartf );
  nf.restart(restartf);
  return restartf;
}

Dumpf& operator<< ( Dumpf& dumpf, const NF& nf )
{
  nf.dump(dumpf);
  return dumpf;
}*/

NF::NF( size_type nodes, double deltat, size_type index )
  : nodes(nodes), deltat(deltat), index(index) {
}

void NF::output( Output&   /*unused*/) const {
}

NF::~NF() = default;
