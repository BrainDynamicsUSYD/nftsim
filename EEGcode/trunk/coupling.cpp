/***************************************************************************
                          coupling.cpp  -  description
                             -------------------
   copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "coupling.h"

Coupling::Coupling(long numnodes, double deltat)
  :nodes(numnodes){
}

Coupling::~Coupling(){
  delete nuobj;
}

void Coupling::init(Istrm& inputf, int coupleid){
  nuobj = new Parameter("Nu",inputf);
}

void Coupling::dump(ofstream& dumpf){
  nuobj->dump(dumpf);
}
//
// Sum the coupling terms
//
void Coupling::updatePa(double * __restrict__ Pa,double * __restrict__ Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat){
  double nu=nuobj->get();
  long n=nodes;
  for(int i=0; i<n; i++)
    Pa[i]=nu*Etaa[i];
}

void Coupling::output(){
}
