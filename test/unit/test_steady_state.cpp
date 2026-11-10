/** @file test_steady_state.cpp
  @brief checkSteadyState: warns when the configured initial state is not a steady state.

  Small networks are built and initialised in the same order as Solver::init,
  then checked. A neural population is at a steady state if its configured Q
  equals S(sum_b nu_ab*phi_ab), its firing function of its initial soma input.
*/

#include "unit_test.h"

#include "configf.h"
#include "coupling.h"
#include "population.h"
#include "propagator.h"
#include "steady_state.h"

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace {

const double theta = 13e-3, sigma = 3.8e-3, qmax = 340.0;

double sigmoid( double v ) {
  return qmax/( 1.0 + std::exp( -(v-theta)/sigma ) );
}

/// A neural population (1) driven by a constant stimulus (2) of Q = 7
/// through a Map propagator and a Map coupling with the given nu (one value
/// or one per node), with configured Q `q`.
struct DrivenPopulation {
  unit::TempConfig conf;
  Configf configf;
  Population post, pre;
  Propagator prop;
  Coupling coup;
  steady_state::Result result;
  std::string warnings;

  DrivenPopulation( std::size_t nodes, double q, const std::string& nu )
    : conf(config(q, nu)), configf(conf.path()),
      post(nodes, 1e-4, 0), pre(nodes, 1e-4, 1),
      prop(nodes, 1e-4, 0, pre, post, 1, "Torus"),
      coup(nodes, 1e-4, 0, prop, post) {
    post.add2Dendrite(0, prop, coup, configf);
    configf.param("Population 1", post);
    configf.param("Population 2", pre);
    configf.param("Propagator 1", prop);
    configf.param("Coupling 1", coup);
    post.initFromInputs();
    std::ostringstream out;
    result = steady_state::checkSteadyState({&post, &pre}, {{1, 0, &prop, &coup}}, out);
    warnings = out.str();
  }

  static std::string config( double q, const std::string& nu ) {
    std::ostringstream s;
    s << std::setprecision(17)
      << "Population 1: Excitatory\n"
      << "Length: 0.5\n"
      << "Q: " << q << "\n"
      << "Firing: Function: Sigmoid Theta: 13e-3 Sigma: 3.8e-3 Qmax: 340\n"
      << " Dendrite 1: alpha: 83 beta: 769\n"
      << "Population 2: Stimulation\n"
      << "Length: 0.5\n"
      << " Stimulus: Const - Mean: 7.0\n"
      << "Propagator 1: Map - Tau: 0\n"
      << "Coupling 1: Map - nu: " << nu << "\n";
    return s.str();
  }
};

} // namespace

TEST(steady_state_consistent_population_is_silent) {
  const double nu = 1e-3;
  DrivenPopulation net(4, sigmoid(nu*7.0), "1e-3");
  CHECK(net.result.populations[0].checked);
  CHECK(net.result.populations[0].consistent);
  CHECK(!net.result.populations[1].checked); // the stimulus is not checked
  CHECK(net.result.phi_consistent[0]);
  CHECK(!net.result.searched);
  CHECK_MSG(net.warnings.empty(), "unexpected warning: " << net.warnings);
}

TEST(steady_state_inconsistent_population_warns) {
  const double nu = 1e-3;
  const double implied = sigmoid(nu*7.0);
  DrivenPopulation net(4, 8.87145, "1e-3");
  const steady_state::PopulationResult& r = net.result.populations[0];
  CHECK(r.checked);
  CHECK(!r.consistent);
  CHECK(r.uniform);
  CHECK_CLOSE(r.v, nu*7.0, 1e-15);
  CHECK_CLOSE(r.implied, implied, 1e-12*implied);
  CHECK_MSG(net.warnings.find("WARNING: Population 1's initial Q (8.87145) is not a steady state:"
                              " its initial inputs give Q = ") == 0, net.warnings);
  CHECK_MSG(net.warnings.find("(soma potential 7.000e-03 V). The simulation will start with a transient.")
            != std::string::npos, net.warnings);
  // The stimulus is fixed, so the steady state is the implied Q itself.
  CHECK(net.result.searched);
  CHECK(net.result.converged);
  CHECK_CLOSE(net.result.suggested[0], implied, 1e-10*implied);
  CHECK_CLOSE(net.result.suggested[1], 7.0, 0.0);
  CHECK_MSG(net.warnings.find("Population 1: Q: ") != std::string::npos, net.warnings);
}

TEST(steady_state_tolerance) {
  // Within the relative tolerance of 1e-3 is a steady state; beyond it is not.
  const double implied = sigmoid(1e-3*7.0);
  DrivenPopulation inside(1, implied*(1.0 + 0.5e-3), "1e-3");
  CHECK(inside.result.populations[0].consistent);
  CHECK(inside.warnings.empty());
  DrivenPopulation outside(1, implied*(1.0 + 2e-3), "1e-3");
  CHECK(!outside.result.populations[0].consistent);
  CHECK(!outside.warnings.empty());
}

TEST(steady_state_per_node_nu) {
  // nu differs at node 3, so the population is at a steady state only at the
  // other nodes; the warning names node 3, and no steady state is suggested
  // for a spatially varying network.
  const double implied = sigmoid(1e-3*7.0);
  DrivenPopulation net(4, implied, "1e-3 1e-3 2e-3 1e-3");
  const steady_state::PopulationResult& r = net.result.populations[0];
  CHECK(!r.consistent);
  CHECK(!r.uniform);
  CHECK_EQ(r.node, std::size_t(2));
  CHECK_CLOSE(r.implied, sigmoid(2e-3*7.0), 1e-12*r.implied);
  CHECK(!net.result.searched);
  CHECK_MSG(net.warnings.find(" at node 3 (soma potential 1.400e-02 V), the node where it differs most.")
            != std::string::npos, net.warnings);
  CHECK_MSG(net.warnings.find("No consistent set of Q values is suggested") != std::string::npos,
            net.warnings);
}

TEST(steady_state_self_coupled_linear_population) {
  // One population exciting itself, with Q = a*V + b and V = nu*Q: the steady
  // state is Q = b/(1 - a*nu) = 6 for a = 2, nu = 0.25, b = 3. From Q = 1 the
  // initial input gives Q = 3.5, and Newton's method finds Q = 6.
  unit::TempConfig conf(
    "Population 1: Self-exciting\n"
    "Length: 0.5\n"
    "Q: 1\n"
    "Firing: Function: Linear a: 2 b: 3\n"
    " Dendrite 1: alpha: 83 beta: 769\n"
    "Propagator 1: Map - Tau: 0\n"
    "Coupling 1: Map - nu: 0.25\n");
  Configf configf(conf.path());
  const std::size_t nodes = 1;
  Population pop(nodes, 1e-4, 0);
  Propagator map(nodes, 1e-4, 0, pop, pop, 1, "Torus");
  Coupling coup(nodes, 1e-4, 0, map, pop);
  pop.add2Dendrite(0, map, coup, configf);
  configf.param("Population 1", pop);
  configf.param("Propagator 1", map);
  configf.param("Coupling 1", coup);
  pop.initFromInputs();
  std::ostringstream out;
  const steady_state::Result result = steady_state::checkSteadyState({&pop}, {{0, 0, &map, &coup}}, out);
  CHECK(!result.populations[0].consistent);
  CHECK_CLOSE(result.populations[0].implied, 3.5, 1e-14);
  CHECK(result.converged);
  CHECK_CLOSE(result.suggested[0], 6.0, 1e-10);
  CHECK_MSG(out.str().find("Population 1: Q: 6\n") != std::string::npos, out.str());
}

TEST(steady_state_initial_phi_mismatch) {
  // An explicit initial phi that is not the presynaptic Q relaxes to it.
  unit::TempConfig conf(
    "Population 1: Excitatory\n"
    "Length: 0.5\n"
    "Q: 1\n"
    "Firing: Function: Linear a: 0 b: 1\n"
    " Dendrite 1: alpha: 83 beta: 769\n"
    "Population 2: Stimulation\n"
    "Length: 0.5\n"
    " Stimulus: Const - Mean: 7.0\n"
    "Propagator 1: Map - phi: 5 Tau: 0\n"
    "Coupling 1: Map - nu: 1e-3\n");
  Configf configf(conf.path());
  Population post(1, 1e-4, 0), pre(1, 1e-4, 1);
  Propagator prop(1, 1e-4, 0, pre, post, 1, "Torus");
  Coupling coup(1, 1e-4, 0, prop, post);
  post.add2Dendrite(0, prop, coup, configf);
  configf.param("Population 1", post);
  configf.param("Population 2", pre);
  configf.param("Propagator 1", prop);
  configf.param("Coupling 1", coup);
  post.initFromInputs();
  std::ostringstream out;
  const steady_state::Result result =
    steady_state::checkSteadyState({&post, &pre}, {{1, 0, &prop, &coup}}, out);
  CHECK(result.populations[0].consistent); // Q = 0*V + 1 = 1 whatever V is
  CHECK(!result.phi_consistent[0]);
  CHECK_MSG(out.str().find("WARNING: Propagator 1's initial phi (5) is not a steady state:"
                           " it relaxes towards Population 2's Q (7).") == 0, out.str());
}
