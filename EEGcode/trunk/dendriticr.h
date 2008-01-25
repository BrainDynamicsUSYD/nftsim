/***************************************************************************
                          dendriticr.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef DENDRITICR_H
#define DENDRITICR_H

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
  void init(ifstream& inputf);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf);
  void stepVab(float *Pab, float * Vab, float *dVabdt, long nodes, float timestep);
  DendriticR* next; // next dendritic response in linked list of dendritic responses
private: 
  void rk4(double* y, float timestep, float nodePab); // Helper routine for stepVab
  void rkderivs(double* y, float nodePab, double* dydt); // Helper routine for stepVab
  float alpha;  // Holding variable of alpha at this moment
  float beta;  // Holding variable of alpha at this moment
  Alpha alphaobj; // Object which initializes and get alpha values
  Beta betaobj; // Object which initializes and gets beta values
};

#endif
