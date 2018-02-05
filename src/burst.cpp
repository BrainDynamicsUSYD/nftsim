/** @file burst.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "burst.h"      // BurstResponse;

// Other nftsim headers
#include "array.h"      // Array;
#include "configf.h"    // Configf;
#include "dendrite.h"   // Dendrite;
#include "output.h"     // Output;

// C++ standard library headers
#include <cmath>     // std::exp;
#include <vector>   // std::vector;
using std::exp;
using std::vector;

void BurstResponse::init( Configf& configf ) {
  configf.param("Sigma",sigma);
  configf.param("Qmax",Q_max);
  double hinit;
  configf.param("Htilde",hinit);
  double xinit;
  configf.param("Xtilde",xinit);

  Veff = -0.066;  // from Matlab scripts
  Vx = 0.14;
  Vk = -0.095;

  double temp;
  configf.param("gX",temp);
  gX.push_back(temp);
  configf.param("gH",temp);
  gH.push_back(temp);

  gX1 = gX[0];
  gH1 = gH[0];

  configf.param("ic",ic);
  configf.param("taux",taux);
  configf.param("tauh",tauh);
  configf.param("ax",ax);
  configf.param("mu",mu);

  if (configf.optional("gX2",gX2)) {
    configf.optional("tx1",tx1);
    configf.optional("tx2",tx2);
    configf.optional("tx3",tx3);
    configf.optional("tx4",tx4);
  } else {
    gX2 = gX1;
  }
  if (configf.optional("gH2",gH2)) {
    configf.optional("th1",th1);
    configf.optional("th2",th2);
    configf.optional("th3",th3);
    configf.optional("th4",th4);
  } else {
    gH2 = gH1;
  }

  for(Array<Dendrite>::size_type i = 0; i<dendrites.size(); i++) {
    configf>>*dendrites[i];  // array.h vector<T*> vector of ptrs info and inserts into this from the ifstream configf
  }
// read config file- see firing_response.init()
// initialize any other private variables (i.e. current)

  htilde.resize(nodes, hinit);
  xtilde.resize(nodes, xinit);
  modtheta.resize(nodes, 0.0);
  thetatemp.resize(nodes, 0.0);
  qfiring.resize(nodes, 0.0);
  xinfinity.resize(nodes, 0.0);

  de->init(xinit, hinit); // call BurstResponse::BurstResponseDE::init
  de->taux = taux;
  de->tauh = tauh;
}

void BurstResponse::BurstResponseDE::init(const double xinit, const double hinit) {
  variables[0].clear();
  variables[1].clear();
  variables[0].resize(nodes, xinit);
  variables[1].resize(nodes, hinit);
}

BurstResponse::BurstResponse( size_type nodes, double deltat, size_type index )  // constructor
  : FiringResponse(nodes,deltat,index) {
  de = new BurstResponseDE(nodes, deltat);
  rk4 = new RK4(*de);
  time = 0;
}

BurstResponse::~BurstResponse() {
  delete de;
  delete rk4;
}

void BurstResponse::step() {
  time++;
  double sec_time = time * deltat;
  if (sec_time <= tx1 || gX2 == gX1) {
    // Do nothing here!
  } else if (sec_time > tx1 && sec_time <= tx2) {
    gX[0] =  gX1 + (sec_time - tx1) * (gX2 - gX1) / (tx2 - tx1);
  } else if (sec_time > tx2 && sec_time <= tx3) {
    gX[0] = gX2;
  } else if (sec_time >tx3 && sec_time <= tx4) {
    gX[0] =  gX2 - ((sec_time - tx3) * (gX2 - gX1) / (tx4 - tx3));
  } else if (sec_time > tx4) {
    gX[0] = gX1;
  }

  if (sec_time <= th1 || gH2 == gH1) {
    // Do nothing here!
  } else if (sec_time > tx1 && sec_time <= tx2) {
    gH[0] =  gH1 + (sec_time - th1) * (gH2 - gH1) / (th2 - th1);
  } else if (sec_time > th2 && sec_time <= th3) {
    gH[0] = gH2;
  } else if (sec_time >th3 && sec_time <= th4) {
    gH[0] =  gH2 - ((sec_time - th3) * (gH2 - gH1) / (th4 - th3));
  } else if (sec_time > th4) {
    gH[0] = gH1;
  }

  FiringResponse::step(); // sum soma potential

  ib = -gX[0] * (Veff -Vx) / 3.0;
  ia = ib - gH[0] * (Veff - Vk);

  for( size_type i=0; i<nodes; i++ ) {
    thetatemp[i] = (ic-3.0*ib*xtilde[i]+(ib-ia)*htilde[i])/mu;
    qfiring[i] = Q_max/(1.0 + exp(-(v[i]-thetatemp[i])/sigma));
    xinfinity[i] = qfiring[i]*ax;
    if(xinfinity[i] < 0.0) {
      xinfinity[i] = 0.0;
    }
  }
  de->xinfinity = xinfinity;

  //Integrate the burst response one step forward in time.
  rk4->step();
  //Copy the burst response from the updated DE to the local variables.
  for( size_type i=0; i<nodes; i++ ) {
    xtilde[i] = (*de)[0][i];
    htilde[i] = (*de)[1][i];
  }

  for(size_type i=0; i<nodes; i++ ) {
    modtheta[i] = (ic-3.0*ib*xtilde[i]+(ib-ia)*htilde[i])/mu; // use updated y to calculate theta
  }
}

void BurstResponse::output(Output& output) const {
  // write a field into an output file
  output.prefix("Pop", index+1);
  output("HTilde", htilde);
  output("XTilde", xtilde);
  output("modtheta", modtheta);
  output("V", v);
  output("gX", gX);
  output("gH", gH);
}

void BurstResponse::fire( vector<double>& Q ) const {
  for(size_type i=0; i<nodes; i++ ) {
    Q[i] = Q_max/( 1.0 + exp( -(v[i]-modtheta[i])/sigma ) );
  }
}

/**
  @brief Computes the derivatives of the BurstResponse function.

  The BurstResponse equation is given by:
  \f{eqnarray*}{
    equation &=& goes here
  \f}
*/
void BurstResponse::BurstResponseDE::rhs( const vector<double>& y, vector<double>& dydt, size_type n ){
  //function returns the derivatives for modulated threshold eqns
  //y[1] is Htilde , y[0] is Xtilde

  dydt[0] = (xinfinity[n] - y[0])/taux;
  dydt[1] = (3.0*y[0] - y[1])/tauh;
}
