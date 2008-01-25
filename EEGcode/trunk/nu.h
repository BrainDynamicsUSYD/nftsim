/***************************************************************************
                          nu.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef NU_H
#define NU_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

class Nu {
public:
  Nu();
  ~Nu();
  float get();
  void init(ifstream& inputf);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf);
private:
  float nu;
};

#endif
