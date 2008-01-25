/***************************************************************************
                          qhistorylist.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "qhistorylist.h"

//
// Builds a linked list of keyrings of Q history. Each Q history
// is passed a depth for its history
//

Qhistorylist::Qhistorylist(int numpops, long gridsize):numhistories(numpops){
   depthofhistory= new int[numhistories]; // create depth of history array
   getdepths(); // initialize depth of history array
   Qhistory *currentlink;
   Qhistory *nextlink;
   int histindex=0; // index to iterate over depthofhistory array
   firstlink = new Qhistory(depthofhistory[histindex], gridsize, histindex);
   numpops--, histindex++;
   currentlink=firstlink;
   for(; numpops>0; numpops--, histindex++){
     nextlink= new Qhistory(depthofhistory[histindex], gridsize, histindex);
    currentlink->next=nextlink;
    currentlink=nextlink;
    } 
}

Qhistorylist::~Qhistorylist(){
  int i=numhistories;
  for(;i>0; i--)
    delete getQhist(i-1);
  delete depthofhistory;
}

//
// Get pointer to the "index" th Qhistory object
//

Qhistory * Qhistorylist::getQhist(int index){
  Qhistory *currentlink;
  if(index<numhistories){
    currentlink=firstlink;
    for(;index>0; index--)
      currentlink=currentlink->next;
    }
  else {
    currentlink=0;
    }
  return currentlink;
}

void Qhistorylist::init(ifstream& inputf, Poplist *ppoplist){
  for(int i=0;i<numhistories; i++)
    getQhist(i)->init(inputf, ppoplist);
}

void Qhistorylist::dump(ofstream& dumpf){
  for(int i=0;i<numhistories; i++){
    dumpf << "Q history for Population " << (i+1) << " ";
    getQhist(i)->dump(dumpf);
  }
}

void Qhistorylist::restart(ifstream& restartf, Poplist *ppoplist){
  for(int i=0;i<numhistories; i++)
    getQhist(i)->restart(restartf, ppoplist);
}

int * Qhistorylist::getdepths(){
//
// *** write input routine here
//
}

void Qhistorylist::updateQhistories(Poplist * ppoplist){
  for(int i=0;i<numhistories; i++)
    getQhist(i)->updateQhistory(ppoplist);
}
