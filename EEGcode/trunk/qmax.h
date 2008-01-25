/***************************************************************************
                          Qmax.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef QMAX_H
#define QMAX_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

class Qmax {
public: 
  Qmax();
  ~Qmax();
  void init(ifstream& inputf);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf);
  float get();
private: 
  float qmax;
};

#endif
