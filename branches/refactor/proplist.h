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
#include<vector>
using std::vector;
#include"configf.h"
#include"propag.h"
#include"waveeqn.h"
#include"pmap.h"
#include"eqnset.h"
#include"pharmonic.h"
#include"qhistorylist.h"
#include"connectmat.h"

class Proplist {
public: 
  Proplist(Configf& inputf, ofstream& dumpf, int numconnects, long nodes, double deltat);
  ~Proplist();
  void init(Configf& inputf,Qhistorylist& qhistorylist,ConnectMat& connectmat);
  void dump(std::ofstream& dumpf);
  void restart(Configf& restartf,Qhistorylist& qhistorylist,ConnectMat& connectmat);
  void step(double **Eta,Qhistorylist& qhistorylist,ConnectMat& pconnectmat); // Transform Q to Eta via stepping forward multiple wave equations

private:
  Proplist(Proplist& ); // no copy constructor
  vector<Propag*> propagarray; // Array of pointers to propagator objects
  Propag& getpropag(int index); // method to obtain pointer to "i"th propagator
  const int numpropag; //number of propagator objects in the list
};

#endif
