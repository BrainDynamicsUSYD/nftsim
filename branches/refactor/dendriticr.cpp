/***************************************************************************
                          dendriticr.cpp  -  Dendritic Response object
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<string>
using std::string;
#include<sstream>
using std::stringstream;
#include<cstdlib>
#include"dendriticr.h"
using std::endl;

DendriticR::DendriticR(long nodes):nodes(nodes)
{
  previousPab = new double [nodes];
}
DendriticR::~DendriticR(){
  delete[ ] previousPab;
}

void DendriticR::init(Configf& inputf, double& Vinit, int propindex, int qindex)
{
  // Determine if an initial value is given or "Steady==0" initial condition
  string buffer; inputf.Param("V",buffer);
  if( buffer == "Steady" ) {
    /*stringstream ss; ss<<"Firing "<<qindex+1<<"*Q:";
    buffer = inputf.Find(ss.str());
    double q = atof(buffer.c_str());
    ss.str(""); ss<<"Couple "<<propindex+1<<"*Nu:";
    buffer = inputf.Find(ss.str());
    double nu = atof(buffer.c_str());
    Vinit = nu*q;*/
  }
  else
    Vinit = atof(buffer.c_str());
  /*if( !Vinit ) {
    stringstream ss; ss<<"Couple "<<propindex+1<<"*Nu:";
    string sbuffer = inputf.Find( ss.str().c_str() );
    double nu = atof(sbuffer.c_str());
    ss.str(""); ss<<"Firing "<<qindex+1<<"*Q:";
    sbuffer = inputf.Find( ss.str().c_str() );
    double Q = atof(sbuffer.c_str());
    Vinit = nu*Q;
  }*/

  for(long i=0; i<nodes; i++)
    previousPab[i]=Vinit;
  inputf.Param("alpha",alpha);
  inputf.Param("beta",beta);

  /*std::streampos sp;
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
  }*/
}

void DendriticR::dump(ofstream& dumpf)
{
  dumpf << "Dendritic Response from population ";
  dumpf << "alpha: " << alpha << " ";
  dumpf << "beta: " << beta << " ";
  dumpf << "Pab_previous:";
  for(long i=0; i<nodes; i++){
    dumpf << previousPab[i] << " ";
  }
  dumpf << endl; // Add endline to dendritic response input
}

void DendriticR::restart(Configf& restartf)
{
  restartf.Param("alpha",alpha);
  restartf.Param("beta",beta);
  /*double tempPab;
  restartf.validate("Pab_previous",':');
  for(long i=0; i<nodes; i++){
    restartf >> tempPab;
    previousPab[i]=tempPab;
  }*/
}

void DendriticR::stepVab(double *Pab, double * Vab, double *dVabdt, double timestep)
{
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
  double deltaPdeltat;
  double C1;

  double expalpha=exp(-alpha*timestep);
  double factoralphabeta=(1.0/alpha)+(1.0/beta);
  if(alpha!=beta){
    double alphaminusbeta;
    double C1expalpha;
    double C2expbeta;
    double expbeta=exp(-beta*timestep);
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
