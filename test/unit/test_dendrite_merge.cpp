/** @file test_dendrite_merge.cpp
  @brief Merging dendrites that share alpha, beta and type leaves the soma potential unchanged.

  FiringResponse integrates dendrites of the same linear type (Dendrite or
  DendriteIntegral) with the same alpha and beta as one dendrite driven by the
  sum of their inputs (FiringResponse::mergeDendrites()). The dendritic
  equation is linear, so the soma potential must match the unmerged one up to
  round-off. A population with mixed groups is driven by different
  time-varying inputs, from different numeric initial V, with both schemes.
*/

#include "unit_test.h"

#include "configf.h"
#include "coupling.h"
#include "firing_response.h"
#include "output.h"
#include "population.h"
#include "propagator.h"

#include <cmath>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace {

const std::size_t kNodes = 3;
const double kDeltat = 1e-4;
const std::size_t kDendrites = 10;
const double kTwoPi = 6.283185307179586;

/// A Coupling whose nu*phi is set directly by the test.
class FixedCoupling : public Coupling {
 public:
  FixedCoupling( size_type nodes, double deltat, size_type index, const Propagator& prepropag,
                 const Population& postpop )
    : Coupling(nodes, deltat, index, prepropag, postpop) {}
  void set( const std::vector<double>& nuphi ) { P = nuphi; }
};

/// Ten dendrites in five groups: {1, 2, 5} RK4 at (50, 200); {3, 10} RK4 at
/// (83, 769); {4, 6} Integral at (50, 200); {7, 8} Integral with alpha ==
/// beta; and 9, a Ramp dendrite, which is never merged. Each starts at a
/// different numeric V.
const char* kConfig =
  "Firing: Function: Linear a: 1.0 b: 0.0\n"
  " Dendrite 1: V: 0.001 alpha: 50 beta: 200\n"
  " Dendrite 2: V: 0.002 alpha: 50 beta: 200\n"
  " Dendrite 3: V: -0.0005 alpha: 83 beta: 769\n"
  " Dendrite 4: Integral - V: 0.0015 alpha: 50 beta: 200\n"
  " Dendrite 5: V: 0.0007 alpha: 50 beta: 200\n"
  " Dendrite 6: Integral - V: 0.0003 alpha: 50 beta: 200\n"
  " Dendrite 7: Integral - V: 0.001 alpha: 100 beta: 100\n"
  " Dendrite 8: Integral - V: 0 alpha: 100 beta: 100\n"
  " Dendrite 9: Ramp - V: 0.0004 alpha: 50 beta: 200 alpha2: 60 beta2: 250 t1: 0.002 t2: 0.004 t3: 0.006 t4: 0.008\n"
  " Dendrite 10: V: 0 alpha: 83 beta: 769\n";

/// The populations, propagator and couplings the dendrites refer to.
struct Network {
  Population pre;
  Population post;
  Propagator propagator;
  std::vector<std::unique_ptr<FixedCoupling>> couplings;
  Network() : pre(kNodes, kDeltat, 0), post(kNodes, kDeltat, 1),
              propagator(kNodes, kDeltat, 0, pre, post, kNodes, "Torus") {
    for( std::size_t c=0; c<kDendrites; c++ ) {
      couplings.emplace_back(new FixedCoupling(kNodes, kDeltat, c, propagator, post));
    }
  }
  /// A different smooth input per coupling and node at step n.
  void setInputs( int n ) {
    for( std::size_t c=0; c<kDendrites; c++ ) {
      std::vector<double> np(kNodes);
      const double cd = static_cast<double>(c);
      for( std::size_t j=0; j<kNodes; j++ ) {
        const double t = n*kDeltat;
        const double jd = static_cast<double>(j);
        np[j] = 1e-3*(1.0 + cd + jd) * std::sin(kTwoPi*(3.0 + 2.0*cd + jd)*t + 0.3*cd)
                + 2e-4*(c % 3 == 0 ? -1.0 : 1.0);
      }
      couplings[c]->set(np);
    }
  }
};

/// A FiringResponse with the dendrites of kConfig, built as Solver does.
std::unique_ptr<FiringResponse> makeResponse( Network& net, const std::string& path,
                                              bool merge, bool second_order ) {
  std::unique_ptr<FiringResponse> fr(new FiringResponse(kNodes, kDeltat, 0));
  Configf configf(path.c_str());
  for( std::size_t c=0; c<kDendrites; c++ ) {
    fr->add2Dendrite(c, net.propagator, *net.couplings[c], configf);
  }
  fr->allowMerging(merge);
  fr->useSecondOrder(second_order);
  configf.param("Firing", *fr);
  fr->initFromInputs();
  return fr;
}

/// Steps merged and unmerged responses side by side and returns the largest
/// soma-potential difference relative to the largest |V|.
double mergedDifference( bool second_order, std::size_t& stepped_merged,
                         std::size_t& stepped_plain ) {
  unit::TempConfig conf(kConfig);
  Network net;
  net.setInputs(0);
  auto merged = makeResponse(net, conf.path(), true, second_order);
  auto plain = makeResponse(net, conf.path(), false, second_order);
  double diff = 0.0, scale = 0.0;
  for( int n=0; n<2000; n++ ) {
    net.setInputs(n);
    merged->step();
    plain->step();
    for( std::size_t j=0; j<kNodes; j++ ) {
      diff = std::fmax(diff, std::fabs(merged->V()[j] - plain->V()[j]));
      scale = std::fmax(scale, std::fabs(plain->V()[j]));
    }
  }
  stepped_merged = merged->steppedDendrites();
  stepped_plain = plain->steppedDendrites();
  return diff/scale;
}

} // namespace

TEST(dendrite_merge_soma_potential) {
  for( bool second_order : {false, true} ) {
    std::size_t stepped_merged = 0, stepped_plain = 0;
    const double rel = mergedDifference(second_order, stepped_merged, stepped_plain);
    CHECK_MSG(rel < 1e-12, "second order " << second_order << ": relative difference " << rel);
    CHECK_EQ(stepped_plain, kDendrites);
    CHECK_EQ(stepped_merged, std::size_t(5)); // the five groups
  }
}

TEST(dendrite_merge_keeps_output_dendrite) {
  // A dendrite whose V is output is not merged, so its V is exactly that of
  // the unmerged dendrite; the rest of its group ({2, 5}) is still merged.
  unit::TempConfig conf(kConfig);
  Network net;
  net.setInputs(0);
  auto merged = makeResponse(net, conf.path(), true, false);
  auto plain = makeResponse(net, conf.path(), false, false);
  Output out_merged(""), out_plain("");
  merged->outputDendrite(0, out_merged);
  plain->outputDendrite(0, out_plain);
  auto outlets_merged = out_merged.release();
  auto outlets_plain = out_plain.release();
  CHECK_EQ(outlets_merged.size(), std::size_t(1));
  CHECK_EQ(outlets_plain.size(), std::size_t(1));
  double rel = 0.0, scale = 0.0;
  bool same = true;
  for( int n=0; n<500; n++ ) {
    net.setInputs(n);
    merged->step();
    plain->step();
    for( std::size_t j=0; j<kNodes; j++ ) {
      same = same && std::equal_to<double>()(outlets_merged[0]->field[j], outlets_plain[0]->field[j]);
      rel = std::fmax(rel, std::fabs(merged->V()[j] - plain->V()[j]));
      scale = std::fmax(scale, std::fabs(plain->V()[j]));
    }
  }
  CHECK_MSG(same, "output dendrite's V differs from the unmerged dendrite's");
  CHECK_MSG(rel/scale < 1e-12, "relative difference " << rel/scale);
  CHECK_EQ(merged->steppedDendrites(), std::size_t(6));
}
