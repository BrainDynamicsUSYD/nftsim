/** @file test_delays.cpp
  @brief Tau (axonal delays in whole time-steps) and Population's delayed
         firing-rate lookup, Population::Q(tau).

  A stimulus population driven by a sine wave is stepped forward, recording
  its current firing rate at every step. The delayed rate Q(tau) must equal
  the rate recorded exactly tau/deltat steps earlier. These tests target two
  fixes in commit "Fix lost and truncated propagation delays":
   - Tau truncated tau/deltat, so 3e-4 s at deltat 1e-4 gave 2 steps, not 3;
   - Population::growHistory did not grow the history for a delay equal to its
     current length, so that delay wrapped round to zero.
*/

#include "unit_test.h"

#include "configf.h"
#include "population.h"
#include "tau.h"

#include <cstddef>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace {

const std::size_t kNodes = 2;

/// Writes the configuration for a sine-wave stimulus population.
std::string stimulus_config() {
  return "Population 1: Stimulation\n"
         "Length: 0.5\n"
         " Stimulus: Sine - Amplitude: 1.0 Frequency: 37.0\n";
}

/// A Tau read from a configuration file holding the given delays (seconds).
std::unique_ptr<Tau> make_tau( const std::string& delays, double deltat ) {
  unit::TempConfig conf(delays + "\n");
  Configf configf(conf.path());
  std::unique_ptr<Tau> tau(new Tau(kNodes, deltat, 0));
  configf >> *tau;
  return tau;
}

/// Steps a stimulus population that has had its history grown for every Tau
/// in taus, and checks that Q(taus[k]) at node j lags the current Q by
/// expected_steps[k][j] steps. Returns "" on success.
std::string check_lags( double deltat, const std::vector<std::string>& delays,
                        const std::vector<std::vector<int>>& expected_steps ) {
  unit::TempConfig conf(stimulus_config());
  Configf configf(conf.path());
  Population pop(kNodes, deltat, 0);
  configf.param("Population 1", pop);

  std::vector<std::unique_ptr<Tau>> taus;
  for( const std::string& d : delays ) {
    taus.push_back(make_tau(d, deltat));
    pop.growHistory(*taus.back());
  }

  const Tau zero_delay(kNodes, deltat, 0);
  std::vector<std::vector<double>> current; // current Q at each step
  std::ostringstream err;
  err << std::setprecision(17);
  for( int step=0; step<40; step++ ) {
    pop.step();
    current.push_back(pop.Q(zero_delay));
    for( std::size_t k=0; k<taus.size(); k++ ) {
      const std::vector<double>& delayed = pop.Q(*taus[k]);
      for( std::size_t j=0; j<kNodes; j++ ) {
        const int lag = expected_steps[k][j];
        if( step < lag ) {
          continue; // still reading the initial history
        }
        const double expected = current[step-lag][j];
        if( delayed[j] != expected ) {
          err << "Tau '" << delays[k] << "' (deltat " << deltat << "), node " << j
              << ", step " << step << ": Q(tau) = " << delayed[j] << ", but Q "
              << lag << " steps earlier was " << expected;
          for( int m=0; m<=step && m<6; m++ ) {
            if( current[step-m][j] == delayed[j] ) {
              err << " (it matches the lag of " << m << " steps)";
              break;
            }
          }
          return err.str();
        }
      }
    }
  }
  return "";
}

} // namespace

TEST(tau_rounds_to_nearest_step) {
  // k*1e-4 is not exactly representable, and k*1e-4/1e-4 can fall just below
  // k (3e-4/1e-4 = 2.9999999999999996), which truncation turned into k-1.
  for( int k=1; k<=12; k++ ) {
    std::ostringstream delay;
    delay << k*1e-4;
    const std::string err = check_lags(1e-4, {delay.str()}, {{k, k}});
    CHECK_MSG(err.empty(), err);
  }
  // Delays that are not whole steps round to the nearest step (with a warning).
  unit::SilenceCerr quiet;
  std::string err = check_lags(1e-4, {"2.6e-4"}, {{3, 3}});
  CHECK_MSG(err.empty(), err);
  err = check_lags(1e-4, {"2.4e-4"}, {{2, 2}});
  CHECK_MSG(err.empty(), err);
}

TEST(population_history_grows_for_delay_equal_to_its_length) {
  // deltat = 2^-10 and delays of 1, 2, 3 steps are exact in binary, so only
  // the history length is under test (as in test/numerical/map-delays.conf).
  const double deltat = 9.765625e-04;
  // A lone delay of one step: the history (length 1) must grow to 2.
  std::string err = check_lags(deltat, {"9.765625e-04"}, {{1, 1}});
  CHECK_MSG(err.empty(), err);
  // Delays of 1, 2 and 3 steps from one population, grown in that order: the
  // 3-step delay equals the length left by the 2-step one.
  err = check_lags(deltat, {"9.765625e-04", "1.953125e-03", "2.9296875e-03"},
                   {{1, 1}, {2, 2}, {3, 3}});
  CHECK_MSG(err.empty(), err);
  // Growing for a shorter delay after a longer one keeps the longer history.
  err = check_lags(deltat, {"2.9296875e-03", "9.765625e-04"}, {{3, 3}, {1, 1}});
  CHECK_MSG(err.empty(), err);
}

TEST(population_inhomogeneous_tau) {
  // One delay per node (2 and 5 steps).
  const double deltat = 9.765625e-04;
  std::string err = check_lags(deltat, {"1.953125e-03 4.8828125e-03"}, {{2, 5}});
  CHECK_MSG(err.empty(), err);
}
