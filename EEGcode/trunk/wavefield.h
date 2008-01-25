/***************************************************************************
                          wavefield.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef WAVEFIELD_H
#define WAVEFIELD_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"

class Wavefield {
public: 
  Wavefield(long gridsize);
  ~Wavefield();
  void init(Istrm& inputf);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
  void update(double* URe, double* UIm);
  double *U_1Re; // Pointer to Eta one step past for use by DE solver routine
  double *U_1Im; // Pointer to Eta one step past for use by DE solver routine
  double *U_2Re; // Pointer to Eta two steps past for use by DE solver routine  
  double *U_2Im; // Pointer to Eta two steps past for use by DE solver routine  
private:
  void copyarr(double* U, double* U_1);
  double *U_ARe; // Eta stored for use by DE solver routine
  double *U_AIm; // Eta stored for use by DE solver routine
  double *U_BRe; // Eta stored for use by DE solver routine  
  double *U_BIm; // Eta stored for use by DE solver routine  
  const long gridsize; // size of grids for integration of wave equation
  long rowlength;
  long sidelength; // number of nodes along one side length of square node population
  long startfirstrow;
  long startlastrow;
 };

#endif
