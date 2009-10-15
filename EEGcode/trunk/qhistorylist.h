/***************************************************************************
                          qhistorylist.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef QHISTORYLIST_H
#define QHISTORYLIST_H

#include<fstream>
using std::ofstream;
#include<iostream>

using std::endl;

#include"istrm.h"
#include"qhistory.h"
class Poplist;

class Qhistorylist {
public:
  Qhistorylist(Istrm& inputf, ofstream& dumpf, int numpops, long nodes);  
  ~Qhistorylist(); 
  void init(Istrm& inputf,Poplist& poplist); // initialize each Q history
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf,Poplist& poplist); // restart each Q history
  Qhistory& getQhist(int index); // reference to Q history "i"
  void updateQhistories(Poplist& poplist); // get Q values from populations and move keyring pointers

private:
  Qhistory ** Qhistarray; // Array of pointers to Qhistory objects
  void getdepths(Istrm& inputf, ofstream& dumpf); // Read in array of depth of each Q history keyring
  int * depthofhistory; // Array numpops long of the depth of each Q history keyring
  const int numhistories; // number of Q histories
};

#endif
