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
#include <memory> // std::unique_ptr;
#include <vector> // std::vector;

class BCMLong : public BCM {
 protected:
  void init( Configf& configf ) override;

  /// One BCM state per presynaptic node i: (*de_2d[i])[k][j] is variable k
  /// of the connection from node i to node j. BCMLong passes no DE to CaDP.
  std::vector<std::unique_ptr<BCMDE>> de_2d;
  std::vector<std::unique_ptr<RK4>> rk4_2d; ///< Integrates de_2d[i].

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
