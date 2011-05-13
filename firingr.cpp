/***************************************************************************
                          firingr.cpp  -  Compute firing response sigmoid
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "firingr.h"
#include<math.h>
using std::endl;

FiringR::FiringR(int popindex,Istrm& inputf):pthetaobj(0),pmthetaobj(0),pm1thetaobj(0),
                 pmsigmaobj(0),sigmaobj(0),pindex(popindex){
  inputf.validate("Firin",103);
  inputf.validate("respo",110); 
  inputf.validate("s",101); // Read succesively upto the end of "Firing response"
  int optionnum;
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
  optionnum=inputf.choose("Sigma:1 ModSigma:2",58);
  if(1==optionnum){
    double initval;
    inputf >> initval;
    sigmaobj = new Parameter("Sigma",initval);
    ismodsigma=false;
  }
  if(2==optionnum){
    pmsigmaobj = new Modsigma(inputf,pindex);
    ismodsigma=true;
  }
  qmaxobj = new Parameter("Qmax",inputf);
  inputf.ignore(200,32); // Ignore appended endline at end of firing response
}

FiringR::~FiringR(){
  if (pmthetaobj) delete pmthetaobj;
  if (pm1thetaobj) delete pm1thetaobj;
  if (pthetaobj) delete pthetaobj;
  if (pmsigmaobj) delete pmsigmaobj;
  if (sigmaobj) delete sigmaobj;
  delete qmaxobj;
}
//
// Method to transform V into Q via sigmoid firing response
//
void FiringR::getQ(double *V,double *Q,long nodes,double timestep){
  double theta;
  double sigma;
  double qmax;
  qmax=qmaxobj->get() ;
  if(ismodsigma) {
    sigma=pmsigmaobj->get(timestep,V);
  }
  else {sigma=sigmaobj->get();}
  if(ismodtheta) {
    if(0==modthetatype){theta=pmthetaobj->get(timestep);}
    else{theta=pm1thetaobj->get(timestep,V,qmax,sigma);}
  }
  else {theta=pthetaobj->get();}
  for(long i=0; i<nodes; i++)
    Q[i] = qmax/(1.0F+exp(-(V[i]-theta)/sigma));
  
}

void FiringR::dump(ofstream& dumpf){
  dumpf << "Firing response "; // Insert Firing Response title
  if(ismodtheta) {
    if(0==modthetatype){pmthetaobj->dump(dumpf);}
    else{pm1thetaobj->dump(dumpf);}
  }
  else {pthetaobj->dump(dumpf);}
  if(ismodsigma) {
    pmsigmaobj->dump(dumpf);
  }
  else {sigmaobj->dump(dumpf);}
  qmaxobj->dump(dumpf);
  dumpf << endl; //Append endl at end of firing response figures
}
