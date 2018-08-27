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
    std::vector<std::vector<double>::size_type> ndr;  ///< number of dendrites for each population

    void init( Configf& configf ) override;
    //void restart( Restartf& restartf ) {}
    //void dump( Dumpf& dumpf ) const;
    CntMat() : NF(0,0,0) {}
    ~CntMat() override = default;
    void step() override {}
  } cnt;

  struct Outputs : public NF {
    std::vector<Outlet*> outlets;
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
    //void restart( Restartf& restartf ) {}
    //void dump( Dumpf& dumpf ) const {}
    Outputs( size_type nodes, double deltat, Dumpf& dumpf, CntMat& cnt,
             Array<Population>& pops, Array<Propagator>& propagators,
             Array<Coupling>& couplings )
      : NF(nodes,deltat,0), dumpf(dumpf), cnt(cnt),
           pops(pops), propagators(propagators),
           couplings(couplings) {}
    ~Outputs() override {
      for(auto & outlet : outlets) {
        delete outlet;
      }
    }
    void step() override;
    void writeName( Outlet& outlet );
    void writeNode( Outlet& outlet );
    void writeOutlet( Outlet& outlet );
    void add( std::vector<Outlet*> _outlets ) {
      for(auto & _outlet : _outlets) {
        outlets.push_back( _outlet );
      }
    }
  };
  Outputs* outputs;

  Array<Population> pops;
  Array<Propagator> propagators;
  Array<Coupling> couplings;
 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;
 public:
  Solver() = delete;              ///< No default constructor allowed.
  Solver(const Solver&) = delete; ///< No copy constructor allowed.

  explicit Solver( Dumpf& dumpf );
  ~Solver() override;

  void solve(); ///< main integration loop
  void step() override;
};

#endif //NFTSIM_SRC_SOLVER_H
