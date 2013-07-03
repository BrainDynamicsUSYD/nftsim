#ifndef HARMONIC_H
#define HARMONIC_H

#include<vector>
using std::vector;
#include"propag.h"

class Harmonic : public virtual Propag
{
  Harmonic(); // no default constructor
  Harmonic(Harmonic&); // no copy constructor
protected:
  virtual void init( Configf& configf );
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

  double gamma;  // damping coefficient

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
      Population& postpop, int longside, string topology );
  virtual ~Harmonic(void);
  virtual void step(void); 
};

#endif
