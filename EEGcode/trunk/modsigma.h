/***************************************************************************
                          modsigma.h  - variance dynamics sigma modulation 
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef MODSIGMA_H
#define MODSIGMA_H

#include<fstream>
using std::ofstream;
#include<iostream>
#include"istrm.h"

class Modsigma {
public: 
  Modsigma(Istrm& inputf, int popindex);
  ~Modsigma();
  void dump(ofstream& dumpf); // dump ithreshold params for restart
  double get(double timestep, double * Vab); // return theta value for modulated threshold

private:
  Modsigma(Modsigma& ); // no copy constructor
  void initoutput(Istrm& inputf, int popindex); // initialize output routine 
  void output(); // output variance variables as needed
  double sigmatheta2; // Parameter sigma_{\theta}^2
  double sigmaVpast; // Parameter sigma_{V} one step in the past
  double avlength; // Parameter characteristic length scale of exponentially
                   // weighted averaging
  ofstream sigmaoutf; // Output stream "eegcode.sigmaout"
};

#endif
