/** @file nf.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_NF_H
#define NEUROFIELD_SRC_NF_H

// Other neurofield headers
#include "configf.h"   // Configf;
//#include "dumpf.h"     // Dumpf;
#include "output.h"    // Output;

// C++ standard library headers
#include <vector>    // std::vector;

class NF {
  NF(void);    // no default constructor allowed
 protected:
  typedef std::vector<double>::size_type size_type;
  virtual void init( Configf& configf ) = 0;
  //virtual void restart( Restartf& restartf ) = 0;
  //virtual void dump( Dumpf& dumpf ) const = 0;
  NF( size_type nodes, double deltat, size_type index );
  size_type nodes;  ///< number of nodes in simulation
  double deltat;    ///< time increment per timestep
  size_type index;  ///< object index within the population/connection model
 public:
  friend Configf&  operator>> ( Configf& configf, NF& nf );
  //friend Restartf& operator>> ( Restartf& restartf, NF& nf );
  //friend Dumpf&    operator<< ( Dumpf& dumpf, const NF& nf );
  virtual void step(void) = 0;
  virtual void output( Output&  /*unused*/) const;

  virtual ~NF(void);
};

#endif //NEUROFIELD_SRC_NF_H
