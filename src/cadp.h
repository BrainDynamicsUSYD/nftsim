/** @file cadp.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_CADP_H
#define NFTSIM_SRC_CADP_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "de.h"         // DE; RK4;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <memory> // std::unique_ptr;
#include <vector> // std::vector;

class CaDP : public Coupling {
 protected:
  struct CaDE : public DE {
    double nu_init = 0.0;

    double B = 0.0; ///< 1/standard deviation of glutamate binding
    double glu_0 = 0.0; ///< glutamate dose-response threshold

    double max = 0.0; ///< maximum synaptic strength
    double xth = 0.0; ///< threshold time-scale of plasticity
    double yth = 0.0; ///< threshold time-scale of plasticity
    double ltd = 0.0; ///< time-scale of depression
    double ltp = 0.0; ///< time-scale of potentiation

    double dth = 0.0; ///< calcium threshold to depression
    double pth = 0.0; ///< calcium threshold to potentiation

    double tCa = 0.0; ///< time-scale of calcium influx/cascade

    double gnmda = 0.0; ///< gain for NMDA receptor
    double z = 0.0; ///< timescale of protein cascade

    int pos = 0; ///< sign of nu

    virtual void init( Configf& configf );
    CaDE( size_type nodes, double deltat ) : DE(nodes,deltat,8) {}
    ~CaDE() override = default;

    void rhs( const std::vector<double>& y, std::vector<double>& dydt, size_type /*unused*/ ) override;
    double sig( double x, double beta ) const;
    virtual double _x( double Ca ) const; ///< potentiation rate
    virtual double _y( double Ca ) const; ///< depression rate
    virtual void pot();
    virtual void dep();
  };
  std::unique_ptr<CaDE> de;  ///< Plasticity state; a BCMDE for BCM, null for BCMLong (which keeps one per node).
  std::unique_ptr<RK4> rk4;  ///< Integrates de (null when de is).
  void init( Configf& configf ) override;
  /// For derived classes: takes ownership of `de`, the plasticity state to
  /// integrate (CaDE or a class derived from it), which may be null.
  CaDP( size_type nodes, double deltat, size_type index,
        const Propagator& prepropag, const Population& postpop,
        std::unique_ptr<CaDE> de );
 public:
  CaDP() = delete;            // No default constructor allowed.
  CaDP(const CaDP&) = delete; // No copy constructor allowed.

  CaDP( size_type nodes, double deltat, size_type index,
        const Propagator& prepropag, const Population& postpop );
  ~CaDP() override;
  void step() override;
  void output( Output& output ) const override;
};

#endif //NFTSIM_SRC_CADP_H
