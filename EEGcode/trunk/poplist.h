/***************************************************************************
                          poplist.h  -  Header file for Poplist object
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef POPLIST_H
#define POPLIST_H

#include<fstream>
using std::ofstream;
#include<iostream>

using std::endl;

#include"istrm.h"
#include"population.h"
class ConnectMat; //forward declare ConnectMat
class PropagNet; //forward declare PropagNet

class Poplist {
public: 
  Poplist(long nodes, int numberofpops,ConnectMat& connectmat);
  ~Poplist();
  void init(Istrm& inputf,PropagNet& propagnet,ConnectMat& connectmat);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf,PropagNet& propagnet,ConnectMat& connectmat);
  void stepPops(double timestep);
  Population& get(int popindex);
private:
  Population** poparray; // Array of pointers to population objects
  const int numpops; //number of populations in array of populations
};

#endif
