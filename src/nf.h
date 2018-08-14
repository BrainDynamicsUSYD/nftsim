/** @file nf.h
  @brief Declaration of the base class for components of a neural field model.

  The NF class serves as the base-class for most of the main NFTsim classes,
  including: Coupling; Dendrite; FiringResponse; Population; Propagator; Tau;
  Timeseries; and Solver classes; as well as Solver::CntMat and Solver::Outputs.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_NF_H
#define NFTSIM_SRC_NF_H

// Other nftsim headers
#include "configf.h"   // Configf;
//#include "dumpf.h"     // Dumpf;
#include "output.h"    // Output;

// C++ standard library headers
#include <vector>    // std::vector;

class NF {
 protected:
  using size_type = std::vector<double>::size_type;
  virtual void init( Configf& configf ) = 0; ///< pure virtual method, all derived classes must implement this.
  //virtual void restart( Restartf& restartf ) = 0;
  //virtual void dump( Dumpf& dumpf ) const = 0;
  NF( size_type nodes, double deltat, size_type index );
  size_type nodes;  ///< number of nodes in simulation
  double deltat;    ///< time increment per timestep
  size_type index;  ///< object index within the population/connection model
 public:
  NF() = delete;  ///< No default constructor allowed.

  friend Configf&  operator>> ( Configf& configf, NF& nf );
  //friend Restartf& operator>> ( Restartf& restartf, NF& nf );
  //friend Dumpf&    operator<< ( Dumpf& dumpf, const NF& nf );
  virtual void step() = 0; ///< pure virtual method, all derived classes must implement this.
  virtual void output( Output&  /*unused*/) const;

  virtual ~NF();
};

#endif //NFTSIM_SRC_NF_H
