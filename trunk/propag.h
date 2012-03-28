#ifndef PROPAG_H
#define PROPAG_H

#include"array.h"
#include"output.h"
#include"population.h"
#include"tau.h"
#include"configf.h"
#include"nf.h"

class Population;
class Tau;

class Propag : public NF
{
  Propag(); // no default constructor
  Propag(Propag&); // no copy constructor
protected:
  void init( Configf& configf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const;

  Population& prepop;
  Population& postpop;
  Tau tau; // tau_ab
  int longside;
  vector<double> p; // phi_ab
public: 
  Propag( int nodes, double deltat, int index, Population& prepop,
      Population& postpop, int longside );
  virtual ~Propag(void);
  virtual void step(void); 
  virtual const vector<double>& phi(void) const;
  virtual vector<Output*> output(void) const;
};

#endif
