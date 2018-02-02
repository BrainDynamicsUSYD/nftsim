/** @file bcmlong.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_BCMLONG_H
#define NFTSIM_SRC_BCMLONG_H

// Other nftsim headers
#include "bcm.h"        // BCM;
#include "configf.h"    // Configf;
#include "de.h"         // RK4;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class BCMLong : public BCM {
 protected:
  void init( Configf& configf ) override;

  std::vector<BCMDE*> de_2d;
  std::vector<RK4*> rk4_2d;

 public:
  BCMLong() = delete;               // No default constructor allowed.
  BCMLong(const BCMLong&) = delete; // No copy constructor allowed.

  BCMLong( size_type nodes, double deltat, size_type index,
           const Propagator& prepropag, const Population& postpop );
  ~BCMLong() override;
  void step() override;
  void output( Output& output ) const override;
};

#endif //NFTSIM_SRC_BCMLONG_H
