/** @file test_propagator.cpp
  @brief A propagator's initial phi: Steady, one number, or one number per node.

  `phi:` may be `Steady` (phi starts at the presynaptic Q), a single number
  used at every node, or `Nodes` numbers, one per node (issue #127). These
  tests check that each form is read by every propagator type, that
  Propagator::phiinit() (used before init() by a dendrite with `V: Steady`)
  returns the same values, that a uniform list behaves exactly like the
  equivalent single number, and that a list of the wrong length is rejected.
*/

#include "unit_test.h"

#include "configf.h"
#include "harmonic.h"
#include "harmonic_integral.h"
#include "population.h"
#include "propagator.h"
#include "wave.h"
#include "wave_legacy.h"

#include <cstddef>
#include <cstdio>     // std::fflush;
#include <cstdlib>    // EXIT_FAILURE;
#include <iostream>   // std::cout;
#include <memory>     // std::unique_ptr;
#include <sstream>    // std::ostringstream;
#include <string>     // std::string;
#include <vector>     // std::vector;
#include <sys/wait.h> // waitpid; WIFEXITED; WEXITSTATUS;
#include <unistd.h>   // fork; _exit;

namespace {

const std::size_t kNodes = 4; // a 2x2 torus
const double kDeltat = 9.765625e-04; // 2^-10 s
const double kQ = 2.0;      // the presynaptic (stimulus) firing rate

/// A stimulus population with constant Q = kQ, then propagator 1.
std::string config( const std::string& type, const std::string& phi ) {
  std::ostringstream conf;
  conf << "Population 1: Stimulation\n"
       << "Length: 0.5\n"
       << " Stimulus: Const - Mean: " << kQ << "\n"
       << "Propagator 1: " << type << " - " << phi
       << " Tau: 0 Range: 0.5 gamma: 116\n";
  return conf.str();
}

/// Population 1, propagator 1 and a postsynaptic population, read from `text`.
struct Model {
  unit::TempConfig conf;
  Configf configf;
  Population pre;
  Population post;
  std::unique_ptr<Propagator> propagator;
  std::vector<double> phiinit; ///< Propagator::phiinit(), called before init()
  Model( const std::string& type, const std::string& phi, bool second_order=false )
    : conf(config(type, phi)), configf(conf.path()),
      pre(kNodes, kDeltat, 0), post(kNodes, kDeltat, 1) {
    const std::size_t longside = 2;
    if( type == "Map" ) {
      propagator.reset(new Propagator(kNodes, kDeltat, 0, pre, post, longside, "Torus"));
    } else if( type == "Wave" ) {
      propagator.reset(new Wave(kNodes, kDeltat, 0, pre, post, longside, "Torus"));
    } else if( type == "WaveLegacy" ) {
      propagator.reset(new WaveLegacy(kNodes, kDeltat, 0, pre, post, longside, "Torus"));
    } else if( type == "Harmonic" ) {
      propagator.reset(new Harmonic(kNodes, kDeltat, 0, pre, post, longside, "Torus"));
    } else {
      propagator.reset(new HarmonicIntegral(kNodes, kDeltat, 0, pre, post, longside, "Torus"));
    }
    propagator->useSecondOrder(second_order);
    configf.param("Population 1", pre);
    phiinit = propagator->phiinit(configf); // must leave the stream where it was
    configf.param("Propagator 1", *propagator);
  }
};

const std::vector<std::string> kTypes = {"Map", "Wave", "WaveLegacy", "Harmonic", "HarmonicIntegral"};

/// Runs a child process that is expected to stop with EXIT_FAILURE.
template<class F> bool exits_with_failure( F f ) {
  std::cout << std::flush;
  std::fflush(nullptr);
  const pid_t pid = fork();
  if( pid == 0 ) {
    unit::SilenceCerr quiet;
    f();
    _exit(0); // f() returned: not rejected
  }
  int status = 0;
  waitpid(pid, &status, 0);
  return WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE;
}

} // namespace

TEST(propagator_phi_forms_are_read) {
  const std::vector<double> per_node = {1.5, -0.25, 3.0, 7.0};
  for( const std::string& type : kTypes ) {
    struct Case { std::string phi; std::vector<double> expected; };
    const std::vector<Case> cases = {
      {"",                        std::vector<double>(kNodes, kQ)},
      {"phi: Steady",             std::vector<double>(kNodes, kQ)},
      {"phi: 0.75",               std::vector<double>(kNodes, 0.75)},
      {"phi: 1.5 -0.25 3.0 7.0",  per_node},
    };
    for( const Case& c : cases ) {
      Model model(type, c.phi);
      for( std::size_t i=0; i<kNodes; i++ ) {
        CHECK_MSG(model.propagator->phi()[i] == c.expected[i],
                  type << " '" << c.phi << "' node " << i << ": phi = "
                  << model.propagator->phi()[i] << ", expected " << c.expected[i]);
        CHECK_MSG(model.phiinit[i] == c.expected[i],
                  type << " '" << c.phi << "' node " << i << ": phiinit = "
                  << model.phiinit[i] << ", expected " << c.expected[i]);
      }
    }
  }
}

TEST(propagator_uniform_phi_list_matches_single_number) {
  for( const std::string& type : kTypes ) {
    for( bool second_order : {false, true} ) {
      Model single(type, "phi: 0.75", second_order);
      Model list(type, "phi: 0.75 0.75 0.75 0.75", second_order);
      for( int step=0; step<50; step++ ) {
        single.propagator->step();
        list.propagator->step();
      }
      for( std::size_t i=0; i<kNodes; i++ ) {
        CHECK_MSG(single.propagator->phi()[i] == list.propagator->phi()[i],
                  type << (second_order ? " (SecondOrder)" : "") << " node " << i
                  << ": " << single.propagator->phi()[i] << " != " << list.propagator->phi()[i]);
      }
    }
  }
}

/// The harmonic propagators have no spatial coupling, so each node of a
/// per-node phi must evolve exactly as a uniform phi of that value.
TEST(propagator_harmonic_nodes_evolve_independently) {
  const std::vector<double> per_node = {1.5, -0.25, 3.0, 7.0};
  for( const std::string type : {"Harmonic", "HarmonicIntegral"} ) {
    for( bool second_order : {false, true} ) {
      for( std::size_t i=0; i<kNodes; i++ ) {
        std::ostringstream phi;
        phi.precision(17);
        phi << "phi: " << per_node[i];
        Model single(type, phi.str(), second_order);
        Model list(type, "phi: 1.5 -0.25 3.0 7.0", second_order);
        for( int step=0; step<50; step++ ) {
          single.propagator->step();
          list.propagator->step();
        }
        CHECK_MSG(single.propagator->phi()[i] == list.propagator->phi()[i],
                  type << (second_order ? " (SecondOrder)" : "") << " node " << i
                  << ": " << list.propagator->phi()[i] << " != " << single.propagator->phi()[i]);
      }
    }
  }
}

TEST(propagator_rejects_wrong_phi) {
  for( const std::string& type : kTypes ) {
    CHECK_MSG(exits_with_failure([&]{ Model m(type, "phi: 1 2 3"); }),
              type << ": three values for four nodes were accepted");
    CHECK_MSG(exits_with_failure([&]{ Model m(type, "phi: 1 2 3 4 5"); }),
              type << ": five values for four nodes were accepted");
    CHECK_MSG(exits_with_failure([&]{ Model m(type, "phi: steady"); }),
              type << ": 'steady' was accepted");
  }
  // Sanity check of the harness: a valid phi does not exit.
  CHECK(!exits_with_failure([&]{ Model m("Wave", "phi: 1 2 3 4"); }));
}
