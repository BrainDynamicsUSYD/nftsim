/***************************************************************************
                          harmonic.h  -  spatially homogeneous harmonic
                                         oscillator propagator
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef HARMONIC_H
#define HARMONIC_H

#include<vector>
using std::vector;
#include"propag.h"
#include"stencil.h"

class Harmonic : public Propag
{
  Harmonic(); // no default constructor
  Harmonic(Harmonic&); // no copy constructor

  double gamma;  // damping coefficient
protected:
  void init( Configf& configf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const;

  vector<double> oldp;
  vector<double> oldQ;
  vector<double> dpdt;
  double dQdt;

  // variables that initializes once
  double twoongamma;
  double expgamma;

  // variables that change every timestep
  double adjustedQ;
  double C1;
  double C1dtplusC2;
public: 
  Harmonic( int nodes, double deltat, int index, Population& prepop,
      Population& postpop, int longside );
  virtual ~Harmonic(void);
  virtual void step(void); 
};

#endif
