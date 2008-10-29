/***************************************************************************
                          weqn.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef WEQN_H
#define WEQN_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"parameter.h"
#include"qhistory.h"
#include"propag.h"
#include"field.h"
#include"prefact.h"

class Weqn {
public: 
  Weqn(long gridsize, double deltat); // For square boundary
  Weqn(long gridsize,double deltat,long longsidelength,long shortsidelength); // For rectangular boudary
  ~Weqn();
  void init(Istrm& inputf, double deltax, Qhistory* pqhistory);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf, double deltax);
  void stepwaveeq(double *PhiRe, double *PhiIm, Qhistory* qhistory, Field* fieldReobj, Field* filedImobj, Prefact* prefactobj);
private:
  Parameter gammaobj;
  Parameter effrangeobj;
  double gamma;
  double effrange;
  int tauab;
  double p2; // Square of mesh ratio, dimensionless equal to (deltat effrange gamma/deltax)^2
  const long gridsize; // size of grids for integration of wave equation
  const double deltat; // Grid spacing in time
  double deltax; // Grid spacing in space
  double deltatdivideddeltaxallsquared; //Factor in p2 definition
  double twominusthreep2; // factor in wave algorithm
  double tenminusthreep2; //factor in wave algorithm
  double deltat2divided12; //factor in wave equation
  double dfact; //factor in wave equation equal to ((gamma deltat)^2)/12.
  double expfact1; //factor of Exp(- gamma deltat)
  double expfact2; //factor of Exp(- 2 gamma deltat)
  Field *Qpast; // Field object holding Q in the past in larger array
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
