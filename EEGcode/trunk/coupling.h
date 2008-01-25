/***************************************************************************
                          coupling.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef COUPLING_H
#define COUPLING_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"nu.h"

class Coupling {
public: 
  Coupling();
  ~Coupling();
  void init(Istrm& inputf); // initialize the nu variable
  void dump(ofstream& dumpf); // dump nu for restart
  void restart(Istrm& restartf); // restart the nu variable
  void restart();
  void updatePa(float *Pa, float *Etaa, long nodes);

private:
  Nu nuobj;
  float nu;
};

#endif
