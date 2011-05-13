/***************************************************************************
                          proplist.h  -  Propagator list
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PROPLIST_H
#define PROPLIST_H

#include<fstream>
#include<iostream>
#include"istrm.h"
#include"propag.h"
#include"waveeqn.h"
#include"pmap.h"
#include"eqnset.h"
#include"pharmonic.h"
#include"qhistorylist.h"
#include"connectmat.h"

class Proplist {
public: 
  Proplist(Istrm& inputf, ofstream& dumpf, int numconnects, long nodes, double deltat);
  ~Proplist();
  void init(Istrm& inputf,Qhistorylist& qhistorylist,ConnectMat& connectmat);
  void dump(std::ofstream& dumpf);
  void restart(Istrm& restartf,Qhistorylist& qhistorylist,ConnectMat& connectmat);
  void step(double **Eta,Qhistorylist& qhistorylist,ConnectMat& pconnectmat); // Transform Q to Eta via stepping forward multiple wave equations

private:
  Proplist(Proplist& ); // no copy constructor
  Propag ** propagarray; // Array of pointers to propagator objects
  Propag& getpropag(int index); // method to obtain pointer to "i"th propagator
  const int numpropag; //number of propagator objects in the list
};

#endif
