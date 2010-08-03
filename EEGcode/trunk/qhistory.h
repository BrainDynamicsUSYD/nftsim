/***************************************************************************
                          qhistory.h  -  description
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef QHISTORY_H
#define QHISTORY_H

#include<fstream>
using std::ofstream;
#include<iostream>

using std::endl;

#include"istrm.h"
#include"tau.h"
class Poplist; //forward declare poplist. Header file is included in .cpp file

class Qhistory {
public: 
  Qhistory(int qdepth,long nodes,int indexQ); // construct a Qhistory with depth qdepth
  ~Qhistory();
  void init(Istrm& inputf,Poplist& poplist); 
  void dump(ofstream& dumpf); // dump history data for restart
  void restart(Istrm& restartf,Poplist& poplist);
  void updateQhistory(Poplist& poplist); // Update Qhistory by reading in Q from populations and moving pointers
  double * getQbytime(Tau& tauobj); // Get a pointer to the Q array with time parameters of tau
  double * getQbytime(int i){return qhistory[i<inew?depth+i-inew:i-inew];}; // Get a pointer to the Q array with time index "i"
private:
  double **qhistory; // Array of pointers to double arrays of qhistories
  void copyQfrompop(Poplist& poplist); // Copies Q array from the version stored in the population
  int * timeindex; // Array of indexes with current time array positions
  const int indexofQ; // [0:numhistories) this is an identifier matching the qhistory with its corresponding population
  const int depth;
  const long nodes;
  int inew; // Index of newest qhistory array
};

#endif
