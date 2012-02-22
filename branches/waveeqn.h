/***************************************************************************
                          waveeqn.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef WAVEEQN_H
#define WAVEEQN_H

#include<fstream>
using std::ofstream;
#include<iostream>

#include"configf.h"
#include"qhistory.h"
#include"propag.h"
#include"field.h"
#include"tau.h"

class WaveEqn: public Propag {
public: 
  WaveEqn(long nodes, double deltat, long longside); // For rectangular boundary
  ~WaveEqn();
  void init(Configf& inputf,Qhistory& qhistory);
  void dump(ofstream& dumpf);
  void restart(Configf& restartf,Qhistory& qhistory);
  void stepwaveeq(double *Phi,Qhistory& qhistory);
private:
  WaveEqn(WaveEqn& ); // no copy contructor
  double effrange;
  double gamma;
  Tau* tauobj;
  double p2; // Square of mesh ratio, dimensionless equal to (deltat effrange gamma/deltax)^2
  const long nodes;
  const double deltat; // Grid spacing in time
  double deltax; // Grid spacing in space
  double deltatdivideddeltaxallsquared; //Factor in p2 definition
//  double twominusfourp2; // factor in wave algorithm
  double twominusthreep2; // factor in wave algorithm
//  double tenminusfourp2; //factor in wave algorithm
  double tenminusthreep2; //factor in wave algorithm
  double deltat2divided12; //factor in wave equation
  double dfact; //factor in wave equation equal to ((gamma deltat)^2)/12.
  double expfact1; //factor of Exp(- gamma deltat)
  double expfact2; //factor of Exp(- 2 gamma deltat)
  Field *phipast; // Field object holding Phi in past in larger array
  Field *Qpast; // Field object holding Q in the past in larger array
  long gridsize;
  long rowlength;
  long longsidelength;
  long shortsidelength;
  long startfirstrow;
  long startlastrow;
  long icentre, itop, ibottom, ileft, iright;
  long itopleft,itopright,ibottomleft,ibottomright;
  long iphi;
};

#endif
