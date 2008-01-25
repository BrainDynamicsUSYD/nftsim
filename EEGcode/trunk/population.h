/***************************************************************************
                          population.h  -  header file for populations object
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef POPULATION_H
#define POPULATION_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"firingr.h"
#include"dendriticrlist.h"
class ConnectMat; //forward declare ConnectMat since we are only using a pointer to it
class PropagNet; //forward declare PropagNet since we are only using a pointer to it

class Population {
public:
  Population(long totalnodes, int popindex, ConnectMat *pconnectmat);
  ~Population();
  void init(ifstream& inputf, PropagNet * ppropagnet, ConnectMat *pconnectmat);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf, PropagNet * ppropagnet, ConnectMat *pconnectmat);
  void stepPop(float timestep);
  float * V;  // Array of soma potential in population
  float * Q;  // Array of firing rate in population
  Population * next;  // Next population in linked list of Populations
private:
  const long nodes; // number of nodes in this population
  FiringR fr;  // Firing Response of the neural population
  DendriticRlist dr; // List of dendritic responses of afferent neural populations
};

#endif
