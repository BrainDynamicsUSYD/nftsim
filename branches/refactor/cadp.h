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

class CaDP: public Couple {
public: 
  CaDP(long nodes, double deltat);
  ~CaDP();
  void init(Istrm& inputf, int coupleid); 
  void dump(ofstream& dumpf); // output values for restart
  void output(); // output variables as needed
  void updatePa(double *Pa,double *Etaa,double const *postV,double const *glu);

private:
  double sig( double x, double beta ) const;
  double potentiate(double Ca) const;
  double depress(double Ca) const;

  double deltat;
  int coupleid; // == dendriticr index, used for getQindex to get V

  double* V; // postsynaptic potential, with 2D spatial dependence
  double* nu;
  double* Ca;
  double* binding; // glutamate binding

  double rho; // linearized sigmoid
  double N; // number of synpases per neuron
  double B; // 1/stanard deviation of glutamate binding

  double nu_max; // maximum synaptic strength
  double nu_th; // threshold time-scale of plasticity
  double nu_ltd; // time-scale of depression
  double nu_ltp; // time-scale of potentiation

  CaDP(CaDP& ); // no copy constructor
  const long nodes;
  ofstream synapoutf;
  ofstream caoutf;
  ofstream voutf;
  ofstream bindoutf;
};

#endif
