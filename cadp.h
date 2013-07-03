#ifndef CADP_H
#define CADP_H

#include"couple.h"
#include"de.h"

class CaDP : public Couple
{
  CaDP();
  CaDP(CaDP&);
protected:
  struct CaDE : public DE
  {
    double alpha; // for fCaDE
    double alpha_beta; // for fCaDE, == alpha -beta

    double B; // 1/stanard deviation of glutamate binding
    double glu_0; // glutamte dose-response threshold

    double max; // maximum synaptic strength
    double xth;  // threshold time-scale of plasticity
    double yth;  // threshold time-scale of plasticity
    double ltd; // time-scale of depression
    double ltp; // time-scale of potentiation

    double dth; // calcium threshold to depression
    double pth; // calcium threshold to potentiation

    double tCa; // time-scale of calcium influx/cascade

    double gnmda; // gain for NMDA receptor

    double pos; // sign of nu

    virtual void init( Configf& configf );
    CaDE( int nodes, double deltat ) : DE(nodes,deltat,8) {}
    virtual ~CaDE(void) {}

    virtual void rhs( const vector<double>& y, vector<double>& dydt );
    double sig( double x, double beta ) const;
    virtual double _x( double Ca ) const; // potentiation rate
    virtual double _y( double Ca ) const; // depression rate
    virtual void pot(void);
    virtual void dep(void);
  };
  CaDE* de;
  RK4* rk4;
  virtual void init( Configf& configf );
public: 
  CaDP( int nodes, double deltat, int index,
          const Propag& prepropag, const Population& postpop );
  virtual ~CaDP(void);
  virtual void step(void);
  virtual const vector<double>& nu(void) const;
  virtual void output( Output& output ) const;
};

#endif
