/***************************************************************************
                          couplepast.h  -  
                             -------------------
    copyright            : (C) 2009
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef COUPLEPLAST_H
#define COUPLEPLAST_H

#include<fstream>
using std::ofstream;
#include<iostream>
#include"istrm.h"
#include"couple.h"
#include"timeseries.h"
#include"qhistorylist.h"
#include"connectmat.h"

class Coupleplast: public Couple {
public: 
  Coupleplast(long nodes, double deltat);
  ~Coupleplast();
  void init(Istrm& inputf, int coupleid); 
  void dump(ofstream& dumpf); // output values for restart
  void output(); // output variables as needed
  void updatePa(double *Pa,double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat);

private:
  Coupleplast(Coupleplast& ); // no copy constructor
  double t; //current time
  const long nodes;
  double timestep;
};

#endif
