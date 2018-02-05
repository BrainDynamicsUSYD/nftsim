/** @file bcm.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "bcm.h"        // BCM;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "de.h"         // RK4;

// C++ standard library headers
#include <vector>   // std::vector;
using std::vector;

void BCM::BCMDE::rhs( const vector<double>& y, vector<double>& dydt, size_type n ) {
  // y == { binding, H, Ca, nutilde, x, y, dnudt, nu, gNMDA }
  CaDE::rhs(y, dydt, n);
  // recalculate dCadt with NMDAR plasticity

  // Ca
  dydt[2] = y[8]*y[0]*y[1] -y[2]/tCa; // replace gnmda with y[8]
  if( y[2]+dydt[2]*deltat < 0 ) {
    dydt[2] = -y[2];
  }
  // gNMDA
  dydt[8] = -y[8]/t_BCM *(y[3]/y[7]-1) +(gnmda_0-y[8])/t_rec;
  if( y[7]==0 ) {
    dydt[8] = 0;
  }
}

void BCM::BCMDE::init( Configf& configf ) {
  CaDE::init(configf);
  configf.param("t_BCM",t_BCM);
  for( size_type i=0; i<nodes; i++ ) {
    variables[8][i] = gnmda;
  }
  gnmda_0 = gnmda;
  if( !configf.optional("t_rec",t_rec) ) {
    t_rec = 1e3;
  }
}

BCM::BCM( size_type nodes, double deltat, size_type index,
          const Propagator& prepropag, const Population& postpop )
  : CaDP(nodes,deltat,index,prepropag,postpop) {
  delete de;
  de = new BCMDE(nodes,deltat);
  delete rk4;
  rk4 = new RK4(*de);
}

BCM::~BCM() = default;

void BCM::output( Output& output ) const {
  output.prefix("Coupling",index+1);
  output("nu",(*de)[7]);
  output("nutilde",(*de)[3]);
  output("Ca",(*de)[2]);
  output("B",(*de)[0]);
  output("gNMDA",(*de)[8]);
}
