/***************************************************************************
                          hebb.h  - simple Hebbian plasticity rule
                             -------------------
    copyright            : (C) 2009
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef HEBB_H
#define HEBB_H

#include<cstdlib>
#include<fstream>
using std::ofstream;
#include<iostream>
#include"istrm.h"
#include"couple.h"
#include"timeseries.h"
#include"qhistorylist.h"
#include"connectmat.h"
#include"parameter.h"

class Hebb: public Couple {
public: 
  Hebb(long nodes, double deltat);
  ~Hebb();
  void init(Istrm& inputf, int coupleid); 
  void dump(ofstream& dumpf); // output values for restart
  void output(); // output variables as needed
  void updatePa(double *Pa,double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat,Couplinglist& couplinglist);

private:

  double deltat;
  int coupleid; // == dendriticr index, used for getQindex to get V

  double* nu;
  int sign;

  double rho; // linearized sigmoid
  double N; // number of synpases per neuron
  double B; // proportionality constant

  Hebb(Hebb& ); // no copy constructor
  const long nodes;
  ofstream synapoutf;
};

#endif
