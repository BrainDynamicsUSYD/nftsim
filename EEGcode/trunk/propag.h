/***************************************************************************
                          propag.h  -  description
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PROPAG_H
#define PROPAG_H

#include<fstream>
using std::ofstream;
#include<iostream>

using std::endl;

#include"istrm.h"
#include"qhistory.h"

class Propag { // Abstract Base class for waveeqn object
public: 
  Propag();
  virtual ~Propag() = 0; // Must be defined in propag.cpp
  virtual void init(Istrm& inputf, Qhistory* qhistory) = 0; 
  virtual void dump(ofstream& dumpf) = 0; 
  virtual void restart(Istrm& restartf) = 0; 
  virtual void stepwaveeq(double *Phi, Qhistory* qhistory) = 0; 
};

#endif
