/***************************************************************************
                          pharmonic.cpp  -  propagator with harmonic oscillators
			                    and no spatial coupling
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "pharmonic.h"
#include<math.h>

Pharmonic::Pharmonic(long totalnodes, double dt):nodes(totalnodes),
            timestep(dt),gammaobj("gamma"){
  previousQ = new double [nodes];
  previousPhi = new double [nodes];
  dPhidt = new double [nodes];
}

Pharmonic::~Pharmonic(){
  delete[ ] previousQ;
  delete[ ] previousPhi;
  delete[ ] dPhidt;
}

void Pharmonic::init(Istrm& inputf, Qhistory* qhistory){
  double phiinit;

  inputf.validate("Initial Phi",58);
  inputf >> phiinit;
  for(long i=0; i<nodes; i++){
    previousQ[i]=phiinit;
    previousPhi[i]=phiinit;
    dPhidt[i]=0.;
  }
  int optionnum;
  optionnum=inputf.choose("Tauab:1 Tauabt:2",58);
  float tauabfloat;
  if(1==optionnum){
    inputf >> tauabfloat;
  }
  if(2==optionnum){
    double tauabt;
    inputf >> tauabt;
    tauabfloat=tauabt/timestep;
  }
  if( !((1==optionnum)||(2==optionnum)) ){
    cerr << "Last read looking for Tauab or Taubt found neither" << endl;
    exit(EXIT_FAILURE);
  }
  tauab=int(tauabfloat);
  if(tauabfloat<1 && tauabfloat>0){
    cerr << "Last read Tauab: " << tauabfloat << endl;
    cerr << "Tauab must be greater than 1 as it is measured in" << endl;
    cerr << "time steps not a time measured in seconds" << endl;
    exit(EXIT_FAILURE);
  }
  gammaobj.init(inputf);
}

void Pharmonic::dump(ofstream& dumpf){
  dumpf << "- Tauab: " << tauab << " ";
  gammaobj.dump(dumpf);
  dumpf << endl;
  dumpf << "Q_previous:";
  for(long i=0; i<nodes; i++){
    dumpf << previousQ[i] << " ";
  }
  dumpf << endl;
  dumpf << "Phi_previous:";
  for(long i=0; i<nodes; i++){
    dumpf << previousPhi[i] << " ";
  }
  dumpf << endl;
  dumpf << "dPhidt:";
  for(long i=0; i<nodes; i++){
    dumpf << dPhidt[i] << " ";
  }
  dumpf << endl; // Add endline to propagator input
}

void Pharmonic::restart(Istrm& restartf){
  restartf.ignore(200,45);
  int optionnum;
  optionnum=restartf.choose("Tauab:1 Tauabt:2",58);
  float tauabfloat;
  if(1==optionnum){
    restartf >> tauabfloat;
  }
  if(2==optionnum){
    double tauabt;
    restartf >> tauabt;
    tauabfloat=tauabt/timestep;
  }
  if( !((1==optionnum)||(2==optionnum)) ){
    cerr << "Last read looking for Tauab or Taubt found neither" << endl;
    exit(EXIT_FAILURE);
  }
  tauab=int(tauabfloat);
  if(tauabfloat<1 && tauabfloat>0){
    cerr << "Last read Tauab: " << tauabfloat << endl;
    cerr << "Tauab must be greater than 1 as it is measured in" << endl;
    cerr << "time steps not a time measured in seconds" << endl;
    exit(EXIT_FAILURE);
  }
  gammaobj.init(restartf);
  double temp;
  restartf.validate("Q_previous",58);
  for(long i=0; i<nodes; i++){
    restartf >> temp;
    previousQ[i]=temp;
  }
  restartf.validate("Phi_previous",58);
  for(long i=0; i<nodes; i++){
    restartf >> temp;
    previousPhi[i]=temp;
  }
  restartf.validate("dPhidt",58);
  for(long i=0; i<nodes; i++){
    restartf >> temp;
    dPhidt[i]=temp;
  }
}

void Pharmonic::stepwaveeq(double * Phi, Qhistory* pqhistory){
//
//  The program currently assumes that Gamma is constant and Q(t) is linear for the time step
//  This is since it is very costly to obtain Q(t).
//  Under these assumptions the solution can be explicitly obtained.
//  Calculating the explicit solution is computationally faster than using
//  Runge-Kutta to evaluate the time step.
//  At current time alpha and beta are not functions of space and so 
//  computed variables are used to speed up the calculation.
//
  double adjustedQ;
  double factorgamma;
  double deltaQdeltat;
  double C1;
  double C1deltatplusc2;

  double* Q=pqhistory->getQbytime(tauab);
  double gamma=gammaobj.get();
  double expgamma=exp(-gamma*timestep);
  factorgamma=(2.0/gamma);
  for(long i=0; i<nodes; i++){
    deltaQdeltat=(Q[i]-previousQ[i])/timestep;
    adjustedQ=previousQ[i]-factorgamma*deltaQdeltat-previousPhi[i];
    C1=dPhidt[i]-gamma*adjustedQ-deltaQdeltat;
    C1deltatplusc2=C1*timestep-adjustedQ;
    Phi[i]=C1deltatplusc2*expgamma+Q[i]-factorgamma*deltaQdeltat;
    dPhidt[i]=(C1-gamma*C1deltatplusc2)*expgamma+deltaQdeltat;
    previousQ[i]=Q[i]; //Save current Q for next step
    previousPhi[i]=Phi[i]; //Save current Q for next step
  }
}
