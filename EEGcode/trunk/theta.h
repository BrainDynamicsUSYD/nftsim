/***************************************************************************
                          theta.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef THETA_H
#define THETA_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

class Theta {
public: 
  Theta();
  ~Theta();
  void init(ifstream& inputf);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf);
  float get();
private: 
  float theta;
};

#endif
