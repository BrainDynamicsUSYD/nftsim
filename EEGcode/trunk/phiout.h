/***************************************************************************
                          phiout.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PHIOUT_H
#define PHIOUT_H

#include<fstream>
#include<iostream>
#include"istrm.h"

class Phiout {
public:
  Phiout(Istrm& inputf, std::ofstream& outputf, int numconct, long nodes);
  ~Phiout();
  void output(std::ofstream& outputf, double **Eta);
  void dump(std::ofstream& dumpf);
private:
  Phiout(Phiout& ); // no copy constructor
  long numtraces; // Number of traces to be outputted
  int * idoftrace; // Array containing wave equation id numbers of requested traces
  long * nodeoftrace; // Array containing node numbers of requested traces
  Phiout(const Phiout& other); // Block copy constructor
  Phiout& operator=(const Phiout& other); // Block assignment operator 
};

#endif
