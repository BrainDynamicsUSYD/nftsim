/***************************************************************************
                          qhistory.h  -  keyring storing q into past
                             to account for propagation delays along axons
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef QHISTORY_H
#define QHISTORY_H

#include<fstream>
#include<iostream>
#include<vector>
using std::vector;

#include"istrm.h"
#include"tau.h"

class Poplist; //forward declare poplist. Header file is included in .cpp file

class Qhistory {
public: 
  Qhistory(long nodes,int indexQ); // construct a Qhistory with depth qdepth
  ~Qhistory();
  void grow(int taumax); // Increase qdepth of Qhistory to at least taumax steps
  void init(Istrm& inputf,Poplist& poplist); 
  void dump(std::ofstream& dumpf);
  void restart(Istrm& restartf,Poplist& poplist);

  void updateQhistory(Poplist& poplist); // Update Qhistory by reading in Q from populations and moving pointers
  double* getQbytime(Tau& tauobj); // Get pointer to Q array with time tau
  double* getQbytime(int i) {
    // Get a pointer to the Q array with time index "i"
    return qhistory[i<inew?qhistory.size()+i-inew:i-inew];
  }

private:
  Qhistory(Qhistory &); // no copy contructor
  vector<double*> qhistory; // Keyring of Q of all nodes
  int * timeindex; // Array of indexes with current time array positions
  const int indexofQ; // Identifier matching corresponding population
  const long nodes;
  int inew; // index of newest qhistory array
};

#endif
