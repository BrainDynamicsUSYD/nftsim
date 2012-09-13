#include<cmath>
#include"qresponse.h"

using std::endl;

void QResponse::init( Configf& configf )
{
  configf.param("Mode",mode);//configf>>mode; 
  if( mode == "Sigmoid" ) {
    configf.param("Theta",theta);
    configf.param("Sigma",sigma);
    configf.param("Qmax",Q_max);
  }
  else if( mode == "Linear") { // Equation is q = av + b;
    configf.param("a",a);
    configf.param("b",b);
  }
  else if( mode == "Quadratic") { // Equation is q = av^2 + b^v + c;
    configf.param("a",a);
    configf.param("b",b);
    configf.param("c",c);
  }
  
  for( size_t i=0; i<dendrites.size(); i++ )
    configf>>*dendrites[i];
}

void QResponse::restart( Restartf& configf )
{
}

void QResponse::dump( Dumpf& dumpf ) const
{
/*  if(theta)
    dumpf << "Sigmoid - Theta: " << theta
      << " Sigma: " << sigma << " Qmax: " << Q_max << endl;
  else
    dumpf << "Linear - Gradient: " << gradient
      << " Intercept: " << intercept << endl;*/
}

QResponse::QResponse( int nodes, double deltat, int index )
    : NF(nodes,deltat,index), v(nodes)
{
}

QResponse::~QResponse()
{
}

void QResponse::step(void)
{
  // step through dendrites, then sum up soma potential
  dendrites.step();
  for( int i=0; i<nodes; i++ )
    v[i] = 0;
  for( size_t i=0; i<dendrites.size(); i++ )
    for( int j=0; j<nodes; j++ )
      v[j] += dendrites[i]->V()[j];
}

void QResponse::add2Dendrite( int index,
    const Propag& prepropag, const Couple& precouple )
{
  dendrites.add( new Dendrite(nodes,deltat,index,prepropag,precouple) );
}

void QResponse::fire( vector<double>& Q ) const
{
  if(mode == "Sigmoid")
    for( int i=0; i<nodes; i++ )
      Q[i] = Q_max/( 1.0F+ exp( -(v[i]-theta)/sigma ) );
  else if (mode == "Linear")
    for( int i=0; i<nodes; i++ )
      Q[i] = v[i]*a +b;
  else if (mode == "Quadratic")
    for( int i=0; i<nodes; i++ )
      Q[i] = v[i]*v[i]*a + v[i]*b +c;
}

const vector<double>& QResponse::V(void) const
{
  return v;
}

vector<Output*> QResponse::output(void) const
{
  vector<Output*> temp;
  temp.push_back( new Output( label("Pop.",index+1)+".V", v ) );
  for( size_t i=0; i<dendrites.size(); i++ ){
    temp.push_back( dendrites[i]->output()[0] );
  }
  return temp;
}
