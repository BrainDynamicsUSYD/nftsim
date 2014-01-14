#include<cmath>
#include"fs.h"

void FS::FSde::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y = { Q, H, dHdt }
  // Q, leave alone
  dydt[0] = 0;
  // H governed by a 2nd order DE
  dydt[1] = y[2]; // == dH/dt
  dydt[2] = ( 3*a_x*y[0] -y[1] -(tau_H+tau_X)*y[2] )/tau_H/tau_X; // d2H/dt2
}

void FS::init( Configf& configf )
{
  configf.param("I_a",I_a);
  configf.param("I_b",I_b);
  configf.param("I_c",I_c);
  configf.param("tau_X",de.tau_X);
  configf.param("tau_H",de.tau_H);
  configf.param("A",A);
  configf.param("a_x",de.a_x);
  configf.param("mu",mu);
  for( size_t i=0; i<dendrites.size(); i++ )
    configf>>*dendrites[i];
  de[2].clear(); de[2].resize(nodes,.01);
}

FS::FS( int nodes, double deltat, int index )
    : QResponse(nodes,deltat,index), de(nodes,deltat), rk4(de), I_theta(nodes)
{
}

FS::~FS(void)
{
}

void FS::step(void)
{
  QResponse::step(); // step through dendrites, then sump up soma potential
  fire( de[0] ); // Q is needed for calculation of H
  rk4.step();
  for( int i=0; i<nodes; i++ )
    I_theta[i] = I_c -I_a*de[1][i] -I_b*de.tau_H*de[2][i]; // I_theta
}

void FS::fire( vector<double>& Q ) const
{
  for( int i=0; i<nodes; i++ ) {
    double I = mu*v[i] -I_theta[i];
    if( I>0 ) Q[i] = A*sqrt(I/I_c);
    else Q[i] = 0;
  }
}

void FS::output( Output& output ) const
{
  output.prefix("Pop",index+1);
  output("V",V());
  output("H",de[1]);
  output("Q",de[0]);
  output("I_theta",I_theta);
}
