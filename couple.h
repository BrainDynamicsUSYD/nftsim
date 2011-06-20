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
#include"couplinglist.h"
#include<complex>
using std::complex;

class Couple { // Abstract Base class for coupling type objects
public: 
  Couple();
  virtual ~Couple() = 0; // Must be defined in couple.cpp
  virtual void init(Istrm& inputf, int coupleid) = 0; 
  virtual void dump(std::ofstream& dumpf) = 0; 
  virtual void updatePa(double *Pa,double *Etaa,Qhistorylist& qhistorylist, ConnectMat& connectmat,Couplinglist& couplinglist) = 0;
  // return X(w) = G L(w) Gamma(w) == 0 unless *this is a Coupleplast
  virtual complex<double> X( int i ) const { return complex<double>(0,0); };
  virtual void output() = 0; 
  int sign; // == +-1, indicating excitatory or inhibitory coupling
};

#endif
