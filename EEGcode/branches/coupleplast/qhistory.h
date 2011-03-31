/***************************************************************************
                          qhistory.h  -  description
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef QHISTORY_H
#define QHISTORY_H

#include<fstream>
#include<iostream>
#include<vector>

#include"istrm.h"
#include"tau.h"
class Poplist; //forward declare poplist. Header file is included in .cpp file

class Qhistory {
public: 
  Qhistory(long nodes,int indexQ); // construct a Qhistory with depth qdepth
  ~Qhistory();
  void grow(int taumax); // Increase qdepth of Qhistory to at least taumax steps
  void init(Istrm& inputf,Poplist& poplist); 
  void dump(std::ofstream& dumpf); // dump history data for restart
  void restart(Istrm& restartf,Poplist& poplist);
  void updateQhistory(Poplist& poplist); // Update Qhistory by reading in Q from populations and moving pointers
  double * getQbytime(Tau& tauobj); // Get a pointer to the Q array with time parameters of tau
  double * getQbytime(int i){return qhistory[i<inew?qhistory.size()+i-inew:i-inew];}; // Get a pointer to the Q array with time index "i"
private:
  Qhistory(Qhistory &); // no copy contructor
  std::vector<double*> qhistory; // Vector of pointers to double arrays of qhistories
  void copyQfrompop(Poplist& poplist); // Copies Q array from the version stored in the population
  int * timeindex; // Array of indexes with current time array positions
  const int indexofQ; // [0:numhistories) this is an identifier matching the qhistory with its corresponding population
  const long nodes;
  int inew; // Index of newest qhistory array
};

#endif
