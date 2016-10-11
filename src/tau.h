/** @file tau.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_TAU_H
#define NEUROFIELD_SRC_TAU_H

#include"nf.h"
using std::vector;
class Population;

class Tau : public NF {
  Tau(Tau& ); // no copy constructor
  int max; ///< if tau is inhomogeneous, == biggest element
  vector<int> m; ///< tau values across nodes, size()==1 if homogeneous
 protected:
  void init( Configf& configf ) override;
 public:
  Tau( int nodes, double deltat, int index );
  ~Tau(void) override;
  void step(void) override;

  friend class Population;
};

#endif
