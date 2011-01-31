/***************************************************************************
                          modtheta1.h  - Another Bursting model threshold modulation 
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef MODTHETA1_H
#define MODTHETA1_H

#include<fstream>
using std::ofstream;
#include<iostream>
#include"istrm.h"

class Modtheta1 {
public: 
  Modtheta1(Istrm& inputf,int popindex);
  ~Modtheta1();
  void dump(ofstream& dumpf); // dump ithreshold params for restart
  double get(double timestep,double* V,double Qmax,double sigma); // return theta value for modulated threshold

private:
  Modtheta1(Modtheta1& ); // no copy constructor
  void initoutput(Istrm& inputf, int popindex); // initialize output routine 
  void output(double* y, double itheta, double V); // output threshold variables as needed
  void rk4(double t, double* y, float timestep,double* V,double Qmax,double sigma); // 4th order Runge Kutta
  void rkderivs(double t, double* y, double* dydt,double* V,double Qmax,double sigma); // Evolution equations
// The following variables relate to a variant of the bursting threshold model in 
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
  long threshtraces; // number of concentration traces to output
  long * threshnodes; // Array containing node numbers of outputted threshold traces
  ofstream threshoutf; // Stream 'eegcode.threshout'
  double currenttime; // Counter holding current time
};

#endif
