/***************************************************************************
                          sigma.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef SIGMA_H
#define SIGMA_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
class Sigma {
public: 
  Sigma();
  ~Sigma();
  float get();
  void init(Istrm& inputf);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
private:
  float sigma;
};

#endif
