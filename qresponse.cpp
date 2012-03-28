#include<cmath>
#include"qresponse.h"
using std::endl;

void QResponse::init( Configf& configf )
{
  if( configf.optional("Theta",theta) ) {
    configf.param("Sigma",sigma);
    configf.param("Qmax",Q_max);
  }
  else {
    configf.param("Gradient",gradient);
    configf.param("Intercept",intercept);
  }
  configf>>dendrites;
}

void QResponse::restart( Restartf& inputf )
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
  if(theta)
    for( int i=0; i<nodes; i++ )
      Q[i] = Q_max/( 1.0F+ exp( -(v[i]-theta)/sigma ) );
  else
    for( int i=0; i<nodes; i++ )
      Q[i] = v[i]*gradient +intercept;
}

const vector<double>& QResponse::V(void) const
{
  return v;
}
