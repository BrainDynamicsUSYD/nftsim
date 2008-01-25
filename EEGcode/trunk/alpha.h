/***************************************************************************
                          alpha.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef ALPHA_H
#define ALPHA_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"

class Alpha {
public: 
  Alpha();
  ~Alpha();
  float get();
  void init(Istrm& inputf);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
private:
  float alpha;
};

#endif
