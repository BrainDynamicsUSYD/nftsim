/** @file cadp.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_CADP_H
#define NEUROFIELD_SRC_CADP_H

#include"couple.h"

class CaDP : public Couple {
  CaDP();
  CaDP(CaDP&);
 protected:
  double nu_init;
  struct CaDE : public DE {
    double nu_init;
    //double alpha; // for fCaDE
    //double alpha_beta; // for fCaDE, == alpha -beta

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
    double z; // timescale of protein cascade

    double pos; // sign of nu

    virtual void init( Configf& configf );
    CaDE( int nodes, double deltat ) : DE(nodes,deltat,8) {}
    ~CaDE(void) override = default;

    void rhs( const vector<double>& y, vector<double>& dydt ) override;
    double sig( double x, double beta ) const;
    virtual double _x( double Ca ) const; // potentiation rate
    virtual double _y( double Ca ) const; // depression rate
    virtual void pot(void);
    virtual void dep(void);
  };
  CaDE* de;
  RK4* rk4;
  void init( Configf& configf ) override;
 public:
  CaDP( int nodes, double deltat, int index,
        const Propag& prepropag, const Population& postpop );
  ~CaDP(void) override;
  void step(void) override;
  void output( Output& output ) const override;
};

#endif
