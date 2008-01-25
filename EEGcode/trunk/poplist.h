/***************************************************************************
                          poplist.h  -  Header file for Poplist object
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef POPLIST_H
#define POPLIST_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"population.h"
class ConnectMat; //forward declare ConnectMat. Since we only use pointer to it no need for header
class PropagNet; //forward declare PropagNet. Since we only use pointer to it no need for header

class Poplist {
public: 
  Poplist(long nodes, int numberofpops, ConnectMat * pconnectmat);
  ~Poplist();
  void init(ifstream& inputf, PropagNet * ppropagnet, ConnectMat * pconnectmat);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf, PropagNet * ppropagnet, ConnectMat * pconnectmat);
  void stepPops(float timestep);
  Population * get(int popindex);
private:
  Population * firstlink; // link to first population in the population linked list
  const int numpops; //number of populations in linked list of populations
};

#endif
