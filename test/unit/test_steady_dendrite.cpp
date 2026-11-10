/** @file test_steady_dendrite.cpp
  @brief A `V: Steady` dendrite starts at its coupling's actual initial input.

  A constant stimulus population drives a neural population through a Map
  propagator and either a Matrix coupling (uniform nu, so the input at node j
  is sum_i nu*phi_i = Nodes*nu*phi) or a Map coupling with one nu per node.
  The objects are built and initialised in Solver::init's order. With constant
  input the dendrite must already be at its steady state, so the soma
  potential equals the coupling's nu*phi from the first step. Before the fix
  the Steady dendrite started at nuinit()*phi, using only the first nu value.
*/

#include "unit_test.h"

#include "configf.h"
#include "coupling.h"
#include "long_coupling.h"
#include "population.h"
#include "propagator.h"

#include <cmath>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace {

const std::size_t kNodes = 4;
const double kDeltat = 1e-4;
const double kPhi = 7.0; // stimulus Mean, so phi = 7 at every node

std::string network_config( const std::string& coupling_line ) {
  return "Population 1: Excitatory\n"
         "Length: 0.5\n"
         "Q: 1.0\n"
         "Firing: Function: Linear a: 1.0 b: 0.0\n"
         " Dendrite 1: V: Steady alpha: 83.0 beta: 769.0\n"
         "Population 2: Stimulation\n"
         "Length: 0.5\n"
         " Stimulus: Const - Mean: 7.0\n"
         "Propagator 1: Map - Tau: 0\n"
         + coupling_line + "\n";
}

/// Builds the network with coupling type C, steps it, and returns the largest
/// relative difference between the soma potential and the expected nu*phi.
template<class C>
double steady_error( const std::string& coupling_line, const std::vector<double>& expected,
                     int steps ) {
  unit::TempConfig conf(network_config(coupling_line));
  Configf configf(conf.path());
  Population post(kNodes, kDeltat, 0);
  Population pre(kNodes, kDeltat, 1);
  Propagator prop(kNodes, kDeltat, 0, pre, post, 2, "Torus");
  C coup(kNodes, kDeltat, 0, prop, post);
  post.add2Dendrite(0, prop, coup, configf);
  configf.param("Population 1", post);
  configf.param("Population 2", pre);
  configf.param("Propagator 1", prop);
  configf.param("Coupling 1", coup);
  post.initFromInputs();
  double err = 0.0;
  for( int n=0; n<steps; n++ ) {
    coup.step();
    pre.step();
    post.step();
    prop.step();
    for( std::size_t j=0; j<kNodes; j++ ) {
      const double e = std::abs(post.V()[j] - expected[j])/std::abs(expected[j]);
      if( e > err ) {
        err = e;
      }
    }
  }
  return err;
}

} // namespace

TEST(steady_dendrite_matrix_coupling) {
  const double nu = 1e-4;
  const std::vector<double> expected(kNodes, kNodes*nu*kPhi); // sum over the 4 source nodes
  CHECK_MSG(steady_error<LongCoupling>("Coupling 1: Matrix - nu: 1e-4", expected, 200) < 1e-12,
            "Steady V with a uniform Matrix coupling does not start at sum_i nu*phi_i");
}

TEST(steady_dendrite_per_node_nu) {
  const std::vector<double> nu = {1e-4, 2e-4, 3e-4, 4e-4};
  std::vector<double> expected;
  for( double x : nu ) {
    expected.push_back(x*kPhi);
  }
  CHECK_MSG(steady_error<Coupling>("Coupling 1: Map - nu: 1e-4 2e-4 3e-4 4e-4", expected, 200) < 1e-12,
            "Steady V with per-node nu does not start at nu_j*phi at each node");
}
