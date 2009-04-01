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
using std::cerr;
using std::endl;
#include<iomanip>
using std::setprecision;

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
  void updatePa(double *Pa, double *Etaa, Qhistorylist* pqhistorylist, ConnectMat* pconnectmat);

private:
  double t; //current time
  long nodes;
  double timestep;
};

#endif
