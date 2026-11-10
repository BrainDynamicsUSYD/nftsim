/** @file test_dendrite.cpp
  @brief Dendrite (RK4) and DendriteIntegral against analytic responses.

  The dendritic response obeys
      (1/(alpha beta)) d2V/dt2 + (1/alpha + 1/beta) dV/dt + V = nu*phi,
  where nu*phi comes from the presynaptic Coupling. A step nu*phi = P from
  rest gives
      V(t) = P [1 - (beta e^{-alpha t} - alpha e^{-beta t})/(beta - alpha)],
  or P [1 - (1 + alpha t) e^{-alpha t}] when alpha = beta.
*/

#include "unit_test.h"

#include "configf.h"
#include "coupling.h"
#include "dendrite.h"
#include "dendrite_integral.h"
#include "population.h"
#include "propagator.h"

#include <cmath>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

namespace {

/// A Coupling whose nu*phi is set directly by the test.
class FixedCoupling : public Coupling {
 public:
  FixedCoupling( size_type nodes, double deltat, const Propagator& prepropag,
                 const Population& postpop )
    : Coupling(nodes, deltat, 0, prepropag, postpop) {}
  void set( const std::vector<double>& nuphi ) { P = nuphi; }
};

/// The objects a Dendrite refers to. Only the Coupling's nu*phi is used.
struct Inputs {
  Population pre;
  Population post;
  Propagator propagator;
  FixedCoupling coupling;
  Inputs( std::vector<double>::size_type nodes, double deltat )
    : pre(nodes, deltat, 0), post(nodes, deltat, 1),
      propagator(nodes, deltat, 0, pre, post, nodes, "Torus"),
      coupling(nodes, deltat, propagator, post) {}
};

std::string dendrite_config( const std::string& type, double v0, double alpha, double beta ) {
  std::ostringstream conf;
  conf.precision(17);
  conf << "Dendrite 1: " << type << " V: " << v0 << " alpha: " << alpha << " beta: " << beta << "\n";
  return conf.str();
}

/// Response to a unit step at t = 0 from rest.
double step_response( double t, double alpha, double beta ) {
  if( alpha == beta ) {
    return 1.0 - (1.0 + alpha*t)*std::exp(-alpha*t);
  }
  return 1.0 - (beta*std::exp(-alpha*t) - alpha*std::exp(-beta*t))/(beta - alpha);
}

/// Response to a unit ramp t (t >= 0) from rest: the integral of step_response.
double ramp_response( double t, double alpha, double beta ) {
  if( t <= 0.0 ) {
    return 0.0;
  }
  if( alpha == beta ) {
    return t - (2.0*(1.0 - std::exp(-alpha*t)) - alpha*t*std::exp(-alpha*t))/alpha;
  }
  return t - ( beta/alpha*(1.0 - std::exp(-alpha*t))
             - alpha/beta*(1.0 - std::exp(-beta*t)) )/(beta - alpha);
}

/// Steps a dendrite of type D under constant nu*phi = levels (one per node),
/// from V = 0, and returns the largest error relative to the step response.
template<class D> double step_error( double deltat, double alpha, double beta, double duration,
                                     const std::vector<double>& levels ) {
  Inputs in(levels.size(), deltat);
  unit::TempConfig conf(dendrite_config("", 0.0, alpha, beta));
  Configf configf(conf.path());
  D dendrite(levels.size(), deltat, 0, in.propagator, in.coupling);
  configf >> dendrite;
  in.coupling.set(levels);
  const int steps = static_cast<int>(std::lround(duration/deltat));
  double err = 0.0;
  for( int n=1; n<=steps; n++ ) {
    dendrite.step();
    for( std::size_t j=0; j<levels.size(); j++ ) {
      const double exact = levels[j]*step_response(n*deltat, alpha, beta);
      err = std::fmax(err, std::fabs(dendrite.V()[j] - exact)/levels[j]);
    }
  }
  return err;
}

} // namespace

TEST(dendrite_rk4_step_response) {
  const std::vector<double> levels = {1.0, 2.5e-3};
  const double alpha = 50.0, beta = 200.0, duration = 0.1;
  const double e1 = step_error<Dendrite>(4e-4, alpha, beta, duration, levels);
  const double e2 = step_error<Dendrite>(2e-4, alpha, beta, duration, levels);
  const double e3 = step_error<Dendrite>(1e-4, alpha, beta, duration, levels);
  CHECK_MSG(e3 < 1e-9, "error " << e3);
  // RK4 with the input held constant over each step is fourth order.
  CHECK_MSG(std::log2(e1/e2) > 3.7 && std::log2(e1/e2) < 4.3, "order " << std::log2(e1/e2));
  CHECK_MSG(std::log2(e2/e3) > 3.7 && std::log2(e2/e3) < 4.3, "order " << std::log2(e2/e3));
  // alpha == beta
  CHECK(step_error<Dendrite>(1e-4, 100.0, 100.0, duration, levels) < 1e-7);
}

TEST(dendrite_numeric_initial_v) {
  // A numeric V must be the initial state (it used to be ignored, starting at
  // 0). With no input, V then relaxes as
  //   V0 (beta e^{-alpha t} - alpha e^{-beta t})/(beta - alpha).
  const double deltat = 1e-4, alpha = 50.0, beta = 200.0, v0 = 0.005;
  Inputs in(3, deltat);
  unit::TempConfig conf(dendrite_config("", v0, alpha, beta));
  Configf configf(conf.path());
  Dendrite dendrite(3, deltat, 0, in.propagator, in.coupling);
  configf >> dendrite;
  for( int j=0; j<3; j++ ) {
    CHECK_EQ(dendrite.V()[j], v0);
  }
  in.coupling.set(std::vector<double>(3, 0.0));
  for( int n=1; n<=500; n++ ) {
    dendrite.step();
  }
  const double t = 500*deltat;
  const double exact = v0*(1.0 - step_response(t, alpha, beta));
  for( int j=0; j<3; j++ ) {
    CHECK_CLOSE(dendrite.V()[j], exact, 1e-9*v0);
  }
}

TEST(dendrite_integral_step_response) {
  // DendriteIntegral is exact for nu*phi linear over each step. Starting from
  // V = 0 (and so a previous nu*phi of 0), a constant input P is seen as a
  // ramp from 0 to P over the first step, then constant:
  //   V(t) = P [R(t) - R(t - dt)]/dt, with R the unit-ramp response.
  // This differs from the step response by O(dt).
  const std::vector<double> levels = {1.0, 2.5e-3};
  const double deltat = 1e-4;
  const double rates[][2] = {{50.0, 200.0}, {83.3, 769.2}, {100.0, 100.0}};
  for( const auto& r : rates ) {
    const double alpha = r[0], beta = r[1];
    Inputs in(levels.size(), deltat);
    unit::TempConfig conf(dendrite_config("Integral -", 0.0, alpha, beta));
    Configf configf(conf.path());
    DendriteIntegral dendrite(levels.size(), deltat, 0, in.propagator, in.coupling);
    configf >> dendrite;
    in.coupling.set(levels);
    double ramp_err = 0.0, step_err = 0.0;
    for( int n=1; n<=1000; n++ ) {
      dendrite.step();
      const double t = n*deltat;
      const double ramped = (ramp_response(t, alpha, beta) - ramp_response(t - deltat, alpha, beta))/deltat;
      for( std::size_t j=0; j<levels.size(); j++ ) {
        ramp_err = std::fmax(ramp_err, std::fabs(dendrite.V()[j] - levels[j]*ramped)/levels[j]);
        step_err = std::fmax(step_err, std::fabs(dendrite.V()[j] - levels[j]*step_response(t, alpha, beta))/levels[j]);
      }
    }
    CHECK_MSG(ramp_err < 1e-10, "alpha " << alpha << ", beta " << beta << ": error " << ramp_err);
    // The ramped start delays the response by about half a step, so the
    // difference is about dt/2 times the largest dV/dt (under 40 per unit P
    // for these rates); alpha*dt is a loose bound on that.
    CHECK_MSG(step_err < alpha*deltat, "alpha " << alpha << ", beta " << beta << ": error " << step_err);
  }
}
