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
#include"wavefield.h"
#include"prefact.h"

class Weqn {
public: 
  Weqn(long gridsize, double deltat);
  ~Weqn();
  void init(Istrm& inputf, double deltax);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf, double deltax);
  void stepwaveeq(double *PhiRe, double *PhiIm, Qhistory* qhistory, Wavefield* fieldobj, Prefact* prefactobj);
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
  double twominusfourp2; // factor in wave algorithm
  double tenminusfourp2; //factor in wave algorithm
  double deltat2divided12; //factor in wave equation
  double dfact; //factor in wave equation equal to ((gamma deltat)^2)/12.
  double expfact1; //factor of Exp(- gamma deltat)
  double expfact2; //factor of Exp(- 2 gamma deltat)
  long rowlength;
  long sidelength; // number of nodes along one side length of square node population
  long startfirstrow;
  long startlastrow;
  long icentre, itop, ibottom, ileft, iright;
  long iphi;
};

#endif
