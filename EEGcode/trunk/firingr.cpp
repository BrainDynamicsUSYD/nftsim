/***************************************************************************
                          firingr.cpp  -  description
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "firingr.h"
#include<math.h>

FiringR::FiringR(int popindex):sigmaobj("Sigma"),qmaxobj("Qmax"),pindex(popindex){
}

FiringR::~FiringR(){
  if (pmthetaobj) delete pmthetaobj;
  if (pthetaobj) delete pthetaobj;
}

void FiringR::init(Istrm& inputf){
  inputf.validate("Firin",103);
  inputf.validate("respo",110); 
  inputf.validate("s",101); // Read succesively upto the end of "Firing response"
  int optionnum;
  optionnum=inputf.choose("Theta:1 ModTheta:2",58);
  if(1==optionnum){
    pthetaobj = new Parameter("Theta");
    double initval;
    inputf  >> initval;
    pthetaobj->init(initval);
    ismodtheta=false;
  }
  if(2==optionnum){
    pmthetaobj = new Modtheta();
    pmthetaobj->init(inputf,pindex);
    ismodtheta=true;
  }
  sigmaobj.init(inputf);
  qmaxobj.init(inputf);
}

//
// Method to transform V into Q via sigmoid firing response
//
void FiringR::getQ(double *V, double *Q, long totalnodes, double timestep){
  if(ismodtheta) {theta=pmthetaobj->get(timestep);}
  else {theta=pthetaobj->get();}
  sigma=sigmaobj.get();
  qmax=qmaxobj.get() ;
  for(long i=0; i<totalnodes; i++)
    Q[i] = qmax/(1.0F+exp(-(V[i]-theta)/sigma));
  
}

void FiringR::dump(ofstream& dumpf){
  dumpf << "Firing response "; // Insert Firing Response title
  if(ismodtheta) {pmthetaobj->dump(dumpf);}
  else {pthetaobj->dump(dumpf);}
  sigmaobj.dump(dumpf);
  qmaxobj.dump(dumpf);
  dumpf << endl; //Append endl at end of firing response figures
}

void FiringR::restart(Istrm& restartf){
  restartf.validate("Firin",103);
  restartf.validate("respo",110); 
  restartf.validate("s",101); // Read succesively upto the end of "Firing response"
  int optionnum;
  optionnum=restartf.choose("Theta:1 ModTheta:2",58);
  if(1==optionnum){
    pthetaobj = new Parameter("Theta");
    double initval;
    restartf  >> initval;
    pthetaobj->restart(initval);
    ismodtheta=false;
  }
  if(2==optionnum){
    pmthetaobj = new Modtheta();
    pmthetaobj->restart(restartf,pindex);
    ismodtheta=true;
  }
  sigmaobj.restart(restartf);
  qmaxobj.restart(restartf);
  restartf.ignore(200,32); // Ignore appended endline at end of firing response
}
