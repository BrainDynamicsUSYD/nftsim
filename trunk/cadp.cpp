#include<cmath>
#include "cadp.h"

double CaDP::sig( double x, double beta ) const
{
  return 1/(1+exp(-beta*x));
}

double CaDP::x(double Ca) const
{
  return th +ltp*sig(Ca-pth,2e7);
  //return th +(ltp-ltd)*sig(Ca-.66e-6,2e7) +ltd/1e-6*Ca;
}

double CaDP::y(double Ca) const
{
  return th +ltd*sig(Ca-dth,2e7) -ltd*sig(Ca-pth,2e7);
}

void CaDP::init( Configf& configf )
{
  Couple::init(configf); // initialize nu and excite()
  configf.param("nu_max",max);
  configf.param("Threshold",th);
  configf.param("LTD",ltd);
  configf.param("LTP",ltp);
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
  dumpf<<"Nu_max: "<<max<<" ";
  dumpf<<"Threshold"<<th<<" ";
  dumpf<<"LTD: "<<ltd<<" ";
  dumpf<<"LTP: "<<ltp<<" ";
  dumpf<<"B: "<<B<<" ";*/
}

CaDP::CaDP( int nodes, double deltat, int index, const vector<double>& glu,
        const Population& prepop, const Population& postpop )
  : Couple(nodes,deltat,index,glu,prepop,postpop), binding(nodes,0), Ca(nodes,0)
{
  dth = .45e-6;
  pth = .66e-6;
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
            x(Ca[i])*(max-n[i]) -y(Ca[i])*n[i] );
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
