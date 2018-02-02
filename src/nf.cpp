/** @file nf.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

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
