#ifndef DENDRITICR_H
#define DENDRITICR_H

#include<vector>
using std::vector;
#include"propag.h"
#include"couple.h"
#include"configf.h"
#include"nf.h"

class Propag;
class Couple;

class Dendrite : public NF
{
  Dendrite(void); // default constructor
  Dendrite(Dendrite& ); // no copy constructor

  double alpha;
  double beta;

  // variables that are intialized once to speed up computation
  double aminusb; // == alpha - beta
  double expa; // == exp(-alpha*deltat)
  double expb; // == exp(-beta*deltat)
  double factorab; // == 1./alpha + 1./beta;

  // variables that are used every timestep
  double adjustednp;
  double deltaPdeltat;
  double C1;
  double dpdt;
  double C1expa;
  double C2expb;
  double C1dtplusC2;
protected:
  vector<double> v;
  vector<double> dvdt;
  vector<double> np;
  vector<double> oldnp;

  const Propag& prepropag;
  const Couple& precouple;

  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  Dendrite( int nodes, double deltat, int index,
      const Propag& prepropag, const Couple& precouple );
  virtual ~Dendrite(void);
  virtual void step(void);
  virtual const vector<double>& V(void) const;
  virtual vector<Output*> output(int req_index) const;
};

#endif
