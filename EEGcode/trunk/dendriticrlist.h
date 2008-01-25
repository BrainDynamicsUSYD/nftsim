/***************************************************************************
                          dendriticrlist.h  -  Header for DenderiticRList
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef DENDRITICRLIST_H
#define DENDRITICRLIST_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"dendriticr.h"
class ConnectMat; // Forward reference here as only use pointer. Header file included in .cpp file
class PropagNet; // Use a forward reference here, the class deference the pointer so propagnet.h is
                 // included in the .cpp file this ensures we don't have circular references

class DendriticRlist {
public: 
  DendriticRlist(long nodes, int popindex, ConnectMat * pconnectmat);
  ~DendriticRlist();
  void init(ifstream& inputf, PropagNet * ppropagnet, ConnectMat * pconnectmat);  // initialize dendritic response list including initializing localP a pointer table to the correct part of ppropagnet->P data
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf, PropagNet * ppropagnet, ConnectMat * pconnectmat);  // restart dendritic response list including initializing localP a pointer table to the correct part of ppropagnet->P data
  void stepVa(float timestep); // Integrates each dendritic subpopulation one timestep forward
  void SumAfferent(float *V); //Sum sub potentials Va matrix to yield potential V for neural population

private: 
  DendriticR * firstlink; // link to first dendritic response in the dendritic response linked list
  int numaff; //number of afferent neural populations connected to this population
  const long numnodes;
  const int popindex; // population index number of the population in which this dendritic response list is contained
  float ** Va; // pointer to array of pointers to subpotentials from afferent tree
  float ** dVdt; // pointer to array of pointers to derivatives of subpotential from afferent tree
  DendriticR * getdendr(int i); // method to obtain pointer to "i"th dendritic response
  float **localP; // A local pointer array pointing to relevant part of P data in propagnet
};

#endif
