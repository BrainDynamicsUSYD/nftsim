#ifndef CARK4_H
#define CARK4_H

#include"couple.h"
#include"de.h"

class CaDP : public virtual Couple
{
  CaDP();
  CaDP(CaDP&);

  struct CaDE : public RK4
  {
    double B; // 1/stanard deviation of glutamate binding
    double glu_0; // glutamte dose-response threshold

    double max; // maximum synaptic strength
    double th;  // threshold time-scale of plasticity
    double ltd; // time-scale of depression
    double ltp; // time-scale of potentiation

    double dth; // calcium threshold to depression
    double pth; // calcium threshold to potentiation

    double tCa; // time-scale of calcium influx/cascade

    double gnmda; // gain for NMDA receptor

	double pos; // sign of nu

    CaDE( int nodes, double deltat ) : RK4(4,nodes,deltat) {}
    virtual ~CaDE(void) {}
    void rhs( const vector<double>& y, vector<double>& dydt );
    inline double sig( double x, double beta ) const;
    inline double po(double Ca) const; // potentiation rate
    inline double de(double Ca) const; // depression rate
  };
protected:
  CaDE rk4;
  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  CaDP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop );
  virtual ~CaDP(void);
  virtual void step(void);
  virtual const vector<double>& nu(void) const;
  virtual vector<Output*> output(void) const;
};

#endif
