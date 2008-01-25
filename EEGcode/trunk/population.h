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
#include"stimulus.h"
class ConnectMat; //forward declare ConnectMat. Header file included in .cpp file
class PropagNet; //forward declare PropagNet since we are only using a pointer to it

class Population {
public:
  Population(long totalnodes, int popindex, ConnectMat *pconnectmat);
  ~Population();
  void init(ifstream& inputf, PropagNet * ppropagnet, ConnectMat *pconnectmat);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf, PropagNet * ppropagnet, ConnectMat *pconnectmat);
  void stepPop(float timestep);
  float * Q;  // Array of firing rate in population
private:
  bool isstimulus; // TRUE if this population is a stimulus population (i.e. has no attached populations on dendrite tree
  float * V;  // Array of soma potential in population (NULL if this is a stimulus population)
  const long nodes; // number of nodes in this population
  FiringR* pfr;  // Pointer to Firing Response of the neural population (NULL if this is a stimulus population)
  DendriticRlist* pdr; // Pointer to List of dendritic responses of afferent neural populations (NULL if this is a stimulus population)
  Stimulus* pstimulus; // Pointer to Stimulus of the neural population (NULL is this is not a stimulus population)
};

#endif
