/***************************************************************************
                          dendriticr.cpp  -  Dendritic Response object
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "dendriticr.h"

DendriticR::DendriticR(long nodes):nodes(nodes){
  previousPab = new double [nodes];
}
DendriticR::~DendriticR(){
  delete[ ] previousPab;
  delete alphaobj;
  delete betaobj;
}

void DendriticR::init(Istrm& inputf, double& Vinit,int propindex, int qindex){
  inputf.validate("V initial",58);
// Determine if an initial value is given or "Steady" initial condition
  std::streampos sp;
  sp = inputf.tellg();
  char ch;
  ch=inputf.get();
  while(' '==ch)ch=inputf.get();
  if('S'==ch){
    inputf.seekg(std::ios::beg);
    double nu=inputf.find("Nu",58,(propindex+1));
    double Q=inputf.find("Initial Q",58,(qindex+1));
    inputf.seekg(sp); // reposition back to original position
    Vinit=nu*Q;
  } else {
    inputf.seekg(sp);
    inputf >> Vinit;
  }
  for(long i=0; i<nodes; i++){
    previousPab[i]=Vinit;
  } 
  alphaobj = new Parameter("alpha",inputf);
  betaobj = new Parameter("beta",inputf);
}

void DendriticR::dump(ofstream& dumpf){
  dumpf << "Dendritic Response from population ";
  alphaobj->dump(dumpf);
  betaobj->dump(dumpf);
  dumpf << "Pab_previous:";
  for(long i=0; i<nodes; i++){
    dumpf << previousPab[i] << " ";
  }
  dumpf << endl; // Add endline to dendritic response input
}

void DendriticR::restart(Istrm& restartf){
  alphaobj = new Parameter("alpha",restartf);
  betaobj = new Parameter("beta",restartf);
  double tempPab;
  restartf.validate("Pab_previous",58);
  for(long i=0; i<nodes; i++){
    restartf >> tempPab;
    previousPab[i]=tempPab;
  }
}

void DendriticR::stepVab(double *Pab, double * Vab, double *dVabdt, double timestep){
//
// Steps Vab(t+Timestep) using Pab(t+Timestep), current Pab(t) and current Vab(t)
//
//  The program assumes that alpha, beta are constant and Pab(t) is linear for the time step
//  This is since it is very costly to obtain Pab(t).
//  Under these assumptions the solution can be explicitly obtained.
//  Calculating the explicit solution is computationally faster than using
//  Runge-Kutta to evaluate the time step.
//  At current time alpha and beta are not functions of space and so 
//  computed variables are used to speed up the calculation.
//
  double adjustedPab;
  double factoralphabeta;
  double deltaPdeltat;
  double C1;

  alpha=alphaobj->get();
  beta=betaobj->get();
  expalpha=exp(-alpha*timestep);
  factoralphabeta=(1.0/alpha)+(1.0/beta);
  if(alpha!=beta){
    double alphaminusbeta;
    double C1expalpha;
    double C2expbeta;
    expbeta=exp(-beta*timestep);
    alphaminusbeta=alpha-beta;
//
#pragma omp parallel for private(adjustedPab,factoralphabeta,deltaPdeltat)
//
    for(long i=0; i<nodes; i++){
      deltaPdeltat=(Pab[i]-previousPab[i])/timestep;
      adjustedPab=previousPab[i]-factoralphabeta*deltaPdeltat-Vab[i];
      C1=(adjustedPab*beta-dVabdt[i]+deltaPdeltat)/alphaminusbeta;
      C1expalpha=C1*expalpha;
      C2expbeta=expbeta*(-C1-adjustedPab);
      Vab[i]=C1expalpha+C2expbeta+Pab[i]-factoralphabeta*deltaPdeltat;
      dVabdt[i]=C1expalpha*(-alpha)+C2expbeta*(-beta)+deltaPdeltat;
      previousPab[i]=Pab[i]; //Save current pulse density for next step
    }
  } 
  else{
    double C1deltatplusc2;
    for(long i=0; i<nodes; i++){
      deltaPdeltat=(Pab[i]-previousPab[i])/timestep;
      adjustedPab=previousPab[i]-factoralphabeta*deltaPdeltat-Vab[i];
      C1=dVabdt[i]-alpha*adjustedPab-deltaPdeltat;
      C1deltatplusc2=C1*timestep-adjustedPab;
      Vab[i]=C1deltatplusc2*expalpha+Pab[i]-factoralphabeta*deltaPdeltat;
      dVabdt[i]=(C1-alpha*C1deltatplusc2)*expalpha+deltaPdeltat;
      previousPab[i]=Pab[i]; //Save current pulse density for next step
    }
  }
}
