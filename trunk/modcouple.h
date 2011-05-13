/***************************************************************************
                          modcouple.h  -  
                             -------------------
    copyright            : (C) 2007 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef MODCOUPLE_H
#define MODCOUPLE_H

#include<fstream>
using std::ofstream;
#include<iostream>
#include"istrm.h"
#include"couple.h"
#include"timeseries.h"
#include"qhistorylist.h"
#include"connectmat.h"

class Modcouple: public Couple {
public: 
  Modcouple(long nodes, double deltat);
  ~Modcouple();
  void init(Istrm& inputf, int coupleid); 
  void dump(ofstream& dumpf); // dump nu for restart
  void output(); // output nu variable as needed
  void updatePa(double *Pa, double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat,Couplinglist& couplinglist);

private:
  Modcouple(Modcouple& ); // no copy constructor
  void initoutput(Istrm& inputf, int coupleid); // initialize output routine for nu
  double t; //current time
  const long nodes;
  double timestep;
  double nuzero; // \nu_0, the zero concentration signal strength
  double nuscal; // \nu_{\infty} / \nu_0 the scale factor to obtain the asymptotic nu value
  double mu; // first time constant of neurotransmitter metabolism
  double lambda; // second time constant of neurotrasmitter metabolism
  double k; // Constant in exponential of H
  double *conc; // Pointer to concentration object
  double *previousconc; // neurotransmitter concentration one step back
  double *h; // H is the neurotransmitter intermediate variable
  double *dhdt; // time derivative of H
  double *nu; // Array of nu synaptic strengths
  double expmu; // Factor in exactstep
  double explambda; // Factor in exactstep
  Timeseries* pconcobj; // Neurotransmitter concentration timeseries
  long synaptraces; // number of synaptic traces to output
  long conctraces; // number of concentration traces to output
  long * synnodes; // Array containing node numbers of outputted synaptic traces
  long * concnodes; // Array containing node numbers of outputted concentration traces
  ofstream synapoutf; // Stream 'neurofield.synapout'
  ofstream concoutf; // Stream 'neurofield.concout'
};

#endif
