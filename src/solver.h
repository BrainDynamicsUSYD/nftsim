/** @file solver.h
  @brief Declares the Solver class, a high level container for managing a simulation.

  The Solver class pulls together Population, Propagator, and Coupling classes.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_SOLVER_H
#define NFTSIM_SRC_SOLVER_H

// Other nftsim headers
#include "array.h"      // Array;
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "dumpf.h"      // Dumpf;
#include "nf.h"         // NF;
#include "output.h"     // Outlet;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <memory>   // std::unique_ptr;
#include <utility>  // std::move;
#include <vector>   // std::vector;

class Solver : public NF {
  Dumpf& dumpf;

  int steps = 0; ///< number of integration steps to perform

  struct CntMat : public NF {
    std::vector<double>::size_type npop=0; ///< number of populations
    std::vector<int>::size_type ncnt=0; ///< number of connections
    std::vector< std::vector<double> > raw; // 2D vector of raw connection matrix

    std::vector<std::vector<double>::size_type> pre;  ///< presynaptic connection index for each population
    std::vector<std::vector<double>::size_type> post; ///< postsynaptic connection index for each population

    void init( Configf& configf ) override;
    CntMat() : NF(0,0,0) {}
    ~CntMat() override = default;
    void step() override {}
  } cnt;

  struct Outputs : public NF {
    std::vector<std::unique_ptr<Outlet>> outlets; ///< Fields to write, in column order.
    int t = 0;
    int outputstart = 0;
    int outputinterval = 0;
    std::vector<unsigned int> node;
    Dumpf& dumpf;
    CntMat& cnt;
    Array<Population>& pops;
    Array<Propagator>& propagators;
    Array<Coupling>& couplings;

    void init( Configf& configf ) override;
    Outputs( size_type nodes, double deltat, Dumpf& dumpf, CntMat& cnt,
             Array<Population>& pops, Array<Propagator>& propagators,
             Array<Coupling>& couplings )
      : NF(nodes,deltat,0), dumpf(dumpf), cnt(cnt),
           pops(pops), propagators(propagators),
           couplings(couplings) {}
    ~Outputs() override = default;
    void step() override;
    void writeName( Outlet& outlet );
    void writeNode( Outlet& outlet );
    void writeOutlet( Outlet& outlet );
    /// Takes the outlets collected in `output`, appending them to outlets.
    void add( Output& output ) {
      for( auto& outlet : output.release() ) {
        outlets.push_back( std::move(outlet) );
      }
    }
  };
  std::unique_ptr<Outputs> outputs; ///< Created by init().

  Array<Population> pops;
  Array<Propagator> propagators;
  Array<Coupling> couplings;
 protected:
  void init( Configf& configf ) override;
 public:
  Solver() = delete;              ///< No default constructor allowed.
  Solver(const Solver&) = delete; ///< No copy constructor allowed.

  explicit Solver( Dumpf& dumpf );
  ~Solver() override;

  void solve(); ///< main integration loop
  void step() override;
};

#endif //NFTSIM_SRC_SOLVER_H
