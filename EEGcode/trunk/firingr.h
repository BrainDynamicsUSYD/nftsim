/***************************************************************************
                          firingr.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef FIRINGR_H
#define FIRINGR_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"parameter.h"
#include"modtheta.h"

class FiringR {
public: 
  FiringR(int popindex);
  ~FiringR();
  void init(Istrm& inputf);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
  void getQ(double *V, double *Q, long totalnodes, double timestep);
private: 
  Parameter* pthetaobj;
  Modtheta* pmthetaobj;
  bool ismodtheta; // TRUE if theta is modulated
  Parameter sigmaobj;
  Parameter qmaxobj;
  double theta;
  double sigma;
  double qmax;
  int pindex;
};

#endif
