/***************************************************************************
                          coupling.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef COUPLING_H
#define COUPLING_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"nu.h"

class Coupling {
public: 
  Coupling();
  ~Coupling();
  void init(ifstream& inputf); // initialize the nu variable
  void dump(ofstream& dumpf); // dump nu for restart
  void restart(ifstream& restartf); // restart the nu variable
  void restart();
  void updatePa(float *Pa, float *Etaa, long nodes);
  Coupling * next; // link to next coupling in linked list

private:
  Nu nuobj;
  float nu;
};

#endif
