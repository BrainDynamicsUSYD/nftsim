/***************************************************************************
                          couple.h  -  description
                             -------------------
    copyright            : (C) 2007 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef COUPLE_H
#define COUPLE_H

#include<fstream>
#include<iostream>
#include"istrm.h"
#include"qhistorylist.h"
#include"connectmat.h"

class Couple { // Abstract Base class for coupling type objects
public: 
  Couple();
  virtual ~Couple() = 0; // Must be defined in couple.cpp
  virtual void init(Istrm& inputf, int coupleid) = 0; 
  virtual void dump(std::ofstream& dumpf) = 0; 
  virtual void updatePa(double *Pa,double *Etaa,Qhistorylist& qhistorylist, ConnectMat& connectmat) = 0;
  virtual void output() = 0; 
};

#endif
