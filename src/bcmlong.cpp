/** @file bcmlong.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "bcmlong.h"    // BCMLong;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "de.h"         // RK4;
#include "make_unique.h" // nftsim::make_unique;
#include "output.h"     // Output;

// C++ standard library headers
#include <iostream> // std::endl; std::cerr; std::streamoff;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::vector;

void BCMLong::init( Configf& configf ) {
  std::streamoff position1 = configf.tell();
  // first obtain parameters
  de_2d[0]->init(configf);
  for( size_type i=0; i<nodes; i++ ) {
    *de_2d[i] = *de_2d[0];
  }

  // then re-read nu-matrix
  std::streamoff position2 = configf.tell();
  configf.seek(position1);

  configf.next("nu");
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    // diagonals == temp[0], off-diagonals ~= 0
    for( size_type i=0; i<nodes; i++ ) {
      for( size_type j=0; j<nodes; j++ ) {
        (*de_2d[i])[7][j] = (*de_2d[i])[3][j] = cadp::bcmlong_offdiagonal_nu;
      }
    }
    for( size_type i=0; i<nodes; i++ ) {
      (*de_2d[i])[7][i] = (*de_2d[i])[3][i] = temp[0];
    }
    pos = (temp[0]>0)?1:-1;
  } else if( temp.size() == size_t(pow(nodes,2)) ) {
    for( size_type i=0; i<nodes; i++ ) {
      for( size_type j=0; j<nodes; j++ ) {
        (*de_2d[i])[7][j] = (*de_2d[i])[3][j] = temp[i+j*nodes];
      }
    }
    pos = (temp[0]>0)?1:-1;
  } else {
    cerr<<"Matrix of nu either has a homogeneous initial value or takes in node^2 values."<<endl;
    exit(EXIT_FAILURE);
  }

  configf.seek(position2);

  for( size_type j=0; j<nodes; j++ ) {
    P[j] = 0;
    for( size_type i=0; i<nodes; i++ ) {
      P[j] += (*de_2d[i])[7][j]*prepropag[i];
    }
  }
}

BCMLong::BCMLong( size_type nodes, double deltat, size_type index,
                  const Propagator& prepropag, const Population& postpop )
  : BCM(nodes,deltat,index,prepropag,postpop,nullptr), de_2d(nodes), rk4_2d(nodes) {
  for( size_type i=0; i<nodes; i++ ) {
    de_2d [i] = nftsim::make_unique<BCMDE>(nodes,deltat);
    rk4_2d[i] = nftsim::make_unique<RK4>(*de_2d[i]);
  }
}

BCMLong::~BCMLong() = default;

void BCMLong::step() {
  // (*de_2d[i])[k][j] : i=from node i, j=to node j, k=kth field of y
  // y == { binding, H, Ca, nutilde, x, y, dnudt, nu, nu_Ca }

  // n2d  [i][j] -> Ca[i][j], n2d[i][j]
  // Ca   [i][j] -> nu_Ca[i][j], B[i], H[j], Ca[i][j]
  // nu_Ca[i][j] -> nu_Ca[i][j], nutilde[i][j], nu[i][j]

  for( size_type i=0; i<nodes; i++ ) {
    for( size_type j=0; j<nodes; j++ ) {
      (*de_2d[i])[0][j] = de_2d[0]->sig( postpop.glu()[i] -de_2d[0]->glu_0, de_2d[0]->B );
      (*de_2d[i])[1][j] = (cadp::nmda_reversal-postpop.V()[j])*de_2d[0]->sig( postpop.V()[j]-cadp::nmda_half_v,cadp::nmda_slope );
    }
  }

  for( size_type i=0; i<nodes; i++ ) {
    de_2d[i]->pot();
    de_2d[i]->dep();
    rk4_2d[i]->step();
  }

  for( size_type j=0; j<nodes; j++ ) {
    P[j] = 0;
    for( size_type i=0; i<nodes; i++ ) {
      P[j] += (*de_2d[i])[7][j]*prepropag[i];
    }
  }
}

void BCMLong::output( Output& output ) const {
  // outputting a 2D matrix rather than a vector:
  // for each "to", output all "from"
  for( size_type i=0; i<nodes; i++ ) {
    output( "C.Matrix", index+1, label("Ca.",        i+1), (*de_2d[i])[2] );
    output( "C.Matrix", index+1, label("nutilde.",   i+1), (*de_2d[i])[3] );
    output( "C.Matrix", index+1, label("nu.",        i+1), (*de_2d[i])[7] );
    output( "C.Matrix", index+1, label("nu_Ca.",     i+1), (*de_2d[i])[8] );
  }
}
