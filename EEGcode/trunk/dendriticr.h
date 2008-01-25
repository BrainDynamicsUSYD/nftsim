/***************************************************************************
                          dendriticr.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef DENDRITICR_H
#define DENDRITICR_H

#include<math.h>
#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"alpha.h"
#include"beta.h"

class DendriticR {
public: 
  DendriticR();
  ~DendriticR();
  void init(ifstream& inputf, float& Vinit);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf);
  void stepVab(float *Pab, float * Vab, float *dVabdt, long nodes, float timestep);
private: 
  float alpha;  // Holding variable of alpha at this moment
  float beta;  // Holding variable of alpha at this moment
  float expalpha; // Factor in exactstep
  float expbeta; // Factor in exactstep
  Alpha alphaobj; // Object which initializes and get alpha values
  Beta betaobj; // Object which initializes and gets beta values
};

#endif
