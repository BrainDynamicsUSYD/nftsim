/***************************************************************************
                          tau.h  -  time delay object
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef TAU_H
#define TAU_H

#include<fstream>
using std::ofstream;
#include<iostream>

#include"istrm.h"
class Qhistory; //forward declare Qhistory

class Tau {
public: 
  Tau(long nodes,double deltat,Istrm& inputf,Qhistory& qhistory);
  ~Tau();
  void dump(ofstream& dumpf);
  bool isarraytau; // TRUE if time delays are in the form of an array
  int tauab; // stores a single value tau
  double* qarray; // array to pass to qhistory when isarraytau true to hold
                  // q return values. Not needed when a single value tau exists
  long* tauarr; //stores an array of tau values
private:
  Tau(Tau& ); // no copy constructor
  const long nodes;
  const double deltat; // Grid spacing in time
};

#endif
