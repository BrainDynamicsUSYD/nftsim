/***************************************************************************
                          connectmat.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef CONNECTMAT_H
#define CONNECTMAT_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

class ConnectMat {
public: 
  ConnectMat(int numpops, int numconct);
  ~ConnectMat();
  void init(ifstream& inputf); // Read in raw connection matrix and setup qphiconnect
  void dump(ofstream& dumpf); // Write out raw connection matrix
  int getQindex(int index){return qphiconnect[index];}; // Return population index number of Q for given wave equation index
  int getDRindex(int index){return drphiconnect[index];}; // Return population index number for DR for given wave equation index
  int getDRlength(int index){return drlength[index];}; // Return the number of dendritic responses attached to particular population

private:
  int *qphiconnect; // Array mapping connection number back to Qb arrays
  int *drphiconnect; // Array mapping connection number back to dendritic responses populations
  int *drlength; // Array each element is the number of dendritic responses attached to population number 'index'
  int *rawcntmat; //Raw connection matrix
  const int numconnect;
  const int nump;
};

#endif
