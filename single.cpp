#include<iostream>
using std::cerr;
using std::endl;
#include"single.h"

void Single::Singlede::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y = { nu*phi, V, W }
  // nu*phi, leave alone
  dydt[0] = 0;
  // V
  //dydt[1] = pow(50e-3,2)*y[1] -pow(y[1],3)/3 -1e-0*y[2] +y[0]; dydt[1]*=timescale;
  dydt[1] = y[1] -pow(y[1],3)/3 -y[2] +y[0]; dydt[1]*=timescale;
  // W
  //dydt[2] = .08*( y[1] -.08*y[2] +40e-3 ); dydt[2]*=timescale;
  dydt[2] = .08*( y[1] +.7 -.8*y[2] ); dydt[2]*=timescale;
}

void Single::init( Configf& configf )
{
  double Vinit = 0; if( !configf.optional("V",Vinit) ) {
    cerr<<"Population "<<index+1
        <<" is a Betz cell, so that it requires a 'V' parameter"<<endl;
    exit(EXIT_FAILURE);
  }
  de[1].clear(); de[1].resize(nodes,Vinit);
  double Winit = 0; if( configf.optional("W",Winit) )
  de[2].clear(); de[2].resize(nodes,Winit);
  configf.optional("Timescale",de.timescale);
  /*if( !qhistory.size() )
    qhistory.push_back( vector<double>(nodes,Vinit) );
  else
    for( size_t i=0; i<qhistory.size(); i++ )
      qhistory[i].resize(nodes,Vinit);*/
  settled = true;
}

void Single::restart( Restartf& restartf )
{
}

void Single::dump( Dumpf& dump ) const
{
}

Single::Single( int nodes, double deltat, int index )
    : Population(nodes,deltat,index), de(nodes,deltat), rk4(de)
{
  de.timescale = 1;
}

Single::~Single()
{
}

void Single::step(void)
{
  for( int j=0; j<nodes; j++ )
      de[0][j] = 0; // reset nu*phi
  for( size_t i=0; i<propags.size(); i++ )
    for( int j=0; j<nodes; j++ )
      de[0][j] += couples[i]->nu()[j] * propags[i]->phi()[j];

  rk4.step(); // step the differential equations
  /*qkey = (qkey+1) % qhistory.size(); // iterate keyring counter
  for( int i=0; i<nodes; i++ )
    qhistory[qkey][i] = de[1][i]; // copy V into history keyring*/
}

void Single::add2Dendrite( int index, const Propag& prepropag,
        const Couple& precouple, Configf& configf )
{
  if( settled ) {
    cerr<<"Population is already settled, no more dendrites can be added!"
        <<endl;
    exit(EXIT_FAILURE);
  }
  propags.push_back( &prepropag );
  couples.push_back( &precouple );
}

double Single::Qinit( Configf& configf ) const
{
  string temp = configf.find( label("Population ",index+1)+"*V:");
  return atof( temp.c_str() );
}

const vector<double>& Single::V(void) const
{
  return de[1];
}

vector<Output*> Single::output(void) const
{
  vector<Output*> temp;
  temp.push_back( new Output( label("Pop.",index+1)+".V", V() ) );
  temp.push_back( new Output( label("Pop.",index+1)+".W", de[2] ) );
  temp.push_back( new Output( label("Pop.",index+1)+".I", de[0] ) );
  return temp;
}
