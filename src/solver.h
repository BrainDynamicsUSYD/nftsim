/** @file solver.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_SOLVER_H
#define NEUROFIELD_SRC_SOLVER_H


#include"population.h"

using std::vector;

class Propag;
class Couple;
class Population;

class Solver : public NF {
  Solver(); // no default constructor
  Solver(Solver& ); // no copy constructor

  Dumpf& dumpf;

  int steps; // number of integration steps to perform

  struct CntMat : public NF {
    int npop; // number of populations
    int ncnt; // number of connections
    vector< vector<double> > raw; // 2D vector of raw connection matrix

    vector<int> pre;  // presynaptic connection index for each population
    vector<int> post;  // postsynaptic connection index for each population
    vector<int> ndr;  // number of dendrites for each population

    void init( Configf& configf ) override;
    //void restart( Restartf& restartf ) {}
    //void dump( Dumpf& dumpf ) const;
    CntMat(void) : NF(0,0,0) {}
    ~CntMat(void) override = default;
    void step(void) override {}
  } cnt;

  struct Outputs : public NF {
    vector<Outlet*> outlets;
    int t;
    int outputstart;
    int outputinterval;
    vector<unsigned int> node;
    Dumpf& dumpf;
    CntMat& cnt;
    Array<Population>& pops;
    Array<Propag>& propags;
    Array<Couple>& couples;

    void init( Configf& configf ) override;
    //void restart( Restartf& restartf ) {}
    //void dump( Dumpf& dumpf ) const {}
    Outputs( int nodes, double deltat, Dumpf& dumpf,
             CntMat& cnt, Array<Population>& pops, Array<Propag>& propags, Array<Couple>& couples )
      : NF(nodes,deltat,0), dumpf(dumpf),
        cnt(cnt), pops(pops), propags(propags), couples(couples) {}
    ~Outputs(void) override {
      for(auto & outlet : outlets) {
        delete outlet;
      }
    }
    void step(void) override;
    void writeName( Outlet& outlet );
    void writeNode( Outlet& outlet );
    void writeOutlet( Outlet& outlet );
    void add( vector<Outlet*> _outlets ) {
      for(auto & _outlet : _outlets) {
        outlets.push_back( _outlet );
      }
    }
  };
  Outputs* outputs;

  Array<Population> pops;
  Array<Propag> propags;
  Array<Couple> couples;
 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;
 public:
  explicit Solver( Dumpf& dumpf );
  ~Solver(void) override;

  void solve(void); // main integration loop
  void step(void) override;
};


#endif
