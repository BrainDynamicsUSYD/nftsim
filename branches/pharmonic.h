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

#include"configf.h"
#include"qhistory.h"
#include"propag.h"
#include"tau.h"

class Pharmonic: public Propag {
public: 
  Pharmonic(long nodes, double deltat);
  ~Pharmonic();
  void init(Configf& inputf,Qhistory& qhistory);
  void dump(ofstream& dumpf);
  void restart(Configf& restartf,Qhistory& qhistory);
  void stepwaveeq(double *Phi,Qhistory& qhistory);
private:
  Pharmonic(Pharmonic& ); // no copy constructor
  const long nodes;
  Tau* tauobj;
  const double timestep; // Grid spacing in time
  double* previousQ; // Store one step in past firing rate field
  double* previousPhi; // Store one step in past phi field
  double* dPhidt; // Store estimated Phi time derivative
  double gamma;
};

#endif
