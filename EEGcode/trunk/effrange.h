/***************************************************************************
                          effrange.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef EFFRANGE_H
#define EFFRANGE_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

class EffRange {
public: 
  EffRange();
  ~EffRange();
  float get();
  void init(ifstream& inputf);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf);
private:
  float effrange;
};

#endif
