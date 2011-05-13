/***************************************************************************
                          connectmat.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef CONNECTMAT_H
#define CONNECTMAT_H

#include<fstream>
#include<iostream>

#include"istrm.h"

class ConnectMat {
public: 
  ConnectMat(int numpops, int numconct, Istrm& inputf); // Read in raw connection matrix and setup qphiconnect
  ~ConnectMat();
  void dump(std::ofstream& dumpf); // Write out raw connection matrix
  int getQindex(int index){return qphiconnect[index];}; // Return population index number of Q for given wave equation index
  int getDRindex(int index){return drphiconnect[index];}; // Return population index number for DR for given wave equation index
  int getDRlength(int index){return drlength[index];}; // Return the number of dendritic responses attached to particular population

private:
  ConnectMat(ConnectMat& ); // no copy constructor
  int *qphiconnect; // Array mapping connection number back to Qb arrays
  int *drphiconnect; // Array mapping connection number back to dendritic responses populations
  int *drlength; // Array each element is the number of dendritic responses attached to population number 'index'
  int *rawcntmat; //Raw connection matrix
  const int numconnect;
  const int nump;
};

#endif
