/** @file test_glutamate_response.cpp
  @brief GlutamateResponse: glutamate starts at the steady state of its inputs.

  A constant stimulus population drives a GlutamateResponse population through
  an excitatory Map propagator and Map coupling, built and initialised in the
  same order as Solver::init. Glutamate obeys
  d(glu)/dt = Lambda*phi_e - glu/tGlu, so with constant phi_e it must start at
  Lambda*tGlu*phi_e and stay there. (Glu::init() used to resize() an
  already-sized vector, so glutamate started at 0 and relaxed upwards.)
*/

#include "unit_test.h"

#include "configf.h"
#include "coupling.h"
#include "glutamate_response.h"
#include "population.h"
#include "propagator.h"

#include <cstddef>
#include <vector>

TEST(glutamate_starts_at_steady_state) {
  const std::size_t nodes = 4;
  const double deltat = 1e-4;
  const double lambda = 150e-6, tglu = 30e-3, phi = 7.0;
  unit::TempConfig conf(
    "Population 1: Excitatory\n"
    "Length: 0.5\n"
    "Q: 1.0\n"
    "Firing: GlutamateResponse - Lambda: 150e-6 tGlu: 30e-3 "
    "Function: Sigmoid Theta: 13e-3 Sigma: 3.8e-3 Qmax: 340\n"
    " Dendrite 1: alpha: 83.0 beta: 769.0\n"
    "Population 2: Stimulation\n"
    "Length: 0.5\n"
    " Stimulus: Const - Mean: 7.0\n"
    "Propagator 1: Map - Tau: 0\n"
    "Coupling 1: Map - nu: 1e-4\n");
  Configf configf(conf.path());

  Population post(nodes, deltat, 0);
  Population pre(nodes, deltat, 1);
  Propagator prop(nodes, deltat, 0, pre, post, 2, "Torus");
  Coupling coup(nodes, deltat, 0, prop, post);
  post.add2Dendrite(0, prop, coup, configf);
  configf.param("Population 1", post);
  configf.param("Population 2", pre);
  configf.param("Propagator 1", prop);
  configf.param("Coupling 1", coup);
  post.initFromInputs();

  const double expected = lambda*tglu*phi;
  for( double g : post.glu() ) {
    CHECK_CLOSE(g, expected, 1e-15*expected);
  }

  // Step as Solver::step does; with constant input glutamate must not move.
  for( int step=0; step<500; step++ ) {
    coup.step();
    pre.step();
    post.step();
    prop.step();
  }
  for( double g : post.glu() ) {
    CHECK_CLOSE(g, expected, 1e-12*expected);
  }
}
