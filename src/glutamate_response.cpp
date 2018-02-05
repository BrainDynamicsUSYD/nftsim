/** @file glutamate_response.cpp
  @brief Implements the GlutamateResponse class, for the soma response of neural populations.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "glutamate_response.h"  // GlutamateResponse;

// Other nftsim headers
#include "array.h"      // Array;
#include "configf.h"    // Configf;
#include "dendrite.h"   // Dendrite;
#include "firing_response.h"  // FiringResponse;

// C++ standard library headers
#include <vector>   // std::vector;
using std::vector;

void GlutamateResponse::init( Configf& configf ) {
  // Glu::init() must be called before FiringResponse::init() as the latter
  // expects to be able to consume the Dendrite lines following Firing.
  glu_m.init(configf);

  FiringResponse::init(configf);

}

void GlutamateResponse::Glu::init( Configf& configf ) {
  configf.optional("Lambda", Lambda);
  configf.optional("tGlu", tGlu);
  variables[0].resize(nodes, 1.0e-6);
  variables[1].resize(nodes);
}

void GlutamateResponse::Glu::rhs( const vector<double>& y, vector<double>& dydt, size_type /*unused*/ ) {
  // y = { glu, excitatory phi }
  // glu
  dydt[0] = Lambda*y[1] -y[0]/tGlu;
  if( y[0]+dydt[0]*deltat <0 ) {
    dydt[0] = -y[0];
  }
  // excitatory phi, leave alone
  dydt[1] = 0;
}

GlutamateResponse::GlutamateResponse( size_type nodes, double deltat, size_type index )
  : FiringResponse(nodes, deltat, index), glu_m(nodes, deltat), glu_rk4(glu_m) {
}

GlutamateResponse::~GlutamateResponse() = default;

void GlutamateResponse::step() {
  FiringResponse::step();

  // glutamate dynamics
  if( glu_m.Lambda != 0 ) {
    for( size_type j=0; j<nodes; j++ ) {
      glu_m[1][j] = 0; // reset excitatory phi
    }
    for( Array<Dendrite>::size_type  i=0; i<dendrites.size(); i++ ) {
      if( dendrites[i]->precouple.excite() ) {
        for( size_type j=0; j<nodes; j++ ) {
          glu_m[1][j] += dendrites[i]->prepropag[j]; // put in excitatory phi
        }
      }
    }
    glu_rk4.step();
  }
}

const vector<double>& GlutamateResponse::glu() const {
  return glu_m[0];
}
