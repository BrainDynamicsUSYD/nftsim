/***************************************************************************
                          coupling.cpp  -  description
                             -------------------
   copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "coupling.h"

Coupling::Coupling(long numnodes, double deltat)
  :nodes(numnodes),nuobj("Nu"){
}

Coupling::~Coupling(){
}

void Coupling::init(Istrm& inputf, int coupleid){
  nuobj.init(inputf);
}

void Coupling::dump(ofstream& dumpf){
  nuobj.dump(dumpf);
}

void Coupling::restart(Istrm& restartf, int coupleid){
  nuobj.restart(restartf);
}

//
// Sum the coupling terms
//
void Coupling::updatePa(double *Pa, double *Etaa){
  nu=nuobj.get();
  for(int i=0; i<nodes; i++)
    *Pa++=nu*(*Etaa++);
}

void Coupling::output(){
}
