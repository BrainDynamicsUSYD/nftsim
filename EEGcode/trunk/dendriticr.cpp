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

void DendriticR::init(Istrm& inputf, float& Vinit){
  inputf.validate("V initial",58);
  inputf >> Vinit;
  alphaobj.init(inputf);
  betaobj.init(inputf);
}

void DendriticR::dump(ofstream& dumpf){
  dumpf << "Dendritic Response from population ";
  alphaobj.dump(dumpf);
  betaobj.dump(dumpf);
}

void DendriticR::restart(Istrm& restartf){
  alphaobj.restart(restartf);
  betaobj.restart(restartf);
}

void DendriticR::stepVab(float *Pab, float * Vab, float *dVabdt, long nodes, float timestep){
//
// Steps Pab(t+Timestep) using current Pab(t) and current Vab(t)
//
//  The program assumes that alpha, beta and Pab(t) are constant for the time step
//  This is since it is very costly to obtain Pab(t).
//  Under these assumptions the solution can be explicitly obtained.
//  Calculating the explicit solution is computationally faster than using
//  Runge-Kutta to evaluate the time step.
//  At current time alpha and beta are not functions of space and so 
//  computed variables are used to speed up the calculation.
//
  double Pabminusy;
  double C1;
  double C2;

  alpha=alphaobj.get();
  beta=betaobj.get();
  expalpha=exp(-alpha*timestep);
  expbeta=exp(-beta*timestep);
  for(long i=0; i<nodes; i++){
    Pabminusy=Pab[i]-Vab[i];
    C1=(Pabminusy*beta-dVabdt[i])/(alpha-beta);
    C2=-C1-Pabminusy;
    Vab[i]=C1*expalpha+C2*expbeta+Pab[i];
    dVabdt[i]=C1*(-alpha)*expalpha+C2*(-beta)*expbeta;
  }
}
