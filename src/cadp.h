/** @file cadp.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_CADP_H
#define NEUROFIELD_SRC_CADP_H

// Other neurofield headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "de.h"         // DE; RK4;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class CaDP : public Coupling {
  CaDP();
  CaDP(CaDP&);
 protected:
  double nu_init;
  struct CaDE : public DE {
    double nu_init;
    //double alpha; // for fCaDE
    //double alpha_beta; // for fCaDE, == alpha -beta

    double B; ///< 1/standard deviation of glutamate binding
    double glu_0; ///< glutamate dose-response threshold

    double max; ///< maximum synaptic strength
    double xth; ///< threshold time-scale of plasticity
    double yth; ///< threshold time-scale of plasticity
    double ltd; ///< time-scale of depression
    double ltp; ///< time-scale of potentiation

    double dth; ///< calcium threshold to depression
    double pth; ///< calcium threshold to potentiation

    double tCa; ///< time-scale of calcium influx/cascade

    double gnmda; ///< gain for NMDA receptor
    double z; ///< timescale of protein cascade

    double pos; ///< sign of nu

    virtual void init( Configf& configf );
    CaDE( size_type nodes, double deltat ) : DE(nodes,deltat,8) {}
    ~CaDE(void) override = default;

    void rhs( const std::vector<double>& y, std::vector<double>& dydt ) override;
    double sig( double x, double beta ) const;
    virtual double _x( double Ca ) const; ///< potentiation rate
    virtual double _y( double Ca ) const; ///< depression rate
    virtual void pot(void);
    virtual void dep(void);
  };
  CaDE* de;
  RK4* rk4;
  void init( Configf& configf ) override;
 public:
  CaDP( size_type nodes, double deltat, size_type index,
        const Propagator& prepropag, const Population& postpop );
  ~CaDP(void) override;
  void step(void) override;
  void output( Output& output ) const override;
};

#endif //NEUROFIELD_SRC_CADP_H
