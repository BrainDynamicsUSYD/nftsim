/***************************************************************************
                          firingr.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "firingr.h"
#include<math.h>

FiringR::FiringR(){
}
FiringR::~FiringR(){
}

void FiringR::init(ifstream& inputf){
  thetaobj.init(inputf);
  sigmaobj.init(inputf);
  qmaxobj.init(inputf);
}

//
// Method to transform V into Q via sigmoid firing response
//
void FiringR::getQ(float *V, float *Q, long totalnodes){
  theta=thetaobj.get();
  sigma=sigmaobj.get();
  qmax=qmaxobj.get() ;
  for(long i=0; i<totalnodes; i++)
    Q[i] = qmax/(1.0F+exp(-(V[i]-theta)/sigma));
}

void FiringR::dump(ofstream& dumpf){
  dumpf << "Firing Response "; // Insert Firing Response title
  thetaobj.dump(dumpf);
  sigmaobj.dump(dumpf);
  qmaxobj.dump(dumpf);
  dumpf << endl; //Append endl at end of firing response figures
}

void FiringR::restart(ifstream& restartf){
  thetaobj.restart(restartf);
  sigmaobj.restart(restartf);
  qmaxobj.restart(restartf);
  restartf.ignore(200,32); // Ignore appended endline at end of firing response
}
