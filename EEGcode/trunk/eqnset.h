/***************************************************************************
                          eqnset.h  -  wave equation propagator with K 
			               extention
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef EQNSET_H
#define EQNSET_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"wavefield.h"
#include"weqn.h"
#include"qhistory.h"
#include"prefact.h"

class Eqnset: public Propag {
public: 
  Eqnset(long gridsize, double deltat);
  ~Eqnset();
  void init(Istrm& inputf); 
  void dump(ofstream& dumpf); 
  void restart(Istrm& restartf); 
  void stepwaveeq(double* Phi, Qhistory* qhistory);
private:
  const long gridsize;
  long sidelength;
  long rowlength;
  long startfirstrow;
  long totalnodes;
  Wavefield **field; // An array of wave field objects storing past values of wave field
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
