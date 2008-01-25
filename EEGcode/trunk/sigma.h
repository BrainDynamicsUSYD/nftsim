/***************************************************************************
                          sigma.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef SIGMA_H
#define SIGMA_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

class Sigma {
public: 
  Sigma();
  ~Sigma();
  float get();
  void init(ifstream& inputf);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf);
private:
  float sigma;
};

#endif
