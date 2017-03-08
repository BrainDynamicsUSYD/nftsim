/** @file tau.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_TAU_H
#define NEUROFIELD_SRC_TAU_H

// Other neurofield headers
#include "configf.h"    // Configf;
#include "nf.h"         // NF;

// Forward declaration to break circular collaboration
class Population;

// C++ standard library headers
#include <vector> //std::vector;

class Tau : public NF {
  Tau(const Tau&);  // No copy constructor allowed.

  size_type max; ///< if tau is inhomogeneous, == biggest element
  std::vector<size_type> m; ///< tau values across nodes, size()==1 if homogeneous
 protected:
  void init( Configf& configf ) override;
 public:
  Tau( size_type nodes, double deltat, size_type index );
  ~Tau(void) override;
  void step(void) override;

  friend class Population;
};

#endif //NEUROFIELD_SRC_TAU_H
