/***************************************************************************
                          waveeqn.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef WAVEEQN_H
#define WAVEEQN_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"effrange.h"
#include"gamma.h"
#include"qhistory.h"

class WaveEqn {
public: 
  WaveEqn(long gridsize, float deltat, float deltax);
  ~WaveEqn();
  void init(ifstream& inputf);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf);
  void stepwaveeq(float *Phi, Qhistory* qhistory);
private:
  Gamma gammaobj;
  EffRange effrangeobj;
  float gamma;
  float effrange;
  int tauab;
  bool islocal;
  float p2; // Square of mesh ratio, dimensionless equal to (deltat effrange gamma/deltax)^2
  const long gridsize; // size of grids for integration of wave equation
  const float deltat; // Grid spacing in time
  const float deltax; // Grid spacing in space
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
