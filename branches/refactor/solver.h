/***************************************************************************
                          solver.h  -  runs the NeuroField simulation
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef NEUROFIELD_H
#define NEUROFIELD_H

#include<vector>
using std::vector;

#include"array.h"
#include"propag.h"
#include"couple.h"
#include"outputf.h"
#include"nf.h"

class Propag;
class Couple;
class Population;

class Solver : public NF
{
  Solver(); // no default constructor
  Solver(Solver& ); // no copy constructor

  Dumpf* dumpf;

  int steps; // number of integration steps to perform
  int skip;  // number of steps to skip before output

  double Lambda; // glutamate concentration per action potential
  double tGlu;   // decay time scale of glutamate
  vector<double> glu; // glutamate concentration in synaptic cleft
  vector<double> dglu; // Delta glutamate concentration from last timestep

  struct CntMat
  {
	int npop; // number of populations
	int ncnt; // number of connections
    vector< vector<double> > raw; // 2D vector of raw connection matrix
	vector<int> pre;  // presynaptic connection index for each population
	vector<int> post;  // postsynaptic connection index for each population
	vector<int> ndr;  // number of dendrites for each population
	void init( Configf& configf );
	void dump( Dumpf& dumpf ) const;
  } cnt;

  Array<Population> pops;
  Array<Propag> propags;
  Array<Couple> couples;
  Array<Outputf> outputfs;
protected:
  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  Solver(Dumpf* dumpf);
  virtual ~Solver(void);

  void solve(void); // main integration loop
  virtual void step(void);
};

#endif
