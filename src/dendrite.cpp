/** @file dendrite.cpp
  @brief Defines the Dendrite class, which handles dendritic dynamics.

  Dendrite dynamics are applied to incoming activity to postsynaptic Population
  objects. The differential equation defining the dendritic dynamics are in the
  member function Dendrite::DendriteDE::rhs.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "dendrite.h"   // Dendrite;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "de.h"         // RK4;
#include "output.h"     // Output;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <iostream> // std::endl; std::cerr;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::string;
using std::vector;

/**
  @brief Computes the derivatives of the dendritic response function.

  The dendritic response function is given by:
  \f{eqnarray*}{
    \frac{dv}{dt}&=&W \\
    \frac{dW}{dt}&=&\left(\nu\phi - V - \left(\frac{1}{\alpha} + \frac{1}{\beta}\right) W\right) \alpha \beta \\
    \frac{d\nu\phi}{dt}&=&0
  \f}
*/
void Dendrite::DendriteDE::rhs( const vector<double>& y, vector<double>& dydt, size_type /*unused*/ ) {
  // y = {V,W==dv/dt,nuphi}
  // dydt = {dv/dt==W, dW/dt==d^2V/dt^2,dnuphi/dt}  d(nuphi)/dt from precouple
  dydt[0] = y[1];

  dydt[1] = (y[2] - y[0] - factorab * y[1]) * alphaxbeta;

  dydt[2] = 0.0;
}

void Dendrite::init( Configf& configf ) {
  if( !configf.next( label("Dendrite ",index+1) )) {
    std::cerr<<"Dendrite "<<index+1<<" not found."<<std::endl;
    exit(EXIT_FAILURE);
  }
  string buffer("Steady");
  configf.optional("V",buffer);
  if( buffer == "Steady" ) {
    for( size_type i=0; i<nodes; i++ ) {
      v[i] = precouple.nuinit(configf)*prepropag.phiinit(configf);
    }
  } else {
    v.resize(nodes,atof(buffer.c_str()));
  }

  configf.param("alpha",alpha);
  configf.param("beta",beta);

  // Initialize constant factors to speed up computation.
  factorab = 1./alpha + 1./beta;
  alphaxbeta = alpha * beta;

  de->init(v[0]); // call Dendrite::DendriteDE::init
  de->factorab = factorab;
  de->alphaxbeta = alphaxbeta;
}

void Dendrite::DendriteDE::init(const double vinit) {
  variables[0].clear();
  variables[1].clear();
  variables[2].clear();
  variables[0].resize(nodes, vinit);
  variables[1].resize(nodes, 0.0);
  variables[2].resize(nodes, 0.0);
}

Dendrite::Dendrite( size_type nodes, double deltat, size_type index,
                    const Propagator& prepropag, const Coupling& precouple )
  : NF(nodes,deltat,index), v(nodes), prepropag(prepropag), precouple(precouple) {
  de = new DendriteDE(nodes, deltat);
  rk4 = new RK4(*de);
}

Dendrite::~Dendrite() {
  delete de;
  delete rk4;
}

void Dendrite::step() {
  //Copy the \nu\phi into their corresponding variable in the DE.
  for( size_type i=0; i<nodes; i++ ) {
    (*de)[2][i] = precouple[i]; // \nu\phi
  }
  //Integrate the dendritic response one step forward in time.
  rk4->step();
  //Copy the voltages from the updated DE to the local variable v.
  for( size_type i=0; i<nodes; i++ ) {
    v[i] = (*de)[0][i]; // Voltage
  }
}

void Dendrite::output( Output& output ) const {
  output("Dendrite",index+1,"V",v);
}
