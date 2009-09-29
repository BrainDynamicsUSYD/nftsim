/***************************************************************************
                          modtheta.cpp  - 
                             -------------------
   copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include <math.h>
#include<cstdlib>
#include<string>
using std::string;
#include<sstream>
using std::stringstream;
#include "modtheta.h"

Modtheta::Modtheta(Istrm& inputf, int popindex):currenttime(0){
  y = new double[2];
  dydt = new double[2];
  inputf.validate("Ia",58);
  inputf >> Ia;
  inputf.validate("Ib",58);
  inputf >> Ib;
  inputf.validate("Ic",58);
  inputf >> Ic;
  inputf.validate("Taux",58);
  inputf >> taux;
  inputf.validate("Tauh",58);
  inputf >> tauh;
  inputf.validate("Ax",58);
  inputf >> Ax;
  inputf.validate("Mu",58);
  inputf >> mu;
  inputf.validate("Xtilda",58);
  inputf >> initXtilda;
  inputf.validate("Htilda",58);
  inputf >> initHtilda;
  y[0] = initHtilda;
  y[1] = initXtilda;
  pI = new Timeseries("","",inputf); // I drive no idenfication for back. compatability
// 
  initoutput(inputf,popindex);
}

Modtheta::~Modtheta(){
  delete [ ] y;
  delete [ ] dydt;
  delete pI;
}

void Modtheta::dump(ofstream& dumpf){
  dumpf << "Ia: " << Ia << " ";
  dumpf << "Ib: " << Ib << " ";
  dumpf << "Ic: " << Ic << " ";
  dumpf << "Taux: " << taux << " ";
  dumpf << "Tauh: " << tauh << " ";
  dumpf << "Ax:" << Ax << " ";
  dumpf << "Mu:" << mu << " ";
  dumpf << "Xtilda:" << y[1] << " ";
  dumpf << "Htilda:" << y[0] << " ";
  pI->dump(dumpf);
}

double Modtheta::get(double timestep){
  rk4(currenttime,y,timestep);
  currenttime+=timestep;
  double itheta=Ic-3*Ib*y[1]+(Ib-Ia)*y[0];
  output(y,itheta);
  return (itheta/mu);
}

void Modtheta::rk4(double t,double* y, float timestep){
//
// This is a simple non-optimized implementation of Runge-Kutta algorithm
//
  double yt[2];
  double dyt[2];
  double dym[2];
  double h=timestep; // temp variable to speed up routine
  double h2=timestep/2; // temp variable to speed up routine
  double h6=timestep/6; // temp variable to speed up routine
  double Ival[1];
//
  pI->get(t,Ival,1);
  rkderivs(t,y,dydt,Ival[0]);
  for(int i=0; i<2; i++) yt[i]=y[i]+h2*dydt[i];
  pI->get(t+h2,Ival,1);
  rkderivs(t+h2,yt,dyt,Ival[0]);
  for(int i=0; i<2; i++) yt[i]=y[i]+h2*dyt[i];
  rkderivs(t+h2,yt,dym,Ival[0]);
  for(int i=0; i<2; i++) {yt[i]=y[i]+h*dym[i]; dym[i]+=dyt[i];}
  pI->get(t+h,Ival,1);
  rkderivs(t+h,yt,dyt,Ival[0]);
  for(int i=0; i<2; i++) y[i]=y[i]+h6*(dydt[i]+dyt[i]+2*dym[i]);
}

void Modtheta::rkderivs(double steptime, double* y, double* dydt, double I){
//
// Function returns the derivatives for modulated threshold equations
// y[0] is H^tilda , y[1] is X^tilda
//
  double Itildaeff=I+(Ia-Ib)*y[0]+3*Ib*y[1];
  double xinfinity;
  if(Itildaeff>Ic) {xinfinity = Ax*sqrt(Itildaeff/Ic-1.0);}
  else {xinfinity=0;}
  dydt[0] = (3*y[1]-y[0])/tauh; // Differential equation defining H^tilda
  dydt[1] = (xinfinity-y[1])/taux; // Differential equaiton defining X^tilda
}

void Modtheta::initoutput(Istrm& inputf, int popindex){
  popindex++; // (popindex+1) is to ensure numbering from one not zero in output
  stringstream ss(stringstream::in | stringstream::out);
  ss << "eegcode.threshout." << popindex;
  threshoutf.open(ss.str().c_str(),ios::out);
  if( !threshoutf ){
    std::cerr << "Unable to open 'eegcode.threshout." << popindex << "' for output \n";
    exit(EXIT_FAILURE);
  }
  threshoutf << "Robinson/Wu/Kim bursting model output data" << endl;
  threshoutf << "Data structure - I_{theta}, H^{tilda}, X^{tilda}" << endl;
}

void Modtheta::output(double* y, double itheta){
  threshoutf << itheta << endl;
  threshoutf << y[0] << endl;
  threshoutf << y[1] << endl;
  threshoutf << endl;
}
