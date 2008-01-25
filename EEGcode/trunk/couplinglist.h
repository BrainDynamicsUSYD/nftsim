/***************************************************************************
                          couplinglist.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef COUPLINGLIST_H
#define COUPLINGLIST_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

class Coupling; // forward reference Coupling as only using pointer here. Header is included in .cpp

class Couplinglist {
public: 
  Couplinglist(int numconnects);
  ~Couplinglist();
  void init(ifstream& inputf); // initialize each coupling object in list
  void dump(ofstream& dumpf); // dump each coupling coefficient for restart
  void restart(ifstream& restartf); // restart each coupling object in list
  void updateP(float **P, float **Eta, long nodes); // Transform Eta to P via coupling terms
                                                    // NOTE: CRUCIAL that coupling maps between larger
						    // periodic boundary conditions "gridsize" and
						    // smaller "nodes" grid

private:
  Coupling * firstlink; // link to first coupling object in linked list
  Coupling * getcoup(int index); // method to obtain pointer to "i" coupling object in linked list
  const int numcoup; // Number of coupling objects in coupling list
};

#endif
