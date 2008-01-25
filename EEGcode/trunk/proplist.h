/***************************************************************************
                          proplist.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PROPLIST_H
#define PROPLIST_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"propag.h"
#include"waveeqn.h"
#include"pmap.h"
#include"eqnset.h"
#include"qhistorylist.h"
#include"connectmat.h"

class Proplist {
public: 
  Proplist(Istrm& inputf, ofstream& dumpf, int numconnects, long gridsize, double deltat);
  ~Proplist();
  void init(Istrm& inputf);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
  void step(double **Eta, Qhistorylist * pqhistorylist, ConnectMat * pconnectmat); // Transform Q to Eta via stepping forward multiple wave equations

private:
  Propag ** propagarray; // Array of pointers to propagator objects
  Propag * getpropag(int index); // method to obtain pointer to "i"th propagator
  const int numpropag; //number of propagator objects in the list
};

#endif
