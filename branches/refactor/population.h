/***************************************************************************
                          population.h  -  header file for populations object
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef POPULATION_H
#define POPULATION_H

#include<fstream>
using std::ofstream;
#include<iostream>
#include"configf.h"
#include"firingr.h"
#include"dendriticrlist.h"
#include"timeseries.h"
class ConnectMat; //forward declare ConnectMat. Header file included in .cpp file
class PropagNet; //forward declare PropagNet since we are only using a pointer to it

class Population {
public:
  Population(long nodes, int popindex,ConnectMat& connectmat);
  ~Population();
  void init(Configf& inputf,PropagNet& propagnet,ConnectMat& connectmat);
  void dump(ofstream& dumpf);
  void restart(Configf& restartf,PropagNet& propagnet,ConnectMat& connectmat);
  void stepPop(double timestep);
  double * Q;  // Array of firing rate in population
  double * V;  // Array of soma potential in population (NULL if this is a stimulus population)
private:
  Population(Population& ); // no copy constructor
  double t; // Current time
  int pindex; // Population index
  bool isstimulus; // TRUE if this population is a stimulus population (i.e. has no attached populations on dendrite tree
  const long nodes; // number of nodes in this population
  FiringR* pfr;  // Pointer to Firing Response of the neural population (NULL if this is a stimulus population)
  DendriticRlist* pdr; // Pointer to List of dendritic responses of afferent neural populations (NULL if this is a stimulus population)
  Timeseries* pstimulus; // Pointer to Stimulus of the neural population (NULL if this is not a stimulus population)
};

#endif
