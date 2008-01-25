/***************************************************************************
                          poplist.cpp  -  Poplist is an object holding
			                  a linked list of (neural) populations
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "poplist.h"

//
// Constructor for Poplist creates a linked list of (neural) populations
//
Poplist::Poplist(long nodes, int numberofpops, ConnectMat * pconnectmat): numpops(numberofpops){
  Population *currentlink;
  Population *nextlink;
  int popindex=0;
  firstlink = new Population(nodes,popindex,pconnectmat);
  numberofpops--, popindex++;
  currentlink=firstlink;
  for(; numberofpops>0; numberofpops--, popindex++){
    nextlink= new Population(nodes,popindex,pconnectmat);
    currentlink->next=nextlink;
    currentlink=nextlink;
    }
}

//
// Destructor sucessively deletes each population object in the linked list
//
Poplist::~Poplist(){
  for(int i=numpops;i>0; i--)
    delete get(i-1);
}

//
// get method returns a pointer to the "index"th Population in the population list
//
Population * Poplist::get(int index){
  Population *currentlink;
  if(index<numpops){
    currentlink=firstlink;
    for(;index>0; index--)
      currentlink=currentlink->next;
    }
  else {
    currentlink=0;
    }
  return currentlink;
 }

//
// init method
//
void Poplist::init(ifstream& inputf, PropagNet *ppropagnet, ConnectMat *pconnectmat){
  for(int i=0; i<numpops; i++)
    get(i)->init(inputf, ppropagnet, pconnectmat);
}

//
// dump method
//
void Poplist::dump(ofstream& dumpf){
  for(int i=0; i<numpops; i++)
    get(i)->dump(dumpf);
}

//
// restart method
//
void Poplist::restart(ifstream& restartf, PropagNet *ppropagnet, ConnectMat *pconnectmat){
  for(int i=0; i<numpops; i++)
    get(i)->restart(restartf, ppropagnet, pconnectmat);
}

//
// Step forward in time each of the neural populations method
//
void Poplist::stepPops(float timestep){
  for(int i=0; i<numpops; i++)
    get(i)->stepPop(timestep);
}
