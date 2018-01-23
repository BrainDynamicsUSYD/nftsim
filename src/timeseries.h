/** @file timeseries.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_TIMESERIES_H
#define NEUROFIELD_SRC_TIMESERIES_H

// Other neurofield headers
#include "configf.h"    // Configf;
#include "nf.h"         // NF;
#include "random.h"     // Random;

// C++ standard library headers
#include <limits>   // std::numeric_limits<double>::infinity()
#include <vector>   // std::vector;

class Timeseries : public NF {
 protected:
  using series_size_type = std::vector<double>::size_type;
  void init( Configf& configf ) override;

  std::vector<Timeseries*> series;
  std::vector<size_type> node;
  double inf = std::numeric_limits<double>::infinity(); ///< Infinity
  double t = 0.0;     ///< Current time relative to stimulus onset.
  double duration = inf; ///< Duration of the stimulus.
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
    double mean = 0.0; ///<
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
    double sigma = 0.03125; ///< width of transition.
    PulseSigmoid(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
    std::vector<double> onset_midpoints;
    double first_onset_mid;
    double PiOnSqrt3;
    int pulse_count;
  };

  struct Sine : public Timeseries {
    double amp = 0.0;   ///< Amplitude of the sine wave.
    double freq = 0.0;  ///< Frequency of the sine wave [s^-1].
    Sine(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    ~Sine() override = default;
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

  struct White : public Timeseries {
    uint_fast64_t seed = 0; ///<
    double amp = 0.0;       ///<
    double mean = 0.0;      ///<
    double deltax = 0.0;    ///<
    Random* random;
    White(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    ~White() override {
      delete random;
    }
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

  struct WhiteCoherent : public Timeseries {
    uint_fast64_t seed = 0; ///<
    double amp = 0.0;       ///<
    double mean = 0.0;      ///<
    Random* random;
    WhiteCoherent(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    ~WhiteCoherent() override {
      delete random;
    }
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
    double t_mns = 0.0; ///<
    double isi = 0.0;   ///< Inter-Stimulus-Interval.
    PAS(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

  struct Burst : public Timeseries {
    double amp = 0.0;    ///<
    double width = 0.0;  ///<
    double bursts = 0.0; ///<
    double freq = 0.0;   ///<
    double oscillation_freq = 0.0; ///<
    double on = 0.0;    ///<
    double off = 0.0;   ///<
    double total = 0.0; ///<
    Burst(size_type nodes, double deltat, size_type index) : Timeseries(nodes, deltat, index) {}
    void init( Configf& configf ) override;
    void fire( std::vector<double>& Q ) const override;
  };

} // namespace TIMESERIES
#endif //NEUROFIELD_SRC_TIMESERIES_H
