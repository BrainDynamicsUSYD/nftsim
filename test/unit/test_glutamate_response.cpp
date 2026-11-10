/** @file test_glutamate_response.cpp
  @brief GlutamateResponse: initial glutamate concentration.
*/

#include "unit_test.h"

#include "configf.h"
#include "glutamate_response.h"

#include <vector>

TEST(glutamate_initial_concentration) {
  // Glu::init() used to resize() an already-sized vector, so the intended
  // initial concentration of 1e-6 was ignored and glutamate started at 0.
  unit::TempConfig conf("Firing: GlutamateResponse - Lambda: 150e-6 tGlu: 30e-3 "
                        "Function: Sigmoid Theta: 13e-3 Sigma: 3.8e-3 Qmax: 340\n");
  Configf configf(conf.path());
  GlutamateResponse gr(4, 1e-4, 0);
  configf.param("Firing", gr);
  const std::vector<double>& glu = gr.glu();
  CHECK_EQ(glu.size(), static_cast<std::vector<double>::size_type>(4));
  for( double g : glu ) {
    CHECK_CLOSE(g, 1.0e-6, 1e-18);
  }
}
