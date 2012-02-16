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
//using std::vector;
#include<vector>
#include"configf.h"

class FiringR {
public: 
  FiringR(int popindex,Configf& inputf);
  ~FiringR();
  void dump(ofstream& dumpf);
  void getQ(double *V, double *Q,long nodes,double timestep);
private: 
  FiringR(FiringR& ); // no copy constructor
  double theta;
  double sigma;
  double Q_max;
  double gradient;
  double intercept;
};

#endif
