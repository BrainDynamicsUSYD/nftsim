/** @file coupling.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#include<iostream>
using std::cerr;
using std::endl;
#include "coupling.h"

void Coupling::init( Configf& configf ) {
  configf.next("nu");
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    n.clear();
    n.resize(nodes,temp[0]);
    pos = (temp[0]>0)?1:-1;
  } else if( temp.size() == size_t(nodes) ) {
    n.clear();
    n.resize(nodes);
    for( int i=0; i<nodes; i++ ) {
      n[i] = temp[i];
    }
    pos = 0;
  } else {
    cerr<<"nu either has a homogeneous initial value or has one intial value per node."<<endl;
    exit(EXIT_FAILURE);
  }
  for( int i=0; i<nodes; i++ ) {
    P[i] = n[i]*prepropag.phiinit(configf);
  }
}

Coupling::Coupling( int nodes, double deltat, int index,
                const Propagator& prepropag, const Population& postpop )
  : NF(nodes,deltat,index), prepropag(prepropag), postpop(postpop), n(nodes), P(nodes) {
}

Coupling::~Coupling() = default;

void Coupling::step() {
  for( int i=0; i<nodes; i++ ) {
    P[i] = n[i]*prepropag[i];
  }
}

double Coupling::nuinit( Configf& configf ) const {
  string buffer = configf.find( label("Coupling ",index+1)+"*nu:");
  return atof(buffer.c_str());
}

void Coupling::output( Output& output ) const {
  output("Coupling",index+1,"nu",n);
}

bool Coupling::excite() const {
  return pos==1;
}

const vector<double>& Coupling::nuphi() const {
  return P;
}
