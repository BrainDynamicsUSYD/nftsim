/***************************************************************************
                          cadp.h  - plasticity rule according to Shouval et al
                             -------------------
    copyright            : (C) 2009
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef CADP_H
#define CADP_H

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

class CaDP: public Couple {
public: 
  CaDP(long nodes, double deltat);
  ~CaDP();
  void init(Istrm& inputf, int coupleid); 
  void dump(ofstream& dumpf); // output values for restart
  void output(); // output variables as needed
  void updatePa(double *Pa,double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat,Couplinglist& couplinglist);

private:

  int coupleid;
  double* V; // postsynaptic potential

  CaDP(CaDP& ); // no copy constructor
  const long nodes;
  ofstream synapoutf;
};

#endif
