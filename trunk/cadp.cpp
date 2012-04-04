#include<cmath>
#include "cadp.h"

double CaDP::sig( double x, double beta ) const
{
  return 1/(1+exp(-beta*x));
}

double CaDP::potentiate(double Ca) const
{
  return nu_th +nu_ltp*sig(Ca-.66e-6,2e7);
}

double CaDP::depress(double Ca) const
{
  return nu_th +nu_ltd*sig(Ca-.45e-6,2e7) -nu_ltd*sig(Ca-.66e-6,2e7);
}

void CaDP::init( Configf& configf )
{
  Couple::init(configf); // initialize nu and excite()
  configf.param("Nu_max",nu_max);
  configf.param("Threshold",nu_th);
  configf.param("LTD",nu_ltd);
  configf.param("LTP",nu_ltp);
  configf.param("B",B);
  if( !configf.optional("tCa",tCa) )
    tCa = 50e-3;
}

void CaDP::restart( Restartf& restartf )
{
}

void CaDP::dump( Dumpf& dumpf ) const
{
  /*dumpf<<"Nu: "<<nu[0]<<" ";
  dumpf<<"Nu_max: "<<nu_max<<" ";
  dumpf<<"Threshold"<<nu_th<<" ";
  dumpf<<"LTD: "<<nu_ltd<<" ";
  dumpf<<"LTP: "<<nu_ltp<<" ";
  dumpf<<"B: "<<B<<" ";*/
}

CaDP::CaDP( int nodes, double deltat, int index, const vector<double>& glu,
        const Population& prepop, const Population& postpop )
  : Couple(nodes,deltat,index,glu,prepop,postpop), Ca(nodes,0),binding(nodes,0)
{
}

CaDP::~CaDP(void)
{
}

void CaDP::step(void)
{
  const vector<double>& V = postpop.V();
  for( int i=0; i<nodes; i++ ) {
    binding[i] = sig( glu[i] -200e-6, B );
    double dCa = deltat*(2e-3*binding[i])
      *(195e-3-V[i])*sig( V[i]-45.5e-3,62 )
      -Ca[i]/tCa*deltat;
    if( Ca[i]+dCa < 0 )
      Ca[i] = 0;
    else
      Ca[i] += dCa;
    double dnu = deltat*(
            potentiate(Ca[i])*(nu_max-n[i]) -depress(Ca[i])*n[i] );
    if( pos*( n[i]+dnu ) < 0 )
      n[i] = 0;
    else
      n[i] += dnu;
  }
}

vector<Output*> CaDP::output(void) const
{
  vector<Output*> temp;
  temp.push_back( new Output( label("Couple.",index+1)+".nu", nu() ) );
  temp.push_back( new Output( label("Couple.",index+1)+".Ca", Ca ) );
  temp.push_back( new Output( label("Couple.",index+1)+".B", binding ) );
  return temp;
}
