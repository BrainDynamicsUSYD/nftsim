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
using std::cerr;
using std::endl;
#include<iomanip>
using std::setprecision;

#include"istrm.h"
#include"couple.h"
#include"timeseries.h"

class Modcouple: public Couple {
public: 
  Modcouple(long nodes, double deltat);
  ~Modcouple();
  void init(Istrm& inputf, int coupleid); 
  void dump(ofstream& dumpf); // dump nu for restart
  void restart(Istrm& restartf, int coupleid); // restart the nu variable
  void output(); // output nu variable as needed
  void updatePa(double *Pa, double *Etaa);

private:
  void initoutput(Istrm& inputf, int coupleid); // initialize output routine for nu
  double t; //current time
  long nodes;
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
  ofstream synapoutf; // Stream 'eegcode.synapout'
  ofstream concoutf; // Stream 'eegcode.concout'
};

#endif
