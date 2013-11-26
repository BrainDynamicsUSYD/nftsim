#include<cstdlib>
#include<iostream>
using std::endl;
#include<cmath>
#include"dendrite.h"

void Dendrite::init( Configf& configf )
{
  if( !configf.next( label("Dendrite ",index+1) )) {
    std::cerr<<"Dendrite "<<index+1<<" not found."<<endl;
    exit(EXIT_FAILURE);
  }
  string buffer("Steady");
  configf.optional("V",buffer);
  if( buffer == "Steady" ) {
    v = precouple.nuphi();
  }
  else
    v.resize(nodes,atof(buffer.c_str()));
  oldnp = v;
  configf.param("alpha",alpha);
  configf.param("beta",beta);

  aminusb = alpha - beta;
  expa = exp(-alpha*deltat);
  expb = exp(-beta*deltat);
  factorab = 1./alpha + 1./beta;
}

Dendrite::Dendrite( int nodes, double deltat, int index,
    const Propag& prepropag, const Couple& precouple )
  : NF(nodes,deltat,index), v(nodes), dvdt(nodes,0), oldnp(nodes),
    prepropag(prepropag), precouple(precouple)
{
}

Dendrite::~Dendrite(void)
{
}

void Dendrite::step(void)
{
  // assume that alpha, beta are constant and nu*phi is linear for the time step
  if(alpha!=beta)
    for(int i=0; i<nodes; i++) {
      dpdt = ( precouple[i] -oldnp[i] )/deltat;
      adjustednp = oldnp[i] -factorab*dpdt -v[i];
      C1 = ( adjustednp*beta -dvdt[i] +dpdt )/aminusb;
      C1expa = C1*expa;
      C2expb = expb*(-C1-adjustednp);
      v[i] = C1expa+C2expb+precouple[i] -factorab*dpdt;
      dvdt[i] = C1expa*(-alpha) +C2expb*(-beta)+dpdt;
      oldnp[i]=precouple[i]; //Save current pulse density for next step
    }
  else // alpha==beta
    for(int i=0; i<nodes; i++) {
      dpdt = ( precouple[i] -oldnp[i] )/deltat;
      adjustednp = oldnp[i] -factorab*dpdt -v[i];
      C1 = dvdt[i] -alpha*adjustednp -dpdt;
      C1dtplusC2 = C1*deltat -adjustednp;
      v[i] = C1dtplusC2*expa +precouple[i] -factorab*dpdt;
      dvdt[i] = (C1-alpha*C1dtplusC2)*expa +dpdt;
      oldnp[i]=precouple[i]; //Save current pulse density for next step
    }
}



void Dendrite::output( int index, Output& output ) const
{
  if( this->index == index )
    output("Dendrite",index+1,"V",v);
}
