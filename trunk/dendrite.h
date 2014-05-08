#ifndef DENDRITICR_H
#define DENDRITICR_H
//#include<vector>

#include"population.h"

using std::vector;

class Dendrite : public NF
{
  Dendrite(void); // default constructor
  Dendrite(Dendrite& ); // no copy constructor

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
  
  double alpha; // needed here for DendriteRamp
  double beta;

  vector<double> v;
  vector<double> dvdt;
  //vector<double> np;
  vector<double> oldnp;

  virtual void init( Configf& configf );
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;
public: 
  const Propag& prepropag;
  const Couple& precouple;

  Dendrite( int nodes, double deltat, int index,
      const Propag& prepropag, const Couple& precouple );
  virtual ~Dendrite(void);
  virtual void step(void);
  inline const vector<double>& V(void) const;
  virtual void output( Output& output ) const;
};

const vector<double>& Dendrite::V(void) const
{
  return v;
}

#endif
