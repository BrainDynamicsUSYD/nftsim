/***************************************************************************
                          coupling.cpp  -  description
                             -------------------
   copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "coupling.h"

Coupling::Coupling():nuobj("Nu"){
}
Coupling::~Coupling(){
}

void Coupling::init(Istrm& inputf){
  nuobj.init(inputf);
}

void Coupling::dump(ofstream& dumpf){
  nuobj.dump(dumpf);
}

void Coupling::restart(Istrm& restartf){
  nuobj.restart(restartf);
}

//
// Sum the coupling terms
//
void Coupling::updatePa(float *Pa, float *Etaa, long nodes){
  nu=nuobj.get();
  for(int i=0; i<nodes; i++)
    *Pa++=nu*(*Etaa++);
}
