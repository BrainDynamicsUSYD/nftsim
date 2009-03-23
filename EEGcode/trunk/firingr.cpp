/***************************************************************************
                          firingr.cpp  -  description
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "firingr.h"
#include<math.h>

FiringR::FiringR(int popindex):pindex(popindex){
}

FiringR::~FiringR(){
  if (pmthetaobj) delete pmthetaobj;
  if (pm1thetaobj) delete pm1thetaobj;
  if (pthetaobj) delete pthetaobj;
  delete sigmaobj;
  delete qmaxobj;
}

void FiringR::init(Istrm& inputf){
  inputf.validate("Firin",103);
  inputf.validate("respo",110); 
  inputf.validate("s",101); // Read succesively upto the end of "Firing response"
  int optionnum;
  pthetaobj=0;
  pmthetaobj=0;
  pm1thetaobj=0;
  optionnum=inputf.choose("Theta:1 ModTheta:2 ModTheta1:3",58);
  if(1==optionnum){
    double initval;
    inputf >> initval;
    pthetaobj = new Parameter("Theta",initval);
    ismodtheta=false;
  }
  if(2==optionnum){
    pmthetaobj = new Modtheta(inputf,pindex);
    ismodtheta=true;
    modthetatype=0;
  }
  if(3==optionnum){
    pm1thetaobj = new Modtheta1(inputf,pindex);
    ismodtheta=true;
    modthetatype=1;
  }
  sigmaobj = new Parameter("Sigma",inputf);
  qmaxobj = new Parameter("Qmax",inputf);
}

//
// Method to transform V into Q via sigmoid firing response
//
void FiringR::getQ(double *V, double *Q, long totalnodes, double timestep){
  sigma=sigmaobj->get();
  qmax=qmaxobj->get() ;
  if(ismodtheta) {
    if(0==modthetatype){theta=pmthetaobj->get(timestep);}
    else{theta=pm1thetaobj->get(timestep,V,qmax,sigma);}
  }
  else {theta=pthetaobj->get();}
  for(long i=0; i<totalnodes; i++)
    Q[i] = qmax/(1.0F+exp(-(V[i]-theta)/sigma));
  
}

void FiringR::dump(ofstream& dumpf){
  dumpf << "Firing response "; // Insert Firing Response title
  if(ismodtheta) {
    if(0==modthetatype){pmthetaobj->dump(dumpf);}
    else{pm1thetaobj->dump(dumpf);}
  }
  else {pthetaobj->dump(dumpf);}
  sigmaobj->dump(dumpf);
  qmaxobj->dump(dumpf);
  dumpf << endl; //Append endl at end of firing response figures
}

void FiringR::restart(Istrm& restartf){
  restartf.validate("Firin",103);
  restartf.validate("respo",110); 
  restartf.validate("s",101); // Read succesively upto the end of "Firing response"
  int optionnum;
  optionnum=restartf.choose("Theta:1 ModTheta:2 Modtheta1:3",58);
  if(1==optionnum){
    double initval;
    restartf >> initval;
    pthetaobj = new Parameter("Theta",initval);
    ismodtheta=false;
  }
  if(2==optionnum){
    pmthetaobj = new Modtheta(restartf,pindex);
    ismodtheta=true;
    modthetatype=0;
  }
  if(3==optionnum){
    pm1thetaobj = new Modtheta1(restartf,pindex);
    ismodtheta=true;
    modthetatype=1;
    
  }
  sigmaobj = new Parameter("Sigma",restartf);
  qmaxobj = new Parameter("Qmax",restartf);
  restartf.ignore(200,32); // Ignore appended endline at end of firing response
}
