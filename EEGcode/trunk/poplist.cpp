/***************************************************************************
                          poplist.cpp  -  Poplist is an object holding
			                  an array of (neural) populations
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "poplist.h"

// Constructor for Poplist creates an array of (neural) populations
Poplist::Poplist(long nodes, int numberofpops, ConnectMat& connectmat): numpops(numberofpops){
  poparray = new Population *[numpops];
  for(int i=0;i<numpops;i++){
    poparray[i] = new Population(nodes,i,connectmat);
  }
}

// Destructor deletes each population object and then array which holds them
Poplist::~Poplist(){
  for(int i=0;i<numpops; i++)
    delete &get(i);
  delete [ ] poparray;
}

// get method returns a pointer to the "index"th Population in the population list
inline Population& Poplist::get(int index){ return *poparray[index]; }

void Poplist::init(Istrm& inputf,PropagNet& propagnet,ConnectMat& connectmat){
  for(int i=0; i<numpops; i++)
    get(i).init(inputf,propagnet,connectmat);
}

void Poplist::dump(ofstream& dumpf){
  for(int i=0; i<numpops; i++)
    get(i).dump(dumpf);
}

void Poplist::restart(Istrm& restartf,PropagNet& propagnet, ConnectMat& connectmat){
  for(int i=0; i<numpops; i++)
    get(i).restart(restartf,propagnet,connectmat);
}

// Step forward in time each of the neural populations method
void Poplist::stepPops(double timestep){
  for(int i=0; i<numpops; i++)
    get(i).stepPop(timestep);
}
