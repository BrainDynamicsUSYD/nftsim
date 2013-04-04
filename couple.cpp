#include<iostream>
using std::cerr;
using std::endl;
#include "couple.h"

void Couple::init( Configf& configf )
{
  configf.next("nu");
  vector<double> temp = configf.numbers();
  if( temp.size() == 1 ) {
    n.clear(); n.resize(nodes,temp[0]);
    pos = (temp[0]>0)?1:-1;
  }
  else if( temp.size() == uint(nodes) ) {
    n.clear(); n.resize(nodes);
    for( int i=0; i<nodes; i++ )
      n[i] = temp[i];
    pos = 0;
  }
  else {
    cerr<<"nu either has a homogeneous initial value or has one intial value per node."<<endl;
    exit(EXIT_FAILURE);
  }
}

void Couple::restart( Restartf& restartf )
{
}

void Couple::dump( Dumpf& dumpf ) const
{
}

Couple::Couple( int nodes, double deltat, int index, const vector<double>& glu,
        const Propag& prepropag, const Population& postpop )
    : NF(nodes,deltat,index), glu(glu),
      prepropag(prepropag), postpop(postpop), n(nodes)
{
}

Couple::~Couple(void)
{
}

void Couple::step(void)
{
}

double Couple::nuinit( Configf& configf ) const
{
  string buffer = configf.find( label("Couple ",index+1)+"*nu:");
  return atof(buffer.c_str());
}

void Couple::output( Output& output ) const
{
  output("Couple",index+1,"nu",n);
}

const vector<double>& Couple::nu(void) const
{
  return n;
}

const double Couple::operator[]( int node ) const
{
  return nu()[node];
}

bool Couple::excite(void) const
{
  return pos==1;
}
