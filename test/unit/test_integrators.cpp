/** @file test_integrators.cpp
  @brief The generic Euler and RK4 integrators in de.h.

  The test system, at node j with rate k_j, is
      dy0/dt = -k_j y0,           y0(0) = 1,
      dy1/dt =  k_j (y0 - y1),    y1(0) = 0,
  with solution y0 = exp(-k_j t), y1 = k_j t exp(-k_j t). The second variable
  depends on the first, so a mix-up between variables and nodes shows up (the
  Euler integrator used to pass one variable across all nodes as the state of
  a node; fixed in commit "Fix state indexing in the (unused) Euler
  integrator").
*/

#include "unit_test.h"

#include "de.h"

#include <cmath>
#include <vector>

namespace {

const double kRates[] = {0.5, 1.0, 2.0, 3.0};
const vvd_size_type kNodes = 4;

struct DecayDE : public DE {
  explicit DecayDE( double deltat ) : DE(kNodes, deltat, 2) {
    for( vvd_size_type j=0; j<kNodes; j++ ) {
      variables[0][j] = 1.0;
      variables[1][j] = 0.0;
    }
  }
  void rhs( const std::vector<double>& y, std::vector<double>& dydt,
            std::vector<double>::size_type n ) override {
    dydt[0] = -kRates[n]*y[0];
    dydt[1] = kRates[n]*(y[0] - y[1]);
  }
};

/// Integrates to t = 1 with the given number of steps; returns the largest
/// absolute error over nodes and variables.
template<class Scheme> double max_error( int steps ) {
  DecayDE de(1.0/steps);
  Scheme scheme(de);
  for( int i=0; i<steps; i++ ) {
    scheme.step();
  }
  double err = 0.0;
  for( vvd_size_type j=0; j<kNodes; j++ ) {
    const double k = kRates[j];
    err = std::fmax(err, std::fabs(de[0][j] - std::exp(-k)));
    err = std::fmax(err, std::fabs(de[1][j] - k*std::exp(-k)));
  }
  return err;
}

/// Observed order of accuracy from errors at steps and 2*steps.
template<class Scheme> double observed_order( int steps ) {
  return std::log2(max_error<Scheme>(steps)/max_error<Scheme>(2*steps));
}

} // namespace

TEST(rk4_accuracy_and_order) {
  CHECK(max_error<RK4>(20) < 1e-5);
  CHECK(max_error<RK4>(160) < 3e-9);
  // (At 10 steps the observed order is still 4.27, outside the asymptotic range.)
  for( int steps : {20, 40, 80} ) {
    const double p = observed_order<RK4>(steps);
    CHECK_MSG(p > 3.8 && p < 4.2, "steps " << steps << ": order " << p);
  }
}

TEST(euler_accuracy_and_order) {
  CHECK(max_error<Euler>(1000) < 2e-3);
  for( int steps : {100, 200, 400} ) {
    const double p = observed_order<Euler>(steps);
    CHECK_MSG(p > 0.95 && p < 1.05, "steps " << steps << ": order " << p);
  }
}
