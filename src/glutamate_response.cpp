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
  // Placeholders: GlutamateResponse::initFromInputs() sets both once the
  // excitatory inputs have their initial values.
  variables[0].assign(nodes, 0.0);
  variables[1].assign(nodes, 0.0);
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
    excitatoryPhi(glu_m[1]); // excitatory phi drives glutamate
    glu_rk4.step();
  }
}

/// Sum, at each node, of the axonal fields phi of the excitatory inputs: the
/// drive that glutamate integrates in step().
void GlutamateResponse::excitatoryPhi( vector<double>& phi_e ) const {
  phi_e.assign(nodes, 0.0);
  for( Array<Dendrite>::size_type i=0; i<dendrites.size(); i++ ) {
    if( dendrites[i]->precouple.excite() ) {
      for( size_type j=0; j<nodes; j++ ) {
        phi_e[j] += dendrites[i]->prepropag[j];
      }
    }
  }
}

/** @brief Starts glutamate at its steady state for the initial inputs.

  Glutamate obeys d(glu)/dt = Lambda*phi_e - glu/tGlu, whose fixed point is
  glu = Lambda*tGlu*phi_e. nftsim starts every other variable at the steady
  state of its initial inputs, so glutamate does too. (It used to start at 0,
  because Glu::init() resized an already-sized vector; it then relaxed
  towards ~4e-5 within a few tGlu in the shipped plasticity configs.) With
  Lambda = 0 glutamate is not integrated and stays at 0.
*/
void GlutamateResponse::initFromInputs() {
  FiringResponse::initFromInputs(); // dendrites first
  vector<double> phi_e;
  excitatoryPhi(phi_e);
  for( size_type j=0; j<nodes; j++ ) {
    glu_m[0][j] = glu_m.Lambda*glu_m.tGlu*phi_e[j];
    glu_m[1][j] = phi_e[j];
  }
}

const vector<double>& GlutamateResponse::glu() const {
  return glu_m[0];
}
