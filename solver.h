#ifndef SOLVER_H
#define SOLVER_H

#include<vector>
using std::vector;

#include"array.h"
#include"propag.h"
#include"couple.h"
#include"output.h"
#include"de.h"
//#include"nf.h"

class Propag;
class Couple;
class Population;

class Solver : public NF
{
  Solver(); // no default constructor
  Solver(Solver& ); // no copy constructor

  Dumpf& dumpf;

  int steps; // number of integration steps to perform

  struct CntMat : public NF
  {
    int npop; // number of populations
    int ncnt; // number of connections
    vector< vector<double> > raw; // 2D vector of raw connection matrix

    vector<int> pre;  // presynaptic connection index for each population
    vector<int> post;  // postsynaptic connection index for each population
    vector<int> ndr;  // number of dendrites for each population

    void init( Configf& configf );
    //void restart( Restartf& restartf ) {}
    //void dump( Dumpf& dumpf ) const;
    CntMat(void) : NF(0,0,0) {}
    ~CntMat(void) {}
    void step(void) {}
  } cnt;

  struct Outputs : public NF
  {
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

    void init( Configf& configf );
    //void restart( Restartf& restartf ) {}
    //void dump( Dumpf& dumpf ) const {}
    Outputs( int nodes, double deltat, Dumpf& dumpf,
        CntMat& cnt, Array<Population>& pops, Array<Propag>& propags, Array<Couple>& couples )
        : NF(nodes,deltat,0), dumpf(dumpf),
        cnt(cnt), pops(pops), propags(propags), couples(couples) {}
    ~Outputs(void) {
      for( size_t i=0; i<outlets.size(); i++ )
        delete outlets[i];
    }
    void step(void);
    void writeName( Outlet& outlet );
    void writeNode( Outlet& outlet );
    void writeOutlet( Outlet& outlet );
    void add( vector<Outlet*> _outlets ) {
      for( size_t i=0; i<_outlets.size(); i++ )
        outlets.push_back( _outlets[i] );
    }
  };
  Outputs* outputs;

  Array<Population> pops;
  Array<Propag> propags;
  Array<Couple> couples;
protected:
  virtual void init( Configf& configf );
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;
public: 
  Solver( Dumpf& dumpf );
  ~Solver(void);

  void solve(void); // main integration loop
  void step(void);
};


#endif
