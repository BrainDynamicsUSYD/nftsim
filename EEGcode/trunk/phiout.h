/***************************************************************************
                          phiout.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PHIOUT_H
#define PHIOUT_H

#include<fstream>
using std::ofstream;
#include<iostream>

using std::endl;
#include<iomanip>
using std::setprecision;

#include"istrm.h"

class Phiout {
public:
  Phiout(Istrm& inputf, ofstream& outputf, int numconct, long nodes);
  ~Phiout();
  void output(ofstream& outputf, double **Eta);
  void dump(ofstream& dumpf);
private:
  long numtraces; // Number of traces to be outputted
  int * idoftrace; // Array containing wave equation id numbers of requested traces
  long * nodeoftrace; // Array containing node numbers of requested traces
  Phiout(const Phiout& other); // Block copy constructor
  Phiout& operator=(const Phiout& other); // Block assignment operator 
};

#endif
