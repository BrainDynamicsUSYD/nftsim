/***************************************************************************
                          couplinglist.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include"couplinglist.h"
#include"coupling.h"

//
// Constructor for Couplinglist creates an array of coupling objects
//

Couplinglist::Couplinglist(int numconnects):numcoup(numconnects){
  couparray = new Coupling *[numcoup];
  for(int i=0;i<numcoup;i++){
    couparray[i] = new Coupling();
  }
}

//
// Destructor deletes each coupling object and then array which holds them
//

Couplinglist::~Couplinglist(){
  for(int i=0;i<numcoup; i++)
    delete getcoup(i);
  delete [ ] couparray;
}

//
// getcoup method returns a pointer to the "index"th coupling object in the list
//

Coupling * Couplinglist::getcoup(int index){
  return couparray[index];
}

void Couplinglist::init(Istrm& inputf){
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

void Couplinglist::restart(Istrm& restartf){
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

