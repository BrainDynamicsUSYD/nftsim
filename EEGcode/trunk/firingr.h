/***************************************************************************
                          firingr.h  -  Firing Response sigmoidal
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef FIRINGR_H
#define FIRINGR_H

#include<fstream>
using std::ofstream;
#include<iostream>

using std::endl;

#include"istrm.h"
#include"parameter.h"
#include"modtheta.h"
#include"modtheta1.h"
#include"modsigma.h"

class FiringR {
public: 
  FiringR(int popindex,Istrm& inputf);
  ~FiringR();
  void dump(ofstream& dumpf);
  void getQ(double *V, double *Q, long totalnodes, double timestep);
private: 
  Parameter* pthetaobj;
  Modtheta* pmthetaobj;
  Modtheta1* pm1thetaobj;
  Modsigma* pmsigmaobj;
  bool ismodtheta; // TRUE if theta is modulated
  bool ismodsigma; // TRUE if sigma is modulated
  int modthetatype;
  Parameter* sigmaobj;
  Parameter* qmaxobj;
  int pindex;
};

#endif
