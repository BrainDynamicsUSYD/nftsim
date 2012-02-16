/***************************************************************************
                          tau.h  -  time delay object
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef TAU_H
#define TAU_H

#include<vector>
using std::vector;
#include<fstream>
#include<iostream>
#include"configf.h"
class Qhistory; //forward declare Qhistory

class Tau {
public: 
  Tau(long nodes,double deltat,Configf& inputf,Qhistory& qhistory);
  ~Tau();
  void dump(std::ofstream& dumpf);
  int tauab; // stores a single value tau
  double* qarray; // array to pass to qhistory when isarraytau true to
                  // hold q return values. Not needed for single value tau
  vector<double> tauarr; //stores an array of tau values
private:
  Tau(Tau& ); // no copy constructor
  const long nodes;
  const double deltat; // Grid spacing in time
};

#endif
