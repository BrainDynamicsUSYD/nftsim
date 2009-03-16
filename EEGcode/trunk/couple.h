/***************************************************************************
                          couple.h  -  description
                             -------------------
    copyright            : (C) 2007 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef COUPLE_H
#define COUPLE_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"qhistorylist.h"
#include"connectmat.h"

class Couple { // Abstract Base class for coupling type objects
public: 
  Couple();
  virtual ~Couple() = 0; // Must be defined in couple.cpp
  virtual void init(Istrm& inputf, int coupleid) = 0; 
  virtual void dump(ofstream& dumpf) = 0; 
  virtual void restart(Istrm& restartf, int coupleid) = 0; 
  virtual void updatePa(double *Pa, double *Etaa, Qhistorylist* pqhistorylist, ConnectMat* pconnectmat) = 0;
  virtual void output() = 0; 
};

#endif
