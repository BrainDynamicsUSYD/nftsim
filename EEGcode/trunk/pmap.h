/***************************************************************************
                          pmap.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PMAP_H
#define PMAP_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"parameter.h"
#include"qhistory.h"
#include"propag.h"

class Pmap: public Propag {
public: 
  Pmap(long gridsize, float deltat);
  ~Pmap();
  void init(Istrm& inputf);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
  void stepwaveeq(float *Phi, Qhistory* qhistory);
private:
  const long gridsize;
  int tauab;
  const float deltat; // Grid spacing in time
  long icentre;
  long iphi;
  long rowlength;
  long sidelength;
  long startfirstrow;
};

#endif
