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
#include"theta.h"
#include"sigma.h"
#include"qmax.h"

class FiringR {
public: 
  FiringR();
  ~FiringR();
  void init(Istrm& inputf);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
  void getQ(float *V, float *Q, long totalnodes);
private: 
  Theta thetaobj;
  Sigma sigmaobj;
  Qmax qmaxobj;
  float theta;
  float sigma;
  float qmax;
};

#endif
