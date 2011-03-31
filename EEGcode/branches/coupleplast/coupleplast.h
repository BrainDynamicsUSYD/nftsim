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
#include<complex>
#include"istrm.h"
#include"couple.h"
#include"timeseries.h"
#include"qhistorylist.h"
#include"connectmat.h"
#include"parameter.h"

class Coupleplast: public Couple {
public: 
  Coupleplast(long nodes, double deltat);
  ~Coupleplast();
  void init(Istrm& inputf, int coupleid); 
  void dump(ofstream& dumpf); // output values for restart
  void output(); // output variables as needed
  void updatePa(double *Pa,double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat);
  double get(void) const; // return nu of this population

private:
  double f_e( double w );
  Coupleplast(Coupleplast& ); // no copy constructor
  double t; //current time
  double nu;
  Parameter* alphaobj;
  Parameter* betaobj;
  const long nodes;
  double timestep;
  ofstream synapoutf;
  std::complex<double> i0; // purely imaginary number i
  std::complex<double> c1; // real number 1
};

#endif
