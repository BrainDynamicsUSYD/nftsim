/***************************************************************************
                          qhistorylist.cpp  -  description
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "qhistorylist.h"
using std::endl;

// Builds an array of keyrings of Q history. Each Q history
// is passed a depth for its history
Qhistorylist::Qhistorylist(Istrm& inputf, std::ofstream& dumpf, int numpops, long nodes):numhistories(numpops){
  Qhistarray = new Qhistory *[numhistories];
  for(int i=0;i<numpops;i++){
    Qhistarray[i] = new Qhistory(nodes, i);
  }
  dumpf << endl; // add an endl since we no longer output q delay depths
}

Qhistorylist::~Qhistorylist(){
  for(int i=0;i<numhistories;i++)
    delete &getQhist(i);
  delete [ ] Qhistarray;
}

// Get pointer to the "index" th Qhistory object
Qhistory& Qhistorylist::getQhist(int index){
  return *Qhistarray[index];
}

void Qhistorylist::init(Istrm& inputf,Poplist& poplist){
  for(int i=0;i<numhistories; i++)
    getQhist(i).init(inputf,poplist);
}

void Qhistorylist::dump(std::ofstream& dumpf){
  for(int i=0;i<numhistories; i++){
    dumpf << "Q history for Population " << (i+1) << " ";
    getQhist(i).dump(dumpf);
  }
  dumpf << endl;
}

void Qhistorylist::restart(Istrm& restartf,Poplist& poplist){
  for(int i=0;i<numhistories; i++)
    getQhist(i).restart(restartf,poplist);
}

void Qhistorylist::updateQhistories(Poplist& poplist){
  for(int i=0;i<numhistories; i++)
    getQhist(i).updateQhistory(poplist);
}
