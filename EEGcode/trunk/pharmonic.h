/***************************************************************************
                          pharmonic.h  -  Propagator which is a harmonic oscillator
                                     and has no spatial coupling
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PHARMONIC_H
#define PHARMONIC_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"parameter.h"
#include"qhistory.h"
#include"propag.h"

class Pharmonic: public Propag {
public: 
  Pharmonic(long totalnodes, double deltat);
  ~Pharmonic();
  void init(Istrm& inputf, Qhistory* qhistory);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
  void stepwaveeq(double *Phi, Qhistory* qhistory);
private:
  const long nodes;
  int tauab;
  const double timestep; // Grid spacing in time
  double* previousQ; // Store one step in past firing rate field
  double* previousPhi; // Store one step in past phi field
  double* dPhidt; // Store estimated Phi time derivative
  Parameter* gammaobj;
};

#endif
