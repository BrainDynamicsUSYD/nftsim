/***************************************************************************
                          eqnset.h  -  wave equation propagator with K 
			               extention
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef EQNSET_H
#define EQNSET_H

#include<fstream>
using std::ofstream;
#include<iostream>

using std::endl;

#include"istrm.h"
#include"field.h"
#include"weqn.h"
#include"qhistory.h"
#include"prefact.h"

class Eqnset: public Propag {
public: 
  Eqnset(long nodes, double deltat); // For square boundary
  Eqnset(long nodes, double deltat, long longsidelength); // For rectangular boundary
  ~Eqnset();
  void init(Istrm& inputf, Qhistory* qhistory); 
  void dump(ofstream& dumpf); 
  void restart(Istrm& restartf); 
  void stepwaveeq(double* Phi, Qhistory* qhistory);
private:
  long gridsize;
  long longsidelength;
  long shortsidelength;
  long rowlength;
  long startfirstrow;
  Field **fieldRe; // An array of field objects storing past Re values of wave field
  Field **fieldIm; // An array of field objects storing past Im values of wave field
  Weqn *weqnobj;
  double* uRe; // Array for temporarily holding current u value for a component
  double* uIm; // Array for temporarily holding current u value for a component
  int numk;
  double** karray; //Array of k vectors
  Prefact** scalfactarr; //Precalculated factor exp(- k dot r)
  double deltax; // Grid spacing in space
  long centrex; //X coordinate of r=0 point in grid
  long centrey; //Y coordinate of r=0 point in grid
  long icentre;
  long iphi;
};

#endif
