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
  std::vector<double> node;
  double inf = std::numeric_limits<double>::infinity(); ///< Infinity
  double t = 0.0;     ///< Current time relative to stimulus onset.
  double duration = inf; ///< Duration of the stimulus.
 public:
  Timeseries(const Timeseries&) = delete;  // No copy constructor allowed.
  Timeseries() = delete;                   // No default constructor allowed.

  Timeseries( size_type nodes, double deltat, size_type index );
  ~Timeseries() override;
  void step() override;
  virtual void fire( std::vector<double>& Q ) const;
};

namespace TIMESERIES {

struct Const : public Timeseries {
  double mean = 0.0;
  Const(size_type nodes,double deltat,size_type index) : Timeseries(nodes,deltat,index) {}
  void init( Configf& configf ) override;
  void fire( std::vector<double>& Q ) const override;
};

struct Pulse : public Timeseries {
  double amp = 0.0, width = 0.0, period = 0.0, pulses = 0.0;
  Pulse(size_type nodes,double deltat,size_type index) : Timeseries(nodes,deltat,index) {}
  void init( Configf& configf ) override;
  void fire( std::vector<double>& Q ) const override;
};

struct White : public Timeseries {
  uint_fast64_t seed=0;
  double amp = 0.0, mean = 0.0, deltax = 0.0;
  Random* random;
  White(size_type nodes,double deltat,size_type index) : Timeseries(nodes,deltat,index) {}
  ~White() override {
    delete random;
  }
  void init( Configf& configf ) override;
  void fire( std::vector<double>& Q ) const override;
};

struct WhiteCoherent : public Timeseries {
  uint_fast64_t seed=0;
  double amp = 0.0, mean = 0.0;
  Random* random;
  WhiteCoherent(size_type nodes,double deltat,size_type index) : Timeseries(nodes,deltat,index) {}
  ~WhiteCoherent() override {
    delete random;
  }
  void init( Configf& configf ) override;
  void fire( std::vector<double>& Q ) const override;
};

struct PAS : public Timeseries {
  double n20w = 0.0, n20h = 0.0, p25w = 0.0, p25h = 0.0;
  double tmsw = 0.0, tmsh = 0.0, t_mns = 0.0, isi = 0.0;
  PAS(size_type nodes,double deltat,size_type index) : Timeseries(nodes,deltat,index) {}
  void init( Configf& configf ) override;
  void fire( std::vector<double>& Q ) const override;
};

struct Burst : public Timeseries {
  double amp = 0.0, width = 0.0, bursts = 0.0, freq = 0.0;
  double oscillation_freq = 0.0, on = 0.0, off = 0.0, total = 0.0;
  Burst(size_type nodes,double deltat,size_type index) : Timeseries(nodes,deltat,index) {}
  void init( Configf& configf ) override;
  void fire( std::vector<double>& Q ) const override;
};

struct Sine : public Timeseries {
  double amp = 0.0, width = 0.0, period = 0.0, phase = 0.0, pulses = 0.0;
  Sine(size_type nodes,double deltat,size_type index) : Timeseries(nodes,deltat,index) {}
  ~Sine() override = default;
  void init( Configf& configf ) override;
  void fire( std::vector<double>& Q ) const override;
};

} // namespace TIMESERIES
#endif //NEUROFIELD_SRC_TIMESERIES_H
