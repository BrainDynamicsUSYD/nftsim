/** @file cadp.h
  @brief Declares CaDP, a coupling whose strength follows calcium-dependent plasticity.

  In CaDP (`Coupling N: CaDP ...`, Fung and Robinson) the synaptic strength
  nu follows the postsynaptic calcium concentration. Calcium enters through
  NMDA receptors, at a rate set by glutamate binding (from the postsynaptic
  GlutamateResponse) and the postsynaptic potential; it drives potentiation
  and depression rates x(Ca) and y(Ca) that set a target strength nutilde,
  which nu follows through a critically damped filter. The state has eight
  variables per node (see CaDP::CaDE::rhs()). BCM and BCMLong extend it.

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

/// Fixed constants of the calcium-dependent plasticity model (CaDP, BCM,
/// BCM-Spatial). Ca is a concentration in the units of `Dth:` and `Pth:`
/// (mol/L in the shipped configs, which use about 0.25e-6 and 0.45e-6).
namespace cadp {
/// The NMDA-receptor calcium current is gNMDA*B*H(V), with
/// H(V) = (nmda_reversal - V) / (1 + exp(-nmda_slope*(V - nmda_half_v))):
/// a driving force times the sigmoidal voltage dependence of NMDA receptors
/// (relief of their magnesium block).
constexpr double nmda_reversal = 195e-3; ///< [V] potential at which H(V) changes sign.
constexpr double nmda_half_v = 45.5e-3;  ///< [V] potential at which the voltage-dependent factor is 1/2.
constexpr double nmda_slope = 62.0;      ///< [1/V] steepness of the voltage-dependent factor.
/// Steepness [1/(units of Ca)] of the sigmoidal thresholds at Dth and Pth in
/// the potentiation and depression rates x(Ca) and y(Ca).
constexpr double ca_threshold_slope = 4e7;
constexpr double ca_init = 0.01e-6;  ///< Initial calcium concentration.
constexpr double default_tCa = 50e-3; ///< [s] Default `tCa:`, the calcium decay time.
constexpr double default_gNMDA = 2e-3; ///< Default `gNMDA:`, the NMDA-receptor gain.
/// [1/s] Default `z:`. nu follows nutilde through the critically damped
/// filter nu'' + 2 z nu' + z^2 nu = z^2 nutilde, with time-scale 1/z (100 s).
constexpr double default_z = 0.01;
constexpr double default_t_rec = 1e3; ///< [s] Default BCM `t_rec:`, the recovery time of gNMDA.
/// Initial off-diagonal coupling strength of a BCM-Spatial matrix when `nu:`
/// is a single value (which sets the diagonal).
constexpr double bcmlong_offdiagonal_nu = 1e-8;
} // namespace cadp

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
