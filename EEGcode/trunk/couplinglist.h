/***************************************************************************
                          couplinglist.h  -  description
                             -------------------
    copyright            : (C) 2007 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef COUPLINGLIST_H
#define COUPLINGLIST_H

#include<fstream>
using std::ofstream;
#include<iostream>

using std::endl;

#include"istrm.h"
#include"qhistorylist.h"
#include"connectmat.h"
class Couple; // forward reference Couple as only using pointer here. Header is included in .cpp

class Couplinglist {
public: 
  Couplinglist(Istrm& inputf, ofstream& dump, int numconnects, long nodes, double deltat);
  ~Couplinglist();
  void init(Istrm& inputf); // initialize each coupling object in list
  void dump(ofstream& dumpf); // dump each coupling coefficient for restart
  void updateP(double **P, double **Eta,Qhistorylist& qhistorylist,ConnectMat& connectmat); // Transform Eta to P via coupling terms
  void output(); // Output the nu data

private:
  Couple ** couparray; // Array of pointers to coupling objects
  Couple& getcoup(int index){return *couparray[index];}; // reference to "index" coupling object array 
  const int numcoup; // Number of coupling objects in coupling list
};

#endif
