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
#include"istrm.h"
#include"parameter.h"

class FiringR {
public: 
  FiringR(int popindex,Istrm& inputf);
  ~FiringR();
  void dump(ofstream& dumpf);
  void getQ(double *V, double *Q,long nodes,double timestep);
private: 
  FiringR(FiringR& ); // no copy constructor
  std::vector<Parameter> params;
  int type;
  int pindex;
};

#endif
