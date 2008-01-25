/***************************************************************************
                          phiout.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PHIOUT_H
#define PHIOUT_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;
#include<iomanip>
using std::setprecision;

class Phiout {
public:
  Phiout(ifstream& inputf, ofstream& outputf);
  ~Phiout();
  void output(ofstream& outputf, float **Eta);
private:
  long numtraces; // Number of traces to be outputted
  int * idoftrace; // Array containing wave equation id numbers of requested traces
  long * nodeoftrace; // Array containing node numbers of requested traces
};

#endif
