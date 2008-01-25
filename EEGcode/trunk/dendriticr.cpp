/***************************************************************************
                          dendriticr.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "dendriticr.h"

DendriticR::DendriticR(){
}
DendriticR::~DendriticR(){
}

void DendriticR::init(ifstream& inputf){
  alphaobj.init(inputf);
  betaobj.init(inputf);
}

void DendriticR::dump(ofstream& dumpf){
  dumpf << "Dendritic Response from population ";
  alphaobj.dump(dumpf);
  betaobj.dump(dumpf);
}

void DendriticR::restart(ifstream& restartf){
  alphaobj.restart(restartf);
  betaobj.restart(restartf);
}

void DendriticR::stepVab(float *Pab, float * Vab, float *dVabdt, long nodes, float timestep){
//
// Steps Pab(t+Timestep) using current Pab(t) and current Vab(t)
//
//  The program use a pseudo-Runge Kutta algorithm
//  The difference is Pab is not evaluated at Pab(t+Timpestep/4),
//  Pab(t+Timpestep/2) and Pab(t+3*Timpestep/4) instead these are all
//  approximated by Pab(t).
//
  double y[2];

  alpha=alphaobj.get();
  beta=betaobj.get();
  for(long i=0; i<nodes; i++){
    y[0]=Vab[i];
    y[1]=dVabdt[i];
    rk4(y, timestep, Pab[i]);
    Vab[i]=y[0];
    dVabdt[i]=y[1];
    }
}

void DendriticR::rk4(double* y, float timestep, float nodePab){
//
//  This is a non-optimized simple implementation of Runge-Kutta algorithm
//  It should probably have h,h2,h6 defined as constructed variables.
//  (alpha*beta) and (alpha+beta) can be sped up also.
//
    double yt[2];
    double dydt[2];
    double dyt[2];
    double dym[2];
    double h=timestep;       /* Temp variable to speed up rk routine*/
    double h2=timestep/2;    /* Temp variable to speed up rk routine*/
    double h6=timestep/6;    /* Temp variable to speed up rk routine*/

    rkderivs(y,nodePab,dydt);
    for(int i=0; i<2; i++) yt[i]=y[i]+h2*dydt[i];
    rkderivs(yt,nodePab,dyt);
    for(int i=0; i<2; i++) yt[i]=y[i]+h2*dyt[i];
    rkderivs(yt,nodePab,dym);
    for(int i=0; i<2; i++) {yt[i]=y[i]+h*dym[i];  dym[i]+=dyt[i];}
    rkderivs(yt,nodePab,dyt);
    for(int i=0; i<2; i++) y[i]=y[i]+h6*(dydt[i]+dyt[i]+2*dym[i]);
}

void DendriticR::rkderivs(double* y, float nodePab, double* dydt){
//
// Function returns the derivatives for the dendritic response ODE
// given by     1/(alpha*beta) Vab'' + (1/alpha + 1/beta) Vab' + Vab = Pab
// The dendritic equation has been converted to two first order ODEs
//              Vab' = u
//              u'   = (alpha*beta)*Pab - (alpha+beta)u - (alpha*beta)Vab
//
  dydt[0] = y[1]; // equation for V'
  dydt[1] = (alpha*beta)*nodePab - (alpha+beta)*y[1] - (alpha*beta)*y[0]; //equation for u'
}
