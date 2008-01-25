/***************************************************************************
                          qhistory.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef QHISTORY_H
#define QHISTORY_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

class Poplist; //forward declare poplist. Header file is included in .cpp file

class Qhistory {
public: 
  Qhistory(int qdepth, long gridsize, int indexQ); // construct a Qhistory with depth qdepth
  ~Qhistory();
  void init(ifstream& inputf, Poplist *ppoplist); 
  void dump(ofstream& dumpf); // dump history data for restart
  void restart(ifstream& restartf, Poplist *ppoplist);
  void updateQhistory(Poplist *ppoplist); // Update Qhistory by reading in Q from populations and moving pointers
  float * getQbytime(int index){return qhistory[timeindex[index]];}; // Get a pointer to the Q array with time index "index"
                             // In this case indexx="0" Q for current time, index="1" Q one time step in the past etc.
private:
  float **qhistory; // Array of pointers to float arrays of qhistories
  void copyQfrompop(Poplist *ppoplist); // Copies Q array from the version stored in the population
  int * timeindex; // Array of indexes with current time array positions
  const int indexofQ; // [0:numhistories) this is an identifier matching the qhistory with its corresponding population
  const int depthq;
  const long gridsize;
  long rowlength;
  long sidelength; // number of nodes along one side length of square node population
  long startfirstrow;
  long startlastrow;
};

#endif
