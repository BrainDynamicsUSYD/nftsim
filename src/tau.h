/** @file tau.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_TAU_H
#define NFTSIM_SRC_TAU_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "nf.h"         // NF;

// Forward declaration to break circular collaboration
class Population;

// C++ standard library headers
#include <vector> //std::vector;

class Tau : public NF {
  size_type max; ///< if tau is inhomogeneous, == biggest element
  std::vector<size_type> m; ///< tau values across nodes, size()==1 if homogeneous
 protected:
  void init( Configf& configf ) override;
 public:
  Tau(const Tau&) = delete;  // No copy constructor allowed.

  Tau( size_type nodes, double deltat, size_type index );
  ~Tau() override;
  void step() override;

  friend class Population;
};

#endif //NFTSIM_SRC_TAU_H
