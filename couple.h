#ifndef COUPLE_H
#define COUPLE_H

#include"array.h"
#include"population.h"
#include"propag.h"
#include"output.h"
#include"configf.h"
#include"nf.h"

class Propag;
class Population;

class Couple : public NF
{
  Couple();
  Couple(Couple&);
protected:
  virtual void init( Configf& configf ); 
  virtual void restart( Restartf& restartf ); 
  virtual void dump( Dumpf& dumpf ) const; 

  const vector<double>& glu;
  const Propag& prepropag;
  const Population& postpop;
  vector<double> n;
  int pos;
public: 
  Couple( int nodes, double deltat, int index, const vector<double>& glu,
      const Propag& prepropag, const Population& postpop );
  virtual ~Couple(void);
  virtual void step(void);
  virtual vector<Output*> output(void) const; 
  virtual const vector<double>& nu(void) const;
  virtual const double operator[]( int node ) const;
  bool excite(void) const;
};

#endif
