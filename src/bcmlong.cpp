/** @file bcmlong.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#include<cstring>
#include"bcmlong.h"

void BCMLong::init( Configf& configf ) {
  int position1 = configf.tell();
  // first obtain parameters
  de_2d[0]->init(configf);
  for( int i=0; i<nodes; i++ ) {
    *de_2d[i] = *de_2d[0];
  }

  // then re-read nu-matrix
  int position2 = configf.tell();
  configf.seek(position1);

  configf.next("nu");
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    // diagonals == temp[0], off-diagonals ~= 0
    for( int i=0; i<nodes; i++ ) {
      for( int j=0; j<nodes; j++ ) {
        (*de_2d[i])[7][j] = (*de_2d[i])[3][j] = 1e-8;
      }
    }
    for( int i=0; i<nodes; i++ ) {
      (*de_2d[i])[7][i] = (*de_2d[i])[3][i] = temp[0];
    }
    pos = (temp[0]>0)?1:-1;
  } else if( temp.size() == size_t(pow(nodes,2)) ) {
    for( int i=0; i<nodes; i++ ) {
      for( int j=0; j<nodes; j++ ) {
        (*de_2d[i])[7][j] = (*de_2d[i])[3][j] = temp[i+j*nodes];
      }
    }
    pos = (temp[0]>0)?1:-1;
  } else {
    cerr<<"Matrix of nu either has a homogeneous initial value or takes in node^2 values."<<endl;
    exit(EXIT_FAILURE);
  }

  configf.seek(position2);

  for( int j=0; j<nodes; j++ ) {
    P[j] = 0;
    for( int i=0; i<nodes; i++ ) {
      P[j] += (*de_2d[i])[7][j]*prepropag[i];
    }
  }
}

BCMLong::BCMLong( int nodes, double deltat, int index,
                  const Propag& prepropag, const Population& postpop )
  : BCM(nodes,deltat,index,prepropag,postpop), de_2d(nodes), rk4_2d(nodes) {
  for( int i=0; i<nodes; i++ ) {
    de_2d [i] = new BCMDE(nodes,deltat);
    rk4_2d[i] = new RK4(*de_2d[i]);
  }
}

BCMLong::~BCMLong() {
  for( int i=0; i<nodes; i++ ) {
    delete de_2d[i];
    delete rk4_2d[i];
  }
}

void BCMLong::step() {
  // (*de_2d[i])[k][j] : i=from node i, j=to node j, k=kth field of y
  // y == { binding, H, Ca, nutilde, x, y, dnudt, nu, nu_Ca }

  // n2d  [i][j] -> Ca[i][j], n2d[i][j]
  // Ca   [i][j] -> nu_Ca[i][j], B[i], H[j], Ca[i][j]
  // nu_Ca[i][j] -> nu_Ca[i][j], nutilde[i][j], nu[i][j]

  for( int i=0; i<nodes; i++ ) {
    for( int j=0; j<nodes; j++ ) {
      (*de_2d[i])[0][j] = de_2d[0]->sig( postpop.glu()[i] -de_2d[0]->glu_0, de_2d[0]->B );
      (*de_2d[i])[1][j] = (195e-3-postpop.V()[j])*de_2d[0]->sig( postpop.V()[j]-45.5e-3,62 );
    }
  }

  for( int i=0; i<nodes; i++ ) {
    de_2d[i]->pot();
    de_2d[i]->dep();
    rk4_2d[i]->step();
  }

  for( int j=0; j<nodes; j++ ) {
    P[j] = 0;
    for( int i=0; i<nodes; i++ ) {
      P[j] += (*de_2d[i])[7][j]*prepropag[i];
    }
  }
}

void BCMLong::output( Output& output ) const {
  // outputting a 2D matrix rather than a vector:
  // for each "to", output all "from"
  for( int i=0; i<nodes; i++ ) {
    output( "C.Matrix", index+1, label("Ca.",        i+1), (*de_2d[i])[0] );
    output( "C.Matrix", index+1, label("nutilde.",   i+1), (*de_2d[i])[3] );
    output( "C.Matrix", index+1, label("nu.",        i+1), (*de_2d[i])[7] );
    output( "C.Matrix", index+1, label("nu_Ca.",     i+1), (*de_2d[i])[1] );
  }
}
