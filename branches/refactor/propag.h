/***************************************************************************
                          propag.h  -  description
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PROPAG_H
#define PROPAG_H

#include<fstream>
#include<iostream>
class Istrm; //Forward declare Istrm
class Qhistory; //Forward declare qhistory

class Propag { // Abstract Base class for waveeqn object
public: 
  Propag();
  virtual ~Propag() = 0; // Must be defined in propag.cpp
  virtual void init(Istrm& inputf,Qhistory& qhistory) = 0; 
  virtual void dump(std::ofstream& dumpf) = 0; 
  virtual void restart(Istrm& restartf,Qhistory& qhistory) = 0; 
  virtual void stepwaveeq(double *Phi,Qhistory& qhistory) = 0; 
};

#endif
