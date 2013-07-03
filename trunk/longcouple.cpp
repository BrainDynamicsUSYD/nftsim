#include<iostream>
using std::cerr;
using std::endl;
#include"longcouple.h"

void LongCouple::init( Configf& configf )
{
  configf.next("nu");
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    n2d.resize(nodes);
    for( int i=0; i<nodes; i++ )
      n2d[i].resize(nodes,temp[0]);
    pos = (temp[0]>0)?1:-1;
  }
  else if( temp.size() == uint(pow(nodes,2)) ) {
    n2d.resize(nodes);
    for( int i=0; i<nodes; i++ ) {
      n2d[i].resize(nodes);
      for( int j=0; j<nodes; j++ )
        n2d[i][j] = temp[i+j*nodes];
    }
    pos = (temp[0]>0)?1:-1;
  }
  else {
    cerr<<"Matrix of nu either has a homogeneous initial value or takes in node^2 values."<<endl;
    exit(EXIT_FAILURE);
  }
}

LongCouple::LongCouple( int nodes, double deltat, int index,
        const Propag& prepropag, const Population& postpop )
    : Couple(nodes,deltat,index,prepropag,postpop)
{
}

LongCouple::~LongCouple(void)
{
}

void LongCouple::step(void)
{
  for( int i=0; i<nodes; i++ )
    for( int j=0; j<nodes; j++ )
      P[j] = n2d[i][j]*prepropag[i];
}

void LongCouple::output( Output& output ) const
{
  // outputting a 2D matrix rather than a vector:
  // for each "to", output all "from"
  for( int i=0; i<nodes; i++ )
    output("C.Matrix",index+1,label("nu.",i+1),n2d[i]);
}
