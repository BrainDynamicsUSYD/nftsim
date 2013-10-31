#include<iostream>
using std::cerr;
using std::endl;
#include<cmath>
#include"tau.h"

void Tau::init( Configf& configf )
{
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    if( remainder(temp[0],deltat) >deltat ) {
      cerr<<"Value of tau not divisible by Deltat!"<<endl;
      exit(EXIT_FAILURE);
    }
    m.resize(1,temp[0]/deltat);
    max = m[0];
  }
  else if( temp.size() == uint(nodes) ) {
    for( int i=0; i<nodes; i++ ) {
      if( remainder(temp[0],deltat) >deltat ) {
        std::cerr<<"Value of tau not divisible by Deltat!"<<endl;
        exit(EXIT_FAILURE);
      }
      m[i] = int(temp[i]/deltat);
      if( m[i]>max )
        max = m[i];
    }
  }
  else {
    std::cerr << "The number of Tau has to be one or the same as the number of nodes." << endl;
    exit(EXIT_FAILURE);
  }
}

Tau::Tau( int nodes, double deltat, int index ) : NF(nodes,deltat,index), max(0)
{
}

Tau::~Tau()
{
}

void Tau::step(void)
{
}
