/** @file timeseries.h
  @brief Declarations for a collection of time-series, primarily for use as stimuli.

  A stimulus population owns one Timeseries, which holds one or more
  (`Superimpose: N`) time-series from namespace TIMESERIES, each with its own
  onset, duration and nodes, and sums them in fire().

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_TIMESERIES_H
#define NFTSIM_SRC_TIMESERIES_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "nf.h"         // NF;
#include "random.h"     // Random;

// C++ standard library headers
#include <limits>   // std::numeric_limits<double>::infinity()
#include <memory>   // std::unique_ptr;
#include <vector>   // std::vector;

class Timeseries : public NF {
 protected:
  using series_size_type = std::vector<double>::size_type;
  void init( Configf& configf ) override;
  double gridSpacing( Configf& configf ) const;

  std::vector<std::unique_ptr<Timeseries>> series; ///< Superimposed stimuli (only in the top-level Timeseries).
  std::vector<size_type> node;
  mutable std::vector<double> scratch; ///< Work space for fire(), one value per node.
  double inf = std::numeric_limits<double>::infinity(); ///< Infinity
  double t = 0.0;     ///< Current time relative to stimulus onset.
  double onset = 0.0; ///< Onset time for the stimulus.
  double duration = inf; ///< Duration of the stimulus.
  double t0 = 0.0;    ///< Value of t at the start of the simulation, set after init.
  size_type step_count = 0; ///< Number of steps taken, so t = t0 + step_count*deltat.
  /// Fraction of deltat within which times are treated as equal.
  static constexpr double time_tolerance = 1.0e-6;
 public:
  Timeseries(const Timeseries&) = delete; // No copy constructor allowed.
  Timeseries() = delete;                  // No default constructor allowed.

  Timeseries( size_type nodes, double deltat, size_type index );
  ~Timeseries() override;
  void step() override;
  virtual void fire( std::vector<double>& Q ) const;
};

namespace TIMESERIES {

  struct Const : public Timeseries {
    double mean = 0.0; ///< The constant value.
    Const(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

  struct PulseRect : public Timeseries {
    double amp = 0.0;    ///< Amplitude of the pulse.
    double width = 0.0;  ///< Width of the pulse [s].
    double period = 0.0; ///< Time between the start of each pulse [s].
    double pulses = 0.0; ///< Maximum number of pulses.
    PulseRect(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

  struct PulseSine : public Timeseries {
    double amp = 0.0;    ///< Amplitude of the pulse.
    double width = 0.0;  ///< Width of the pulse [s], wavelength of sine.
    double period = 0.0; ///< Time between the start of each pulse [s].
    double phase = 0.0;  ///< Phase of the sine wave in [degrees].
    double pulses = 0.0; ///< Maximum number of pulses.
    PulseSine(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    ~PulseSine() override = default;
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

  struct PulseSigmoid : public Timeseries {
    double amp = 0.0;    ///< Amplitude of the pulse.
    double width = 0.0;  ///< Width of the pulse [s].
    double period = 0.0; ///< Time between the start of each pulse [s].
    double pulses = 0.0; ///< Maximum number of pulses.
    double sigma = 0.03125; ///< Width of transition.
    PulseSigmoid(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
    std::vector<double> onset_midpoints;
    double first_pulse_mid = 0.0; ///< Mid-point of the first pulse.
    static constexpr double pi_on_sqrt3 = 1.813799364234217836866491779801435768604278564; //M_PI / std::sqrt(3.0);
    size_type pulse_count = 0; ///< Number of pulses, min of pulses and number of pulses that fit in duration.
  };

  struct Sine : public Timeseries {
    double amp = 0.0;   ///< Amplitude of the sine wave.
    double freq = 0.0;  ///< Frequency of the sine wave [s^-1].
    Sine(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    ~Sine() override = default;
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

  /// Gaussian white noise, independent at each node (`White`).
  struct White : public Timeseries {
    uint_fast64_t seed = 0; ///< Ranseed, if given.
    double stddev = 0.0;    ///< Standard deviation per step (StdDev, or computed from ASD).
    double asd = 0.0;       ///< Amplitude spectral density.
    double mean = 0.0;      ///< Mean.
    double deltax = 0.0;    ///< Grid spacing used to scale ASD on a grid.
    std::unique_ptr<Random> random;
    White(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

  /// Gaussian white noise in time, the same at every node (`WhiteCoherent`).
  struct WhiteCoherent : public Timeseries {
    uint_fast64_t seed = 0; ///< Ranseed, if given.
    double stddev = 0.0;      ///< Standard deviation per step (StdDev, or computed from ASD).
    double asd    = 0.0;      ///< Amplitude spectral density.
    double mean   = 0.0;      ///< Mean.
    std::unique_ptr<Random> random;
    WhiteCoherent(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

  struct PAS : public Timeseries {
    double n20w = 0.0;  ///< Width of the negative amplitude response at around 20 ms.
    double n20h = 0.0;  ///< Height of the negative amplitude response at around 20 ms.
    double p25w = 0.0;  ///< Width of the positive amplitude response at around 25 ms.
    double p25h = 0.0;  ///< Height of the positive amplitude response at around 25 ms.
    double tmsw = 0.0;  ///< Width of the Transcranial Magnetic Stimulation(TMS).
    double tmsh = 0.0;  ///< Height of the Transcranial Magnetic Stimulation(TMS).
    double t_mns = 0.0; ///< Time of the median nerve stimulation (later than onset when ISI < 0).
    double isi = 0.0;   ///< Inter-Stimulus-Interval.
    PAS(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

  /// Bursts of rectangular pulses (`Burst`), eg theta-burst stimulation:
  /// pulses of width `Width` at `Burst Frequency`, `Bursts` pulses per burst,
  /// bursts at `Oscillation Frequency`, in trains `On` seconds on and `Off` off.
  struct Burst : public Timeseries {
    double amp = 0.0;    ///< Pulse amplitude.
    double width = 0.0;  ///< Pulse width [s].
    double bursts = 0.0; ///< Pulses per burst.
    double freq = 0.0;   ///< Pulse frequency within a burst [s^-1].
    double oscillation_freq = 0.0; ///< Burst frequency [s^-1].
    double on = 0.0;    ///< Duration of each train of bursts [s].
    double off = 0.0;   ///< Pause between trains [s].
    Burst(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

} // namespace TIMESERIES
#endif //NFTSIM_SRC_TIMESERIES_H
