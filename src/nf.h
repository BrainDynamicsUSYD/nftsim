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
#include "output.h"    // Output;

// C++ standard library headers
#include <cmath>     // std::fabs; std::round;
#include <vector>    // std::vector;

/// True when time x is an integer number of time-steps deltat, to within
/// 1e-6 of a step. Used to decide whether to warn about times that will be
/// rounded to the nearest step (an exact floating-point test warns for almost
/// every decimal value of deltat).
inline bool is_step_multiple( double x, double deltat ) {
  const double steps = x/deltat;
  return std::fabs(steps - std::round(steps)) <= 1e-6;
}

class NF {
 protected:
  using size_type = std::vector<double>::size_type;
  virtual void init( Configf& configf ) = 0; ///< pure virtual method, all derived classes must implement this.
  NF( size_type nodes, double deltat, size_type index );
  size_type nodes;  ///< number of nodes in simulation
  double deltat;    ///< time increment per timestep
  size_type index;  ///< object index within the population/connection model
 public:
  NF() = delete;  ///< No default constructor allowed.

  friend Configf&  operator>> ( Configf& configf, NF& nf );
  virtual void step() = 0; ///< pure virtual method, all derived classes must implement this.
  virtual void output( Output&  /*unused*/) const;
  /// True if step() steps only the calling thread's nodes, par::range(nodes),
  /// so that threads can share the nodes (see parallel.h). Array::step() runs
  /// the step() of any other object on one thread.
  virtual bool nodeParallel() const { return false; }

  virtual ~NF();
};

#endif //NFTSIM_SRC_NF_H
