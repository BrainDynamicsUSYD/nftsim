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
using std::cerr;
using std::endl;

#include"istrm.h"
#include"parameter.h"
#include"qhistory.h"
#include"propag.h"

class WaveEqn: public Propag {
public: 
  WaveEqn(long gridsize, float deltat);
  ~WaveEqn();
  void init(Istrm& inputf);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
  void stepwaveeq(float *Phi, Qhistory* qhistory);
private:
  Parameter gammaobj;
  Parameter effrangeobj;
  float gamma;
  float effrange;
  int tauab;
  float p2; // Square of mesh ratio, dimensionless equal to (deltat effrange gamma/deltax)^2
  const long gridsize; // size of grids for integration of wave equation
  const float deltat; // Grid spacing in time
  float deltax; // Grid spacing in space
  float deltatdivideddeltaxallsquared; //Factor in p2 definition
  float twominusfourp2; // factor in wave algorithm
  float tenminusfourp2; //factor in wave algorithm
  float deltat2divided12; //factor in wave equation
  float dfact; //factor in wave equation equal to ((gamma deltat)^2)/12.
  float expfact1; //factor of Exp(- gamma deltat)
  float expfact2; //factor of Exp(- 2 gamma deltat)
  float *Phi_1; // Eta one step into past stored for use by DE solver routine
  float *Phi_2; // Eta two steps into past stored for use by DE solver routine  
  long rowlength;
  long sidelength; // number of nodes along one side length of square node population
  long startfirstrow;
  long startlastrow;
  long icentre, itop, ibottom, ileft, iright;
  long iphi;
};

#endif
