/***************************************************************************
                          qhistorylist.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef QHISTORYLIST_H
#define QHISTORYLIST_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"qhistory.h"
class Poplist;

class Qhistorylist {
public:
  Qhistorylist(int numpops, long gridsize);  
  ~Qhistorylist(); 
  void init(ifstream& inputf, Poplist *ppoplist); // initialize each Q history
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf, Poplist *ppoplist); // restart each Q history
  Qhistory * getQhist(int index); // return pointer to Q history
  void updateQhistories(Poplist * ppoplist); // get Q values from populations and move keyring pointers

private:
  Qhistory * firstlink; // link to first qhistory object in linked list
  int * getdepths(); // Read in array of depth of each Q history keyring
  int * depthofhistory; // Array numpops long of the depth of each Q history keyring
  const int numhistories; // number of Q histories
};

#endif
