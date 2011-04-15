/***************************************************************************
                          couplepast.h  -  
                             -------------------
    copyright            : (C) 2009
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef COUPLEPLAST_H
#define COUPLEPLAST_H

#include<cstdlib>
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
#include<complex>
using std::complex;

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
  const long nodes;
  ofstream synapoutf;

  const std::complex<double> i0; // purely imaginary number i
  const std::complex<double> c1; // real number 1

  double nu; // evolving coupling constant
  double rho; // constant linearized sigmoid slope
  double alpha; // constant dendritic constant
  double beta; // constant dendritic constant
  double gamma; // constant axonal constant
  double Ap;
  double Am;
  double Taup;
  double Taum;

public:
  void X( double* return_val ) const; // returns array of X(w)

private:
#define W_CUTOFF	int(5000)
#define W_STEP		int(1)
  complex<double> L[int(W_CUTOFF/W_STEP)];
  complex<double> H[int(W_CUTOFF/W_STEP)];
  complex<double> Gamma[int(W_CUTOFF/W_STEP)];
  double filter[int(W_CUTOFF/W_STEP)];
};

#endif
