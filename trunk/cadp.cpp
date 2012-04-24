#include<cmath>
#include "cadp.h"

double CaDP::sig( double x, double beta ) const
{
  return 1/(1+exp(-beta*x));
}

double CaDP::x(double Ca) const
{
  return th +ltp*sig(Ca-pth,4e7);
}

double CaDP::y(double Ca) const
{
  return th +ltd*sig(Ca-dth,4e7) -ltd*sig(Ca-pth,4e7);
}

void CaDP::init( Configf& configf )
{
  Couple::init(configf); // initialize nu and excite()
  nhu = n;
  configf.param("nu_max",max);
  configf.param("LTD",dth);
  configf.param("LTP",pth);
  configf.param("Threshold",th);
  configf.param("x",ltp);
  configf.param("y",ltd);
  configf.param("B",B);
  if( !configf.optional("tCa",tCa) )
    tCa = 50e-3;
}

void CaDP::restart( Restartf& restartf )
{
}

void CaDP::dump( Dumpf& dumpf ) const
{
}

CaDP::CaDP( int nodes, double deltat, int index, const vector<double>& glu,
        const Population& prepop, const Population& postpop )
  : Couple(nodes,deltat,index,glu,prepop,postpop), binding(nodes,0), Ca(nodes,0)
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
    double dnhu = deltat*( x(Ca[i])*(max-nhu[i]) -y(Ca[i])*nhu[i] );
    if( pos*( nhu[i]+dnhu ) < 0 )
      nhu[i] = 0;
    else
      nhu[i] += dnhu;
    /*static double p = 0;
    double dp = deltat*( .01*(nhu[i]-n[i]) -2/1*p );
    p += dp; double dn = p*deltat; // delayed, long term plasticity*/
    double dn = dnhu; // "instantaneous" plasticity
    if( pos*( n[i]+dn ) < 0 )
      n[i] = 0;
    else
      n[i] += dn;
  }
}

vector<Output*> CaDP::output(void) const
{
  vector<Output*> temp;
  temp.push_back( new Output( label("Couple.",index+1)+".nu", nu() ) );
  temp.push_back( new Output( label("Couple.",index+1)+".nu_hat", nhu ) );
  temp.push_back( new Output( label("Couple.",index+1)+".Ca", Ca ) );
  temp.push_back( new Output( label("Couple.",index+1)+".B", binding ) );
  return temp;
}
