#ifndef CADP_H
#define CADP_H

#include"couple.h"

class CaDP : public Couple
{
  CaDP();
  CaDP(CaDP&);

  double sig( double x, double beta ) const;
  double x(double Ca) const; // potentiation rate
  double y(double Ca) const; // depression rate

  vector<double> binding; // glutamate binding

  double B; // 1/stanard deviation of glutamate binding

  double max; // maximum synaptic strength
  double th; // threshold time-scale of plasticity
  double ltd; // time-scale of depression
  double ltp; // time-scale of potentiation

  double tCa; // time-scale of calcium influx/cascade
protected:
  vector<double> Ca;
  double dth; // calcium threshold to depression
  double pth; // calcium threshold to potentiation

  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  CaDP( int nodes, double deltat, int index, const vector<double>& glu,
          const Population& prepop, const Population& postpop );
  virtual ~CaDP(void);
  virtual void step(void);
  virtual vector<Output*> output(void) const;
};

#endif
