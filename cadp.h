#ifndef CADP_H
#define CADP_H

#include"couple.h"

class CaDP : public virtual Couple
{
  CaDP();
  CaDP(CaDP&);

  inline double sig( double x, double beta ) const;
  inline double x(double Ca) const; // potentiation rate
  inline double y(double Ca) const; // depression rate

  vector<double> binding; // glutamate binding

  double B; // 1/stanard deviation of glutamate binding
  double glu_0; // glutamte dose-response threshold

  double max; // maximum synaptic strength
  double th; // threshold time-scale of plasticity
  double ltd; // time-scale of depression
  double ltp; // time-scale of potentiation

  double tCa; // time-scale of calcium influx/cascade
protected:
  vector<double> Ca;
  vector<double> nhu; // transient coupling strength
  double dth; // calcium threshold to depression
  double pth; // calcium threshold to potentiation
  vector<double> g; // gain for the NMDA receptor

  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  CaDP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop );
  virtual ~CaDP(void);
  virtual void step(void);
  virtual vector<Output*> output(void) const;
};

#endif
