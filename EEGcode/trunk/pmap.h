/***************************************************************************
                          pmap.h  -  Propagator which directly maps
                                     \phi(t)= Q(t-\tau)
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PMAP_H
#define PMAP_H

#include<fstream>
using std::ofstream;
#include<iostream>

using std::endl;

#include"istrm.h"
#include"parameter.h"
#include"qhistory.h"
#include"propag.h"

class Pmap: public Propag {
public: 
  Pmap(long nodes, double deltat);
  ~Pmap();
  void init(Istrm& inputf,Qhistory& qhistory);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
  void stepwaveeq(double *Phi,Qhistory& qhistory);
private:
  const long nodes;
  int tauab;
  const double deltat; // Grid spacing in time
};

#endif
