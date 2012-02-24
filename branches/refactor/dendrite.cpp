/***************************************************************************
                          dendrite.cpp  -  calculates dendritic response
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<cstdlib>
#include<iostream>
using std::endl;
#include<cmath>
#include"dendrite.h"

void Dendrite::init( Configf& configf )
{
  if( !configf.Next( label("Dendrite ",index+1) )) {
    std::cerr<<"Dendrite from population "<<index+1<<" not found."<<endl;
    exit(EXIT_FAILURE);
  }
  string buffer; configf.Param("V",buffer);
  if( buffer == "Steady" ) {
    v.resize(nodes);
    for( int i=0; i<nodes; i++ )
      v[i] = prepropag->phi()[i]*precouple->nu()[i];
  }
  else
    for( int i=0; i<nodes; i++ )
      v[i] = atof(buffer.c_str());
  oldnuphi = v;
  configf.Param("alpha",alpha);
  configf.Param("beta",beta);
}

void Dendrite::restart( Restartf& restartf )
{
}

void Dendrite::dump( Dumpf& dumpf ) const
{
/*  dumpf << "Dendritic Response from population ";
  dumpf << "alpha: " << alpha << " ";
  dumpf << "beta: " << beta << " ";
  dumpf << "nuphi_previous:";
  for(int i=0; i<nodes; i++){
    dumpf << oldnuphi[i] << " ";
  }
  dumpf << endl; // Add endline to dendritic response input*/
}

Dendrite::Dendrite( int nodes, double deltat, int index,
    const Propag* prepropag, const Couple* precouple )
  : NF(nodes,deltat,index), v(nodes), dvdt(nodes,0), oldnuphi(nodes),
    prepropag(prepropag), precouple(precouple)
{
}

Dendrite::~Dendrite()
{
}

void Dendrite::step(void)
{
//
// Steps Vab(t+Timestep) using nuphi(t+Timestep), current nuphi(t) and current Vab(t)
//
//  The program assumes that alpha, beta are constant and nuphi(t) is linear for the time step
//  This is since it is very costly to obtain nuphi(t).
//  Under these assumptions the solution can be explicitly obtained.
//  Calculating the explicit solution is computationally faster than using
//  Runge-Kutta to evaluate the time step.
//  At current time alpha and beta are not functions of space and so 
//  computed variables are used to speed up the calculation.
//

  vector<double> nuphi(nodes);
  for( int i=0; i<nodes; i++ )
    nuphi[i] = precouple->nu()[i]*prepropag->phi()[i];

  double adjustednuphi;
  double deltaPdeltat;
  double C1;

  double expalpha=exp(-alpha*deltat);
  double factoralphabeta=(1.0/alpha)+(1.0/beta);
  if(alpha!=beta){
    double alphaminusbeta;
    double C1expalpha;
    double C2expbeta;
    double expbeta=exp(-beta*deltat);
    alphaminusbeta=alpha-beta;
//#pragma omp parallel for private(adjustedPab,factoralphabeta,deltaPdeltat)
    for(int i=0; i<nodes; i++) {
      deltaPdeltat=(nuphi[i]-oldnuphi[i])/deltat;
      adjustednuphi=oldnuphi[i]-factoralphabeta*deltaPdeltat-v[i];
      C1=(adjustednuphi*beta-dvdt[i]+deltaPdeltat)/alphaminusbeta;
      C1expalpha=C1*expalpha;
      C2expbeta=expbeta*(-C1-adjustednuphi);
      v[i]=C1expalpha+C2expbeta+nuphi[i]-factoralphabeta*deltaPdeltat;
      dvdt[i]=C1expalpha*(-alpha)+C2expbeta*(-beta)+deltaPdeltat;
      oldnuphi[i]=nuphi[i]; //Save current pulse density for next step
    }
  } 
  else {
    double C1deltatplusc2;
    for(int i=0; i<nodes; i++) {
      deltaPdeltat=(nuphi[i]-oldnuphi[i])/deltat;
      adjustednuphi=oldnuphi[i]-factoralphabeta*deltaPdeltat-v[i];
      C1=dvdt[i]-alpha*adjustednuphi-deltaPdeltat;
      C1deltatplusc2=C1*deltat-adjustednuphi;
      v[i]=C1deltatplusc2*expalpha+nuphi[i]-factoralphabeta*deltaPdeltat;
      dvdt[i]=(C1-alpha*C1deltatplusc2)*expalpha+deltaPdeltat;
      oldnuphi[i]=nuphi[i]; //Save current pulse density for next step
    }
  }
}

const vector<double>& Dendrite::V(void) const
{
  return v;
}
