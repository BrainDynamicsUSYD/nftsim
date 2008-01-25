/***************************************************************************
                          poplist.cpp  -  Poplist is an object holding
			                  an array of (neural) populations
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "poplist.h"

//
// Constructor for Poplist creates an array of (neural) populations
//
Poplist::Poplist(long nodes, int numberofpops, ConnectMat * pconnectmat): numpops(numberofpops){
  poparray = new Population *[numpops];
  for(int i=0;i<numpops;i++){
    poparray[i] = new Population(nodes,i,pconnectmat);
  }
}

//
// Destructor deletes each population object and then array which holds them
//
Poplist::~Poplist(){
  for(int i=0;i<numpops; i++)
    delete get(i);
  delete [ ] poparray;
}

//
// get method returns a pointer to the "index"th Population in the population list
//
Population * Poplist::get(int index){
  return poparray[index];
 }

//
// init method
//
void Poplist::init(Istrm& inputf, PropagNet *ppropagnet, ConnectMat *pconnectmat){
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
void Poplist::restart(Istrm& restartf, PropagNet *ppropagnet, ConnectMat *pconnectmat){
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
