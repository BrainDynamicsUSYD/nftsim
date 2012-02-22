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
#include<iostream>
#include"configf.h"
#include"qhistory.h"
#include"propag.h"
#include"tau.h"

class Pmap: public Propag {
public: 
  Pmap(long nodes, double deltat);
  ~Pmap();
  void init(Configf& inputf,Qhistory& qhistory);
  void dump(std::ofstream& dumpf);
  void restart(Configf& restartf,Qhistory& qhistory);
  void stepwaveeq(double *Phi,Qhistory& qhistory);
private:
  Pmap(Pmap& ); // no copy constructor
  const long nodes;
  Tau* tauobj;
  const double deltat; // Grid spacing in time
};

#endif
