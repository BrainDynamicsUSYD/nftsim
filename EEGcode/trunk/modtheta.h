/***************************************************************************
                          modtheta.h  - Bursting model threshold modulation 
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef MODTHETA_H
#define MODTHETA_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;
#include<iomanip>
using std::setprecision;

#include"istrm.h"
#include"timeseries.h"

class Modtheta {
public: 
  Modtheta();
  ~Modtheta();
  void init(Istrm& inputf, int popindex); 
  void dump(ofstream& dumpf); // dump ithreshold params for restart
  void restart(Istrm& restartf, int popindex); // restart
  double get(double timestep); // return theta value for modulated threshold

private:
  void initoutput(Istrm& inputf, int popindex); // initialize output routine 
  void output(double* y, double itheta); // output threshold variables as needed
  void rk4(double t, double* y, float timestep); // 4th order Runge Kutta
  void rkderivs(double t, double* y, double* dydt, double I); // Evolution equations
  long nodes;
// The following variables relate to the bursting threshold model in 
// Robinson et al. J. of Theoret. Biololgy (2008) 250:663-672
  double Ia; // Ia current density 
  double Ib; // Ib current density
  double Ic; // Ic current denisty
  double taux; // tau_{x} time constant
  double tauh; // tau_{h} time constant
  double Ax; // A_{x}
  double mu; // Mu 
  double initXtilda; // Initial value for X^{tilda}
  double initHtilda; // Initial value for H^{tilda}
  double* y; // Storing H^{tilda} and X^{tilda}
  double* dydt; // Storing the time derivatives of above values
  Timeseries* pI; // Timeseries for effective current 
  long threshtraces; // number of concentration traces to output
  long * threshnodes; // Array containing node numbers of outputted threshold traces
  ofstream threshoutf; // Stream 'eegcode.threshout'
  double currenttime; // Counter holding current time
};

#endif