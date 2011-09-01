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
#include"istrm.h"
#include"qhistorylist.h"
#include"connectmat.h"
class Couple; // forward reference Couple as only using pointer here. Header is included in .cpp

class Couplinglist {
public: 
  double* glu; // glutamate concentration in synaptic cleft
  Couplinglist(Istrm& inputf, ofstream& dump, int numconnects, long nodes, double deltat);
  ~Couplinglist();
  void init(Istrm& inputf); // initialize each coupling object in list
  void dump(ofstream& dumpf); // dump each coupling coefficient for restart
  void updateP(double **P, double **Eta,Qhistorylist& qhistorylist,ConnectMat& connectmat); // Transform Eta to P via coupling terms
  void output(); // Output the nu data
  Couple* getcoup(int index){
    if( index < numcoup )
      return couparray[index]; // reference to "index" coupling object array 
    else
      return getcoup(index-1); // return the largest valid indexed object
  }

private:
  Couplinglist(Couplinglist& ); // no copy constructor
  Couple ** couparray; // Array of pointers to coupling objects
  const int numcoup; // Number of coupling objects in coupling list
  long numnodes;
  double deltat;
  double Lambda; // proportionality constant between excitatory eta and glu
  double* dglu;
};

#endif
