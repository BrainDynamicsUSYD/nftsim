/***************************************************************************
                          qhistorylist.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "qhistorylist.h"

// Builds an array of keyrings of Q history. Each Q history
// is passed a depth for its history
Qhistorylist::Qhistorylist(Istrm& inputf, ofstream& dumpf, int numpops, long nodes):numhistories(numpops){
  depthofhistory= new int[numhistories]; // create depth of history array
  getdepths(inputf,dumpf); // initialize depth of history array
   
  Qhistarray = new Qhistory *[numhistories];
  for(int i=0;i<numpops;i++){
    Qhistarray[i] = new Qhistory(depthofhistory[i], nodes, i);
  }
}

Qhistorylist::~Qhistorylist(){
  for(int i=0;i<numhistories;i++)
    delete &getQhist(i);
  delete [ ] Qhistarray;
  delete [ ] depthofhistory;
}

// Get pointer to the "index" th Qhistory object
Qhistory& Qhistorylist::getQhist(int index){
  return *Qhistarray[index];
}

void Qhistorylist::init(Istrm& inputf,Poplist& poplist){
  for(int i=0;i<numhistories; i++)
    getQhist(i).init(inputf,poplist);
}

void Qhistorylist::dump(ofstream& dumpf){
  for(int i=0;i<numhistories; i++){
    dumpf << "Q history for Population " << (i+1) << " ";
    getQhist(i).dump(dumpf);
  }
}

void Qhistorylist::restart(Istrm& restartf,Poplist& poplist){
  for(int i=0;i<numhistories; i++)
    getQhist(i).restart(restartf,poplist);
}

void Qhistorylist::getdepths(Istrm& inputf, ofstream& dumpf){
  int temp;
  dumpf << "Q delay depths ";
  for(int i=0;i<numhistories; i++){
    inputf.ignore(200,58); // throw away everything uptil colon character
    inputf >> temp;
    dumpf << ": " << temp;
    depthofhistory[i]=temp;
  }
  dumpf << endl;
}

void Qhistorylist::updateQhistories(Poplist& poplist){
  for(int i=0;i<numhistories; i++)
    getQhist(i).updateQhistory(poplist);
}
