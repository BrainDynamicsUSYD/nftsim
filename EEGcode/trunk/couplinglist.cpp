/***************************************************************************
                          couplinglist.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include"couplinglist.h"
#include"coupling.h"

//
// Constructor for Couplinglist creates a linked list of wave equation objects
//

Couplinglist::Couplinglist(int numconnects):numcoup(numconnects){
  Coupling *currentlink;
  Coupling *nextlink;
  firstlink = new Coupling();
  numconnects--;
  currentlink=firstlink;
  for(; numconnects>0; numconnects--){
    nextlink= new Coupling();
    currentlink->next=nextlink;
    currentlink=nextlink;
    }
}

//
// Destructor sucessively deletes each object in the linked list
//

Couplinglist::~Couplinglist(){
  int i=numcoup;
  for(;i>0; i--)
    delete getcoup(i-1);
}

//
// getcoup method returns a pointer to the "index"th wave equation object in the list
//

Coupling * Couplinglist::getcoup(int index){
  Coupling *currentlink;
  if(index<numcoup){
    currentlink=firstlink;
    for(;index>0; index--)
      currentlink=currentlink->next;
    }
  else {
    currentlink=0;
    }
  return currentlink;
 }

void Couplinglist::init(ifstream& inputf){
  for(int i=0; i<numcoup; i++)
    getcoup(i)->init(inputf);
}

void Couplinglist::dump(ofstream& dumpf){
  for(int i=0; i<numcoup; i++){
    dumpf << "Coupling " << (i+1) << " ";
    getcoup(i)->dump(dumpf);
    dumpf << endl; //Append endl to coupling data
  }
}

void Couplinglist::restart(ifstream& restartf){
  for(int i=0; i<numcoup; i++){
    getcoup(i)->restart(restartf);
    restartf.ignore(200,32); // throwaway endl of each coupling data 
  }
}

//
// updateP method updates P via each coupling object 

void Couplinglist::updateP(float **P, float **Eta, long nodes){
  for(int i=0;i<numcoup;i++){
    getcoup(i)->updatePa(P[i],Eta[i],nodes);
    }
}

