/***************************************************************************
                          coupling.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef COUPLING_H
#define COUPLING_H

#include<fstream>
using std::ofstream;
#include<iostream>
#include"istrm.h"
#include"parameter.h"
#include"couple.h"
#include"qhistorylist.h"
#include"connectmat.h"

class Coupling: public Couple {
public: 
  Coupling(long nodes, double deltat);
  ~Coupling();
  void init(Istrm& inputf, int coupleid); // initialize the nu variable
  void dump(ofstream& dumpf); // dump nu for restart
  void updatePa(double *Pa,double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat,Couplinglist& couplinglist);
  void output(); // dummy output routine - no output is generated

private:
  Coupling(Coupling& ); // no copy constructor
  const long nodes;
  Parameter* nuobj;
};

#endif
