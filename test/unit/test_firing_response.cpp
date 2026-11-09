/** @file test_firing_response.cpp
  @brief FiringResponse: the sigmoid and polynomial firing-rate functions.
*/

#include "unit_test.h"

#include "configf.h"
#include "firing_response.h"

#include <cmath>
#include <string>
#include <vector>

namespace {

/// Exposes the soma potential so a test can set it directly.
class TestFiringResponse : public FiringResponse {
 public:
  explicit TestFiringResponse( size_type nodes ) : FiringResponse(nodes, 1e-4, 0) {}
  std::vector<double>& soma_potential() { return v; }
};

} // namespace

TEST(firing_response_sigmoid) {
  const double theta = 0.01292, sigma = 0.0038, qmax = 340.0;
  unit::TempConfig conf("Firing: Function: Sigmoid Theta: 0.01292 Sigma: 0.0038 Qmax: 340.0\n");
  Configf configf(conf.path());
  TestFiringResponse fr(6);
  configf.param("Firing", fr);

  const double ln3 = std::log(3.0);
  std::vector<double>& v = fr.soma_potential();
  v = {theta, theta + sigma*ln3, theta - sigma*ln3, 1.0, -1.0, 0.0};
  std::vector<double> q(6, -1.0);
  fr.fire(q);
  CHECK_CLOSE(q[0], qmax/2.0, 1e-12);       // half maximum at threshold
  CHECK_CLOSE(q[1], 0.75*qmax, 1e-12);      // 1/(1+1/3) = 3/4
  CHECK_CLOSE(q[2], 0.25*qmax, 1e-12);      // 1/(1+3) = 1/4
  CHECK_CLOSE(q[3], qmax, 1e-12);           // saturates
  CHECK_CLOSE(q[4], 0.0, 1e-12);
  CHECK_CLOSE(q[5], qmax/(1.0 + std::exp(theta/sigma)), 1e-12);
}

TEST(firing_response_polynomials) {
  struct Case { const char* conf; double expected_at_2; };
  const Case cases[] = {
    {"Firing: Function: Linear a: 3.0 b: 1.5\n", 3.0*2 + 1.5},
    {"Firing: Function: Quadratic a: 3.0 b: 1.5 c: -2.0\n", 3.0*4 + 1.5*2 - 2.0},
    {"Firing: Function: Cubic a: 3.0 b: 1.5 c: -2.0 d: 0.25\n", 3.0*8 + 1.5*4 - 2.0*2 + 0.25},
  };
  for( const Case& c : cases ) {
    unit::TempConfig conf(c.conf);
    Configf configf(conf.path());
    TestFiringResponse fr(2);
    configf.param("Firing", fr);
    fr.soma_potential() = {2.0, 0.0};
    std::vector<double> q(2, -1.0);
    fr.fire(q);
    CHECK_MSG(std::fabs(q[0] - c.expected_at_2) < 1e-12, c.conf << " gave " << q[0]);
  }
}
